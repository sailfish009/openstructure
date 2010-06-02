//------------------------------------------------------------------------------
// This file is part of the OpenStructure project <www.openstructure.org>
//
// Copyright (C) 2008-2010 by the OpenStructure authors
//
// This library is free software; you can redistribute it and/or modify it under
// the terms of the GNU Lesser General Public License as published by the Free
// Software Foundation; either version 3.0 of the License, or (at your option)
// any later version.
// This library is distributed in the hope that it will be useful, but WITHOUT
// ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
// FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public License for more
// details.
//
// You should have received a copy of the GNU Lesser General Public License
// along with this library; if not, write to the Free Software Foundation, Inc.,
// 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
//------------------------------------------------------------------------------
#include <QCursor>
#include <QVBoxLayout>
#include <QDesktopServices>
#include <QDir>
#include <QHeaderView>
#include <QFileInfo>
#include <QMessageBox>
#include <QApplication>

#include <ost/platform.hh>
#include <ost/config.hh>
#include <ost/mol/mol.hh>

#include <ost/io/mol/load_entity.hh>

#include <ost/io/io_exception.hh>

#include <ost/gfx/entity.hh>
#include <ost/gfx/scene.hh>

#include <ost/gui/gosty_app.hh>
#include <ost/gui/perspective.hh>
#include <ost/gui/panel_bar/panels.hh>
#include <ost/gui/file_loader.hh>

#include <ost/gui/python_shell/python_interpreter.hh>

#if OST_IMG_ENABLED
#  include <ost/img/image_handle.hh>
#  include <ost/io/img/load_map.hh>
#  include <ost/gfx/map_iso.hh>
#endif

#include "widget_registry.hh"
#include "file_browser.hh"

