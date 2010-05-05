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


#include <QtGui>

#include "sequence_model.hh"
#include "title_row.hh"

namespace ost { namespace gui {

TitleRow::TitleRow(QObject* parent) : BaseRow(QFont("Courier",10),parent)
{ }

QVariant TitleRow::GetData(int column, int role) const
{
  column -= 1;
  if(column<0){
    return QVariant();
  }
  else if(role==Qt::DisplayRole && column%10==9){
    return QVariant(QString::number(column+1));
  }
  else if(role==Qt::DisplayRole && column%10==0){
      return QVariant(QString::number(column));
  }
  else if (role==Qt::FontRole){
    return QVariant(QFont("Verdana",8));
  }
  else if (role==Qt::TextAlignmentRole){
    return QVariant(Qt::AlignHCenter|Qt::AlignBottom);
  }
  else if (role==Qt::SizeHintRole){
      QSize size = this->GetCellSize();
      size.setHeight(10);
      return QVariant(size);
  }
  return BaseRow::GetData(column, role);
}

Qt::ItemFlags TitleRow::Flags(int column) const
{
  if(column>=0){
    return Qt::ItemIsSelectable|Qt::ItemIsEnabled;
  }
  return BaseRow::Flags(column);
}

void TitleRow::DoubleClicked(int column)
{
 if(this->parent()){
   SequenceModel* model = qobject_cast<SequenceModel*>(this->parent()->parent());
   int rows = model->rowCount()-1;
   QItemSelection add = QItemSelection(model->index(1,column),model->index(rows,column));
   model->SelectionChanged(add,QItemSelection());
 }
}

}}
