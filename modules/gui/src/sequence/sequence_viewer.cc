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

#include <QStandardItem>
#include <QVBoxLayout>

#include "sequence_viewer.hh"

#include "sequence_table_view.hh"
namespace ost { namespace gui {


SequenceViewerV2::SequenceViewerV2(QWidget* parent): Widget(NULL,parent)
{
  QStandardItemModel* model=new QStandardItemModel();

  QStandardItem* item = new QStandardItem("Best protein the world has ever seen");
  model->setItem(0, 0, item);
  item = new QStandardItem("slakdjjjjjjjjjjjjjjjjjjjjjjjjlakskdjaksdjaksldjalsdjaklsdjalskdjaskdjaksdjaksdjkasjdkasjdklasdjladjkdjfhjksgb,myxcnaiosjh3iklrjakslfjka");
  model->setItem(0, 1, item);
  item = new QStandardItem("Worst protein the world has ever seen");
  model->setItem(1, 0, item);
  item = new QStandardItem("slakdjjjjjjjjjjjjjjjjjjjjjjjjlakskdjaksdjaksldjalsdjaklsdjalskdjaskdjaksdjaksdjkasjdkasjdklasdjladjkdjfhjksgb,myxcnaiosjh3iklrjakslfjkb");
  model->setItem(1, 1, item);

  QVBoxLayout* layout = new QVBoxLayout(this);
  seq_table_view_ = new SequenceTableView(model);
  layout->addWidget(seq_table_view_);

  this->setLayout(layout);
}

SequenceViewerV2::~SequenceViewerV2(){}

}}
