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

#include <QStandardItem>
#include <QVBoxLayout>
#include <QPushButton>
#include <QHeaderView>

#include <ost/mol/chain_view.hh>
#include <ost/mol/entity_view.hh>

#include <ost/seq/sequence_handle.hh>

#include <ost/gfx/entity.hh>
#include <ost/gfx/entity_fw.hh>

#include "sequence_viewer.hh"

#include "sequence_delegate.hh"

namespace ost { namespace gui {


SequenceViewerV2::SequenceViewerV2(QWidget* parent): Widget(NULL,parent)
{
  gfx::Scene::Instance().AttachObserver(this);
  QStandardItemModel* model=new QStandardItemModel();

  QVBoxLayout* layout = new QVBoxLayout(this);

  seq_table_view_ = new SequenceTableView(model);
  layout->addWidget(seq_table_view_);
  //seq_table_view_->setItemDelegateForColumn(2,new SequenceDelegate(seq_table_view_));
  this->setLayout(layout);
  connect(model,SIGNAL(columnsInserted(const QModelIndex&, int, int)),seq_table_view_,SLOT(columnCountChanged(const QModelIndex&, int, int)));
  //QPushButton* pb = new QPushButton("BLABLA");
  //seq_table_view_->setIndexWidget(model->index(0,2),pb);
  seq_table_view_->verticalHeader()->hide();
  seq_table_view_->horizontalHeader()->setStyleSheet(
      "QHeaderView::section {"
         "padding-bottom: 0px;"
         "padding-top: 0px;"
         "padding-left: 0px;"
         "padding-right: 0px;"
         "margin: 0px;"
      "}"
    );
  seq_table_view_->horizontalHeader()->setMinimumSectionSize(8);
  seq_table_view_->horizontalHeader()->setDefaultSectionSize(10);
}

void SequenceViewerV2::NodeAdded(const gfx::GfxNodeP& n)
{
  if (gfx::EntityP o=boost::dynamic_pointer_cast<gfx::Entity>(n)) {
    // extract all chains
    mol::EntityView v=o->GetView();
    for (mol::ChainViewList::const_iterator c=v.GetChainList().begin(),
         e1=v.GetChainList().end(); c!=e1; ++c) {
      mol::ChainView chain=*c;
      String seq_str;
      seq_str.reserve(chain.GetResidueCount());
      for (mol::ResidueViewList::const_iterator r=chain.GetResidueList().begin(),
           e2=chain.GetResidueList().end(); r!=e2; ++r) {
        mol::ResidueView res=*r;
        seq_str.append(1, res.GetOneLetterCode());
      }
      if (seq_str.empty()) {
        continue;
      }
      String name=o->GetName();
      if (chain.GetName()!="" && chain.GetName()!=" ") {
        name+=" ("+chain.GetName()+")";
      }
      seq::SequenceHandle seq=seq::CreateSequence(name, seq_str);
      mol::EntityView v_one_chain=v.GetHandle().CreateEmptyView();
      v_one_chain.AddChain(chain, mol::ViewAddFlag::INCLUDE_ALL);
      seq.AttachView(v_one_chain);
      QStandardItem* item = new QStandardItem(name.c_str());
      QStandardItemModel* model = qobject_cast<QStandardItemModel*>(seq_table_view_->model());
      if(model){
        int row = model->rowCount();
        model->setHeaderData(0, Qt::Horizontal, QObject::tr("") );
        model->setHeaderData(row, Qt::Vertical, QObject::tr("") );
        model->setItem(row, 0, item);
        for(int i = 0; i< seq.GetLength(); i++){
          item = new QStandardItem(QString(seq.GetOneLetterCode(i)));
          item->setTextAlignment(Qt::AlignLeft|Qt::AlignVCenter);
          item->setFont(QFont("Courier",10));
          QFontMetrics m = QFontMetrics(QFont("Courier",10));
          item->setSizeHint(QSize(m.width(QString(seq.GetOneLetterCode(i)))+6,item->sizeHint().height()));
          model->setItem(row, i+1, item);
          model->setHeaderData(i+1, Qt::Horizontal, QObject::tr("") );
        }
      }
    }
    seq_table_view_->resizeColumnsToContents();
  }
}

void SequenceViewerV2::NodeRemoved(const gfx::GfxNodeP& node)
{
  if (gfx::EntityP o=boost::dynamic_pointer_cast<gfx::Entity>(node)) {

  }
}

SequenceViewerV2::~SequenceViewerV2(){}

}}
