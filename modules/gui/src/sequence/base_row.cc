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

#include "base_row.hh"

namespace ost { namespace gui {

BaseRow::BaseRow(QObject *parent) : QObject(parent)
{ }

void BaseRow::InsertPainter(Painter* painter, int pos)
{
  if(pos == -1 || pos == painter_.size()){
    painter_.append(painter);
  }
  else if(this->IsPainterPosValid(pos)){
    painter_.insert(pos, painter);
  }
}

void BaseRow::RemovePainter(Painter* painter)
{
  painter_.removeAll(painter);
}

Painter* BaseRow::GetPainter(int pos)
{
  if(this->IsPainterPosValid(pos)){
    return painter_[pos];
  }
  return NULL;
}

int BaseRow::GetPainterCount()
{
  return painter_.size();
}

bool BaseRow::IsPainterPosValid(int pos)
{
  if(pos >= 0 && pos < painter_.size()){
    return true;
  }
  return false;
}

const PainterList& BaseRow::GetPainters() const
{
  return painter_;
}

QVariant BaseRow::GetData(int column, int role) const
{
  return QVariant();
}

bool BaseRow::SetData(int column, const QVariant& value, int role)
{
  return false;
}

Qt::ItemFlags BaseRow::Flags(int column) const
{
  return Qt::NoItemFlags;
}

void BaseRow::DoubleClicked(int column)
{ }

}}
