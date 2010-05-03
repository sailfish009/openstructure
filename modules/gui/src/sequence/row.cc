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

#include "row.hh"

namespace ost { namespace gui {

Row::Row(QObject *parent) : QObject(parent)
{ }

void Row::InsertPainter(Painter* painter, int pos)
{
  if(pos == -1 || pos == painter_.size()){
    painter_.append(painter);
  }
  else if(this->IsPosValid(pos)){
    painter_.insert(pos, painter);
  }
}

void Row::RemovePainter(Painter* painter)
{
  painter_.removeAll(painter);
}

Painter* Row::GetPainter(int pos)
{
  if(this->IsPosValid(pos)){
    return painter_[pos];
  }
  return NULL;
}

int Row::GetPainterCount()
{
  return painter_.size();
}

bool Row::IsPosValid(int pos)
{
  if(pos >= 0 && pos < painter_.size()){
    return true;
  }
  return false;
}

const PainterList& Row::GetPainters() const
{
  return painter_;
}

}}
