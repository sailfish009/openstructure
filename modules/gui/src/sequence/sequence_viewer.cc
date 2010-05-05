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

#include <QVBoxLayout>
#include <QPushButton>
#include <QHeaderView>
#include <QAbstractItemView>

#include <ost/mol/chain_view.hh>
#include <ost/mol/entity_view.hh>

#include <ost/seq/sequence_handle.hh>

#include <ost/gfx/entity.hh>
#include <ost/gfx/scene.hh>
#include <ost/gfx/gfx_node_visitor.hh>

#include "sequence_model.hh"

#include "sequence_viewer.hh"

#include "sequence_delegate.hh"

namespace ost { namespace gui {

struct GetNodesVisitor: public gfx::GfxNodeVisitor {
  GetNodesVisitor(): nodes_() {}
  virtual void VisitObject(gfx::GfxObj* o, const Stack& st) {
    nodes_.push_back(o->shared_from_this());
  }
  gfx::NodePtrList nodes_;
  gfx::NodePtrList GetNodes(){return nodes_;}
};

SequenceViewerV2::SequenceViewerV2(QWidget* parent): Widget(NULL,parent)
{
  gfx::Scene::Instance().AttachObserver(this);
  model_ = new SequenceModel(this);

  QVBoxLayout* layout = new QVBoxLayout(this);

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
  connect(seq_table_view_,SIGNAL(MouseWheelEvent(QWheelEvent*)),this,SLOT(MouseWheelEvent(QWheelEvent*)));

  gfx::GfxNodeP root_node = gfx::Scene::Instance().GetRootNode();
  GetNodesVisitor gnv;
  gfx::Scene::Instance().Apply(gnv);
  gfx::NodePtrList list = gnv.GetNodes();
  for(unsigned int i=0; i<list.size();i++){
    this->NodeAdded(list[i]);
  }
}

void SequenceViewerV2::NodeAdded(const gfx::GfxNodeP& n)
{
  if (gfx::EntityP o=boost::dynamic_pointer_cast<gfx::Entity>(n)) {
    model_->InsertGfxEntity(o);
    seq_table_view_->resizeColumnsToContents();
    seq_table_view_->resizeRowsToContents();
  }
}

void SequenceViewerV2::NodeRemoved(const gfx::GfxNodeP& node)
{
  if (gfx::EntityP o=boost::dynamic_pointer_cast<gfx::Entity>(node)) {
    model_->RemoveGfxEntity(o);
  }
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
  QItemSelectionModel* model = seq_table_view_->selectionModel();
  gfx::EntityP entity=boost::dynamic_pointer_cast<gfx::Entity>(o);
  if(entity){
    const QModelIndexList& list = model_->GetModelIndexes(entity, view);
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

SequenceViewerV2::~SequenceViewerV2(){
  gfx::Scene::Instance().DetachObserver(this);
}

}}