namespace ost { namespace gui {

FileBrowser::FileBrowser(QWidget* parent):
 Widget(NULL, parent)
{
  QString path=QDir::currentPath();
# if defined(_MSC_VER)
    QDir dir(QCoreApplication::applicationDirPath());
    dir.cdUp();
    dir.cdUp();
    path=dir.path();
    QString example_path=path+"/share/openstructure/examples/entity";
    if (QDir(example_path).exists()) {
      path=example_path;
    } 
# elif defined(__APPLE__)
  if (path.contains("DNG.app") || path=="/") {
    QString example_path="/Applications/OpenStructure/Examples/entity";    
    if (QDir(example_path).exists()) {
      path=example_path;
    }    
  }
#endif
  this->Init(path);
}

FileBrowser::FileBrowser(const QString& path, QWidget* parent):
 Widget(NULL, parent)
{
  this->Init(path);
}

void FileBrowser::Init(const QString& path)
{
  model_=new QDirModel;
  model_->setSorting(QDir::Name|QDir::DirsFirst|QDir::IgnoreCase);

  view_=new QListView(this);
  view_->setModel(model_);
  view_->setRootIndex(model_->index(path));
  view_->setAttribute(Qt::WA_MacShowFocusRect, false);
  view_->setContextMenuPolicy(Qt::CustomContextMenu);
  connect(view_,SIGNAL(customContextMenuRequested(const QPoint&)),this,SLOT(ShowContextMenu(const QPoint&)));
  menu_= new QComboBox(this);
  UpdateMenu(path);

  QVBoxLayout* l=new QVBoxLayout(this);
  l->addWidget(menu_);
  l->addWidget(view_);
  l->setMargin(0);
  l->setSpacing(0);

  this->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
  view_->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

  connect(view_, SIGNAL(doubleClicked(const QModelIndex&)), this,
          SLOT(DoubleClicked(const QModelIndex&)));

  connect(menu_, SIGNAL(currentIndexChanged(int)), this,
            SLOT(ChangeToParentDirectory(int)));
  QDir icon_path(GetSharedDataPath().c_str());
  icon_path.cd("gui");
  icon_path.cd("icons");

  QAction* split_action = new QAction(this);
  split_action->setText("split");
  split_action->setToolTip("Split File Browser");
  split_action->setIcon(QIcon(icon_path.absolutePath()+QDir::separator()+QString("split_icon.png")));
  action_list_.append(split_action);

  connect(split_action, SIGNAL(triggered(bool)), this, SLOT(Split()));
}
ActionList FileBrowser::GetActions()
{
  return action_list_;
}

bool FileBrowser::Save(const QString& prefix)
{
  return true;
}

bool FileBrowser::Restore(const QString& prefix)
{
  return true;
}

void FileBrowser::DoubleClicked(const QModelIndex& index)
{
  LoadObject(index);
}

void FileBrowser::ChangeToParentDirectory(int index){
  model_->refresh();
  QString path = menu_->itemData(index).toString();
  view_->setRootIndex(model_->index(path));
  view_->viewport()->update();
}

void FileBrowser::UpdateMenu(const QString& path){
  menu_->clear();
  QDir directory = QDir(path);
  AddItem(directory);
  while(directory.cdUp()){
# if defined(_MSC_VER)
  QDir temp=directory;
  if (temp.cdUp()){
    AddItem(directory);    
  }
# else
    AddItem(directory);
# endif
  }
# if defined(_MSC_VER)  
  QFileInfoList drive_list=QDir::drives();
  for (int i=0;i<drive_list.size();++i) {
    AddItem(drive_list[i].dir(), drive_list[i].path());
  }
#endif
}

void FileBrowser::Split(){
  Panels* panels = GostyApp::Instance()->GetPerspective()->GetPanels();
  QString current_path = model_->filePath(view_->rootIndex());
  Widget* new_file_browser = new FileBrowser(current_path);
  panels->MoveNextTo(qobject_cast<Widget*>(this), new_file_browser);
}

void FileBrowser::AddItem(const QDir& directory, const QString& mypath){
  QVariant variant = QVariant(directory.path());
  QIcon icon = model_->fileIcon(model_->index(variant.toString()));
  if (mypath!=""){
    menu_->addItem(icon, mypath,variant);
  } else {
    menu_->addItem(icon, directory.dirName(),variant);
  }
}

void FileBrowser::LoadObject(const QModelIndex& index){
  if (index.isValid()) {
    if(model_->isDir(index)){
      view_->setRootIndex(index);
      UpdateMenu(model_->filePath(index));
    }
    else{
      QString file_name=model_->filePath(index);
      FileLoader::LoadObject(file_name);
    }
  }
}

void FileBrowser::keyPressEvent(QKeyEvent* event){
  if (event->key() == Qt::Key_F5) {
    model_->refresh();
  }
}

void FileBrowser::ShowContextMenu(const QPoint& pos){

  QModelIndex index = view_->selectionModel()->currentIndex();
  QMenu* menu = new QMenu(this);
  if(model_->isDir(index)){
    QAction* open_action = new QAction(menu);
    open_action->setText("Open");
    connect(open_action,SIGNAL(triggered(bool)),this,SLOT(LoadCurrentObject()));
    menu->addAction(open_action);
  }
  if(!model_->isDir(index)){
    QAction* load_action = new QAction(menu);
    load_action->setText("Load");
    connect(load_action,SIGNAL(triggered(bool)),this,SLOT(LoadCurrentObject()));
    menu->addAction(load_action);
    QAction* system_open_action = new QAction(menu);
    system_open_action->setText("Open with system editor");
    connect(system_open_action,SIGNAL(triggered(bool)),this,SLOT(LoadWithSystemEditor()));
    menu->addAction(system_open_action);
  }
  if(menu->actions().size()>0){
    menu->exec(QCursor::pos());
  }
}

void FileBrowser::LoadCurrentObject(){
  QModelIndex index = view_->selectionModel()->currentIndex();
  this->LoadObject(index);
}

void FileBrowser::LoadWithSystemEditor(){
  QModelIndex index = view_->selectionModel()->currentIndex();
  QString file_name=model_->filePath(index);
  std::cout << file_name.toStdString() << std::endl;
  QDesktopServices::openUrl(file_name);
}


OST_REGISTER_WIDGET_WITH_DEFAULT_FACTORY(ost::gui, FileBrowser, "File Browser");  




}}

