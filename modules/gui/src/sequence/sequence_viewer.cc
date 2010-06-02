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

/*
  Author: Stefan Scheuber
 */
#include <boost/pointer_cast.hpp>

#include <QAbstractItemView>
#include <QApplication>
#include <QClipboard>
#include <QHeaderView>
#include <QPushButton>
#include <QShortcut>
#include <QVBoxLayout>
#include <QVarLengthArray>

#include <ost/mol/chain_view.hh>
#include <ost/mol/entity_view.hh>

#include <ost/seq/sequence_handle.hh>

#include <ost/gfx/entity.hh>
#include <ost/gfx/scene.hh>
#include <ost/gfx/gfx_node_visitor.hh>

#include <ost/gui/widget_registry.hh>
#include <ost/gui/gosty_app.hh>

#include "sequence_model.hh"
#include "sequence_viewer.hh"

namespace ost { namespace gui {

class SequenceViewerV2Factory: public WidgetFactory {
public:
  SequenceViewerV2Factory() :
    WidgetFactory("ost::gui::SequenceViewerV2", "Sequence Viewer V2") {
  }

  virtual Widget* Create(QWidget* parent) {
    return GostyApp::Instance()->GetSequenceViewerV2();
  }
};

OST_REGISTER_WIDGET(SequenceViewerV2, SequenceViewerV2Factory);

struct GetNodesVisitor: public gfx::GfxNodeVisitor {
  GetNodesVisitor(): nodes_() {}
  virtual void VisitObject(gfx::GfxObj* o, const Stack& st) {
    nodes_.push_back(o->shared_from_this());
  }
  gfx::NodePtrList nodes_;
  gfx::NodePtrList GetNodes(){return nodes_;}
};

SequenceViewerV2::SequenceViewerV2(bool listen_scene, QWidget* parent): Widget(NULL,parent)
{
  if(listen_scene){
    gfx::Scene::Instance().AttachObserver(this);
  }
  model_ = new SequenceModel(this);

  QVBoxLayout* layout = new QVBoxLayout(this);
  layout->setMargin(0);
  layout->setSpacing(0);

  seq_search_bar_ = new SeqSearchBar(this);
  seq_search_bar_->hide();
  layout->addWidget(seq_search_bar_);
  QShortcut* shortcut=new QShortcut(QKeySequence(tr("Ctrl+F")), this);
  connect(shortcut, SIGNAL(activated()), this, SLOT(FindInSequence()));
  connect(seq_search_bar_, SIGNAL(Changed(const QString&, bool, const QString&)), this, SLOT(OnSearchBarUpdate(const QString&, bool, const QString&)));

  seq_table_view_ = new SequenceTableView(model_);
  layout->addWidget(seq_table_view_);
  this->setLayout(layout);
  connect(model_,SIGNAL(columnsInserted(const QModelIndex&, int, int)),seq_table_view_,SLOT(columnCountChanged(const QModelIndex&, int, int)));
  connect(model_,SIGNAL(rowsInserted(const QModelIndex&, int, int)),seq_table_view_,SLOT(rowCountChanged(const QModelIndex&, int, int)));

  seq_table_view_->horizontalHeader()->setMinimumSectionSize(2);
  seq_table_view_->verticalHeader()->setMinimumSectionSize(2);
  seq_table_view_->setSelectionMode(QAbstractItemView::ExtendedSelection);
  connect(seq_table_view_->selectionModel(), SIGNAL(selectionChanged(const QItemSelection&, const QItemSelection&)), this, SLOT(SelectionModelChanged(const QItemSelection&, const QItemSelection&)));
  connect(seq_table_view_,SIGNAL(doubleClicked(const QModelIndex&)),model_,SLOT(DoubleClicked(const QModelIndex&)));
  connect(seq_table_view_->GetStaticColumn(),SIGNAL(doubleClicked(const QModelIndex&)),this,SLOT(DoubleClicked(const QModelIndex&)));
  connect(seq_table_view_->GetStaticRow(),SIGNAL(doubleClicked(const QModelIndex&)),this,SLOT(DoubleClicked(const QModelIndex&)));
  connect(seq_table_view_,SIGNAL(CopyEvent(QKeyEvent*)),this,SLOT(CopyEvent(QKeyEvent*)));
  connect(seq_table_view_,SIGNAL(MouseWheelEvent(QWheelEvent*)),this,SLOT(MouseWheelEvent(QWheelEvent*)));

  if(listen_scene){
    gfx::GfxNodeP root_node = gfx::Scene::Instance().GetRootNode();
    GetNodesVisitor gnv;
    gfx::Scene::Instance().Apply(gnv);
    gfx::NodePtrList list = gnv.GetNodes();
    for(unsigned int i=0; i<list.size();i++){
      this->NodeAdded(list[i]);
    }
  }
}

void SequenceViewerV2::NodeAdded(const gfx::GfxNodeP& n)
{
  if (gfx::EntityP o=boost::dynamic_pointer_cast<gfx::Entity>(n)) {
    model_->InsertGfxEntity(o);
    seq_table_view_->resizeColumnsToContents();
    seq_table_view_->resizeRowsToContents();
  }
  this->UpdateSearchBar();
}

void SequenceViewerV2::NodeRemoved(const gfx::GfxNodeP& node)
{
  if (gfx::EntityP o=boost::dynamic_pointer_cast<gfx::Entity>(node)) {
    model_->RemoveGfxEntity(o);
  }
}

void SequenceViewerV2::AddAlignment(const seq::AlignmentHandle& alignment)
{
  if(alignment.GetCount()>0 && alignment.GetLength()>0){
    model_->InsertAlignment(alignment);
    seq_table_view_->resizeColumnsToContents();
    seq_table_view_->resizeRowsToContents();
  }
}

void SequenceViewerV2::RemoveAlignment(const seq::AlignmentHandle& alignment)
{
  model_->RemoveAlignment(alignment);
}

void SequenceViewerV2::UpdateSearchBar()
{
  QStringList sequence_names_;
  for(int i = 1; i< model_->rowCount(); i++){
    QString name = model_->data(model_->index(i,0),Qt::DisplayRole).toString();
    sequence_names_.append(name);
  }
  seq_search_bar_->UpdateItems(sequence_names_);
}

void SequenceViewerV2::SelectionModelChanged(const QItemSelection& sel, const QItemSelection& desel)
{
  gfx::Scene::Instance().DetachObserver(this);
  model_->SelectionChanged(sel, desel);
  gfx::Scene::Instance().AttachObserver(this);
}

void SequenceViewerV2::SelectionChanged(const gfx::GfxObjP& o,
                                      const mol::EntityView& view)
{
  disconnect(seq_table_view_->selectionModel(), SIGNAL(selectionChanged(const QItemSelection&, const QItemSelection&)), this, SLOT(SelectionModelChanged(const QItemSelection&, const QItemSelection&)));
  gfx::EntityP entity=boost::dynamic_pointer_cast<gfx::Entity>(o);
  if(entity){
    const QModelIndexList& list = model_->GetModelIndexes(entity, view);
    this->SelectList(list);
  }
  connect(seq_table_view_->selectionModel(), SIGNAL(selectionChanged(const QItemSelection&, const QItemSelection&)), this, SLOT(SelectionModelChanged(const QItemSelection&, const QItemSelection&)));
}

void SequenceViewerV2::DoubleClicked(const QModelIndex& index)
{
  disconnect(seq_table_view_->selectionModel(), SIGNAL(selectionChanged(const QItemSelection&, const QItemSelection&)), this, SLOT(SelectionModelChanged(const QItemSelection&, const QItemSelection&)));
  model_->DoubleClicked(index);
  connect(seq_table_view_->selectionModel(), SIGNAL(selectionChanged(const QItemSelection&, const QItemSelection&)), this, SLOT(SelectionModelChanged(const QItemSelection&, const QItemSelection&)));
}

void SequenceViewerV2::MouseWheelEvent(QWheelEvent* event)
{
  int delta = event->delta();
  if (event->orientation() == Qt::Vertical) {
    if(delta>0){
      model_->ZoomIn();
      seq_table_view_->viewport()->update();
      seq_table_view_->resizeColumnsToContents();
      seq_table_view_->resizeRowsToContents();
    }
    else if(delta<0){
      model_->ZoomOut();
      seq_table_view_->viewport()->update();
      seq_table_view_->resizeColumnsToContents();
      seq_table_view_->resizeRowsToContents();
    }
  }
  event->accept();
}

void SequenceViewerV2::CopyEvent(QKeyEvent* event)
{
  QItemSelectionModel* model = seq_table_view_->selectionModel();
  const QModelIndexList& list = model->selectedIndexes();
  if(list.size()>0){
    QString clipboard_string;
    QSet<int> rows;
    int min_col=model_->columnCount();
    int max_col=0;
    for(int i = 0; i < list.size(); i++){
      if(list[i].column()>max_col){
        max_col = list[i].column();
      }
      if(list[i].column()<min_col){
        min_col = list[i].column();
      }
      rows.insert(list[i].row());
    }

    bool first_row = true;
    for(int i = 1; i < model_->rowCount(); i++){
      if(rows.contains(i)){
        if(!first_row){
          clipboard_string.append("\n");
        }
        for(int j=min_col; j<=max_col; j++){
          const QModelIndex& index = model_->index(i,j);
          if(model->isSelected(index)){
            clipboard_string.append(model_->data(index,Qt::DisplayRole).toString());
          }
          else{
            clipboard_string.append('-');
          }
        }
        first_row = false;
      }
    }
    QApplication::clipboard()->setText(clipboard_string);
  }
  event->accept();
}

void SequenceViewerV2::FindInSequence()
{
  if(seq_search_bar_->isHidden()){
    seq_search_bar_->show();
  }
  else{
    seq_search_bar_->hide();
  }
}

void SequenceViewerV2::OnSearchBarUpdate(const QString& subject,
                                           bool search_in_all, const QString& name)
{
  seq_table_view_->selectionModel()->clear();
  if(search_in_all){
    const QModelIndexList& list = model_->GetModelIndexes(subject);
    this->SelectList(list);
  }
  else{
    const QModelIndexList& list = model_->GetModelIndexes(subject,name);
    this->SelectList(list);
  }
}

void SequenceViewerV2::SelectList(const QModelIndexList& list)
{
  QItemSelectionModel* model = seq_table_view_->selectionModel();
  QSet<int> rows_visited;
  for(int i = 0; i<list.size(); i++){
    int row =list[i].row();
    if(!rows_visited.contains(row)){
      model->select(list[i],QItemSelectionModel::Rows|QItemSelectionModel::Deselect);
      rows_visited.insert(row);
    }
  }
  for(int i = 0; i<list.size(); i++){
    model->select(list[i],QItemSelectionModel::Select);
  }
}

const QStringList& SequenceViewerV2::GetDisplayModes()
{
  return model_->GetDisplayModes();
}
const QStringList& SequenceViewerV2::GetDisplayModes(const seq::AlignmentHandle& alignment)
{
  return model_->GetDisplayModes(alignment);
}
const QStringList& SequenceViewerV2::GetDisplayModes(const gfx::EntityP& entity)
{
  return model_->GetDisplayModes(entity);
}

const QString& SequenceViewerV2::GetCurrentDisplayMode()
{
  return model_->GetCurrentDisplayMode();
}
const QString& SequenceViewerV2::GetCurrentDisplayMode(const seq::AlignmentHandle& alignment)
{
  return model_->GetCurrentDisplayMode(alignment);
}
const QString& SequenceViewerV2::GetCurrentDisplayMode(const gfx::EntityP& entity)
{
  return model_->GetCurrentDisplayMode(entity);
}

void SequenceViewerV2::ChangeDisplayMode(const QString& string)
{
  model_->SetDisplayMode(string);
  seq_table_view_->viewport()->update();
}

void SequenceViewerV2::ChangeDisplayMode(const seq::AlignmentHandle& alignment, const QString& string)
{
  model_->SetDisplayMode(alignment, string);
  seq_table_view_->viewport()->update();
}

void SequenceViewerV2::ChangeDisplayMode(const gfx::EntityP& entity, const QString& string)
{
  model_->SetDisplayMode(entity, string);
  seq_table_view_->viewport()->update();
}

SequenceViewerV2::~SequenceViewerV2(){
  gfx::Scene::Instance().DetachObserver(this);
}

}}
