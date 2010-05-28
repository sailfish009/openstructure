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

#include "align_properties_painter.hh"

namespace ost { namespace gui {

namespace {

QMap<QString,QColor> GetColorMap(){
  QMap<QString,QColor> map;
  map["G"]=QColor(Qt::gray);
  map["A"]=QColor(Qt::gray);
  map["V"]=QColor(Qt::gray);
  map["L"]=QColor(Qt::gray);
  map["I"]=QColor(Qt::gray);
  map["F"]=QColor(255,165,0);
  map["Y"]=QColor(255,165,0);
  map["W"]=QColor(255,165,0);
  map["C"]=QColor(Qt::yellow);
  map["M"]=QColor(Qt::yellow);
  map["S"]=QColor(Qt::green);
  map["T"]=QColor(Qt::green);
  map["K"]=QColor(Qt::red);
  map["R"]=QColor(Qt::red);
  map["H"]=QColor(Qt::red);
  map["D"]=QColor(Qt::blue);
  map["E"]=QColor(Qt::blue);
  map["N"]=QColor(Qt::blue);
  map["Q"]=QColor(Qt::blue);
  map["P"]=QColor(Qt::magenta);
  return map;
}

}

QMap<QString,QColor> AlignPropertiesPainter::color_map_ = GetColorMap();


AlignPropertiesPainter::AlignPropertiesPainter(QObject* parent)
    : Painter(parent)
{}

void AlignPropertiesPainter::Paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index){
  painter->save();
  if (index.column()>0){
    QString text = index.data(Qt::DisplayRole).toString();
    if(color_map_.contains(text)){
      painter->fillRect(option.rect, color_map_[text]);
    }
  }
  painter->restore();
}

}}
