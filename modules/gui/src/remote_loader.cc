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
#include <vector>

#include <QString>
#include <QSettings>
#include <QNetworkReply>
#include <QHBoxLayout>
#include <QDir>

#include <ost/platform.hh>

#include <ost/gui/file_loader.hh>
#include <ost/gui/loader_manager.hh>

#include "widget_registry.hh"
#include "remote_loader.hh"

namespace ost { namespace gui {

RemoteLoader::RemoteLoader(QWidget* parent):
 Widget(NULL, parent)
{
  img_support_=OST_IMG_ENABLED;
  line_edit_ = new QLineEdit(this);
  button_ = new QPushButton("Load",this);
  progress_bar_ = new QProgressBar(this);
  progress_bar_->setVisible(false);
  progress_bar_->setRange(0,0);

  QHBoxLayout* l=new QHBoxLayout(this);
  l->addWidget(line_edit_);
  l->addWidget(button_);
  l->addWidget(progress_bar_);
  l->setMargin(0);
  l->setSpacing(0);

  this->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
  this->setMinimumHeight(progress_bar_->height());
  this->setMaximumHeight(progress_bar_->height());
  connect(button_, SIGNAL(clicked()), this,
          SLOT(Clicked()));

  QDir icon_path(GetSharedDataPath().c_str());
  icon_path.cd("gui");
  icon_path.cd("icons");
  site_loader_menu_ = new QMenu(this);
  site_actions_ = new QActionGroup(site_loader_menu_);

  site_loader_menu_ = new QMenu(this);
  site_actions_ = new QActionGroup(site_loader_menu_);

  QAction* select_url_action = new QAction(this);
  select_url_action->setText("URL");
  select_url_action->setToolTip("Select remote URL");
  select_url_action->setIcon(QIcon(icon_path.absolutePath()+QDir::separator()+QString("site_icon.png")));
  action_list_.append(select_url_action);

  connect(select_url_action, SIGNAL(triggered(bool)), this, SLOT(UrlClick()));
  this->BuildMenu();
  this->RenameButton();
}

void RemoteLoader::UrlClick()
{
  this->BuildMenu();
  site_loader_menu_->exec(QCursor::pos());
}

void RemoteLoader::BuildMenu(String active_loader)
{
  if(!active_loader.empty()){
    selected_site_loader_ = active_loader;
  }
  else if(site_actions_->checkedAction()!=NULL){
    selected_site_loader_=site_actions_->checkedAction()->text().toStdString();
  }
  site_loader_menu_->clear();
  QList<QAction*> actions = site_actions_->actions();
  for(int i=0;i<actions.size();i++){
    site_actions_->removeAction(actions[i]);
  }
  std::vector<String> site_loaders = FileLoader::GetSiteLoaderIdents();
  for(unsigned int i=0; i<site_loaders.size(); i++){
    QString loader_ident(site_loaders[i].c_str());
    RemoteSiteLoader* loader = FileLoader::GetLoaderManager()->GetRemoteSiteLoader(loader_ident);
    if(loader && ((loader->IsImg() && img_support_) || !loader->IsImg())){
      QAction* action = new QAction(loader_ident,site_loader_menu_);
      connect(action, SIGNAL(triggered()), this, SLOT(RenameButton()));
      action->setCheckable(true);
      site_actions_->addAction(action);
      if((site_actions_->checkedAction()==NULL && loader_ident == FileLoader::GetLoaderManager()->GetDefaultRemoteSiteIdent()) ||selected_site_loader_==loader_ident.toStdString() ){
        action->setChecked(true);
      }
      site_loader_menu_->addAction(action);
    }
  }
}

ActionList RemoteLoader::GetActions()
{
  return action_list_;
}

bool RemoteLoader::Save(const QString& prefix)
{
  this->BuildMenu();
  QSettings settings;
  settings.beginGroup(prefix);
  settings.setValue("loader", site_actions_->checkedAction()->text());
  settings.endGroup();
  return true;
}

bool RemoteLoader::Restore(const QString& prefix)
{
  QSettings settings;
  settings.beginGroup(prefix);
  if (settings.contains("loader")) {
    this->BuildMenu(settings.value("loader").toString().toStdString());
  }
  else{
    this->BuildMenu();
  }
  this->RenameButton();
  return true;
}

void RemoteLoader::Clicked()
{
  this->BuildMenu();
  if(!line_edit_->text().isEmpty() && site_actions_->checkedAction()){
    RemoteSiteLoader* loader = FileLoader::GetLoaderManager()->GetRemoteSiteLoader(site_actions_->checkedAction()->text());

    QString text = line_edit_->text().simplified();
    QString id = text;
    QString selection = "";
    int pos = text.indexOf('[');
    if(pos >= 0){
      id = text.left(pos);
      selection = text.right(text.size()-(pos+1));
      pos = selection.lastIndexOf(']');
      if(pos>=0){
        selection = selection.left(pos);
      }
    }
    QNetworkReply* network_reply = loader->ById(id,selection);
    if(network_reply){
      progress_bar_->reset();
      connect(network_reply, SIGNAL(downloadProgress(qint64,qint64)), this, SLOT(UpdateProgress(qint64,qint64)));
      connect(network_reply, SIGNAL(finished()), this, SLOT(DownloadFinished()));
      this->ShowProgressBar(true);
    }
    line_edit_->setText("");
    line_edit_->setFocus();
  }
}

void RemoteLoader::keyPressEvent(QKeyEvent* event){
  if (event->key() == Qt::Key_Return || event->key() == Qt::Key_Enter) {
    this->Clicked();
  }
}

void RemoteLoader::ShowProgressBar(bool visibility){
  button_->setVisible(!visibility);
  line_edit_->setVisible(!visibility);
  progress_bar_->setVisible(visibility);
}

void RemoteLoader::UpdateProgress(qint64 read, qint64 total){
  progress_bar_->setMaximum(total);
  progress_bar_->setValue(read);

}

void RemoteLoader::DownloadFinished(){
  this->ShowProgressBar(false);
}


void RemoteLoader::RenameButton(){
  if(site_actions_->checkedAction()){
    QString text = "Load (" + site_actions_->checkedAction()->text() + ")";
    button_->setText(text);
  }
}

OST_REGISTER_WIDGET_WITH_DEFAULT_FACTORY(ost::gui, RemoteLoader, "Remote Loader");

}}

