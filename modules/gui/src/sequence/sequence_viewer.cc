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

#include <ost/mol/chain_view.hh>
#include <ost/mol/entity_view.hh>

#include <ost/seq/sequence_handle.hh>

#include <ost/gfx/entity.hh>
#include <ost/gfx/entity_fw.hh>

#include "sequence_model.hh"

#include "sequence_viewer.hh"

#include "sequence_delegate.hh"

namespace ost { namespace gui {


SequenceViewerV2::SequenceViewerV2(QWidget* parent): Widget(NULL,parent)
{
  gfx::Scene::Instance().AttachObserver(this);
  model_ = new SequenceModel(this);

  QVBoxLayout* layout = new QVBoxLayout(this);

  seq_table_view_ = new SequenceTableView(model_);
  layout->addWidget(seq_table_view_);
  this->setLayout(layout);
  connect(model_,SIGNAL(columnsInserted(const QModelIndex&, int, int)),seq_table_view_,SLOT(columnCountChanged(const QModelIndex&, int, int)));

  seq_table_view_->horizontalHeader()->setMinimumSectionSize(2);
  seq_table_view_->verticalHeader()->setMinimumSectionSize(2);
}

void SequenceViewerV2::NodeAdded(const gfx::GfxNodeP& n)
{
  if (gfx::EntityP o=boost::dynamic_pointer_cast<gfx::Entity>(n)) {
    // extract all chains
    mol::EntityView v=o->GetView();
    for (mol::ChainViewList::const_iterator c=v.GetChainList().begin(),
         e1=v.GetChainList().end(); c!=e1; ++c) {
      mol::ChainView chain=*c;
      QString name = QString(o->GetName().c_str());
      if (chain.GetName()!="" && chain.GetName()!=" ") {
        name= name + " ("+chain.GetName().c_str()+")";
      }
      model_->InsertChain(name,chain);
    }
    seq_table_view_->resizeColumnsToContents();
    seq_table_view_->resizeRowsToContents();
  }
}

void SequenceViewerV2::NodeRemoved(const gfx::GfxNodeP& node)
{
  if (gfx::EntityP o=boost::dynamic_pointer_cast<gfx::Entity>(node)) {

  }
}

SequenceViewerV2::~SequenceViewerV2(){}

}}
