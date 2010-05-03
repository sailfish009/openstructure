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

#include "seq_selection_painter.hh"

namespace ost { namespace gui {

SeqSelectionPainter::SeqSelectionPainter(QObject* parent)
    : Painter(parent)
{}

void SeqSelectionPainter::Paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index){
  painter->save();
  if (option.state & QStyle::State_HasFocus){
    painter->fillRect(option.rect, QColor(240,240,0,60));
  }
  if (option.state & QStyle::State_MouseOver){
    painter->fillRect(option.rect, QColor(240,240,240,128));
  }
  if (option.state & QStyle::State_Selected){
    painter->fillRect(option.rect, QColor(0,240,0,128));
  }
  painter->restore();
}

}}
