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
#include "sequence_delegate.hh"

namespace ost { namespace gui {

SequenceDelegate::SequenceDelegate(SequenceModel* seq_model, QObject *parent)
    : QItemDelegate(parent), seq_model_(seq_model)
{
  QFontMetrics metrics = QFontMetrics(QFont("Courier",10));
  default_size = QSize(metrics.width(QString("_"))+2,metrics.height()+2);
}

void SequenceDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option,
                         const QModelIndex &index) const
{
/*  Row* row = seq_model_->GetRow(index);
  for(int i=0; i < row->GetPainterCount(); i++){
    row->GetPainter(i)->Paint(painter, option, index);
  }*/
}

QSize& SequenceDelegate::GetDefaultSize(){
  return default_size;
}

}}
