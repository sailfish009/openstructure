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
#include <iostream>
#include "sequence_delegate.hh"

namespace ost { namespace gui {

SequenceDelegate::SequenceDelegate(SequenceTableView* view, QObject *parent)
    : QItemDelegate(parent), view_(view)
{

}

/*
QWidget *SequenceDelegate::createEditor(QWidget *parent,  const QStyleOptionViewItem &, const QModelIndex &) const
{
  QSpinBox *editor = new QSpinBox(parent);
  editor->setMinimum(0);
  editor->setMaximum(100);

  return editor;
}
*/

void SequenceDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option,
                         const QModelIndex &index) const
{
  painter->save();

  painter->setPen(QPen(Qt::NoPen));
  if(option.state & QStyle::State_Selected){
    painter->setBrush(option.palette.highlight());
  }
  else{
    painter->setBrush(QBrush(Qt::white));
  }
  painter->drawRect(option.rect);

  painter->setPen(QPen(Qt::lightGray));
  if(index.column()%2){
    painter->drawLine(option.rect.topLeft(),option.rect.bottomRight());
  }
  else{
    painter->drawLine(option.rect.bottomLeft(),option.rect.topRight());
  }
  painter->setPen(QPen(Qt::black));
  QVariant value = index.data(Qt::DisplayRole);
  if (value.isValid()){
      QString text = value.toString();
      painter->setFont(QFont("Courier",10));
      painter->drawText(option.rect, Qt::AlignLeft|Qt::AlignVCenter, text);
  }
  painter->restore();
}
}}
