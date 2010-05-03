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

#include "seq_secstr_painter.hh"
#include "view_object.hh"

namespace ost { namespace gui {

SeqSecStrPainter::SeqSecStrPainter(QObject* parent)
    : Painter(parent)
{}

void SeqSecStrPainter::Paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index){
  QVariant var = index.data(Qt::UserRole);
  painter->save();
  painter->setPen(QPen(Qt::lightGray));
  ListEntry entry = index.data(Qt::UserRole).value<ListEntry>();
  if(entry.secstr.size()>0){
    QSize size = index.data(Qt::UserRole+1).toSize();
    QVarLengthArray<mol::SecStructure>& sec_str = entry.secstr;
    int column = index.column()-1;
    if(column < sec_str.size()){
      mol::SecStructure sec_element = sec_str[column];

      int std_diff = (size.height()/2);
      int diff = size.height()/1.25;
      int center = option.rect.top()+(option.rect.height()/2);

      if (sec_element.IsCoil()) {
        int pos = center - std_diff;
        painter->drawLine(option.rect.left(),pos,option.rect.right(),pos);
        pos = center + std_diff;
        painter->drawLine(option.rect.left(),pos,option.rect.right(),pos);
        //stack.push_back(QPointF(s.first*advance_, -.6*height_));
        //stack.push_back(QPointF((s.last+1)*advance_,  -.6*height_));
      } else if (sec_element.IsHelical()) {
        int pos = center - diff;
        painter->drawLine(option.rect.left(),pos,option.rect.right(),pos);
        pos = center + diff;
        painter->drawLine(option.rect.left(),pos,option.rect.right(),pos);
        if(column - 1 > 0 && !sec_str[column-1].IsHelical()){
          painter->drawLine(option.rect.left(),center+std_diff,option.rect.left(),center+diff);
          painter->drawLine(option.rect.left(),center-std_diff,option.rect.left(),center-diff);
        }
        if(column + 1 < sec_str.size() && !sec_str[column+1].IsHelical()){
          painter->drawLine(option.rect.right(),center+std_diff,option.rect.right(),center+diff);
          painter->drawLine(option.rect.right(),center-std_diff,option.rect.right(),center-diff);
        }
      } else if (sec_element.IsExtended()) {
        int pos = center - diff;
        if(column - 1 > 0 && !sec_str[column-1].IsExtended()){
          painter->drawLine(option.rect.left(),center+std_diff,option.rect.left(),center+diff);
          painter->drawLine(option.rect.left(),center-std_diff,option.rect.left(),center-diff);
        }
        if(column + 1 < sec_str.size() && !sec_str[column+1].IsExtended()){
          int max_diff = size.height();
          painter->drawLine(option.rect.left(),center+diff,option.rect.left(),center+max_diff);
          painter->drawLine(option.rect.left(),center-diff,option.rect.left(),center-max_diff);
          painter->drawLine(option.rect.left(),center+max_diff,option.rect.right(),center+std_diff);
          painter->drawLine(option.rect.left(),center-max_diff,option.rect.right(),center-std_diff);
        }
        else{
          painter->drawLine(option.rect.left(),pos,option.rect.right(),pos);
          pos = center + diff;
          painter->drawLine(option.rect.left(),pos,option.rect.right(),pos);
        }
      }
      if(!(column+1 < sec_str.size())){
        painter->drawLine(option.rect.right(),center+std_diff,option.rect.right(),center-std_diff);
      }
    }
  }
  painter->restore();
}

}}
