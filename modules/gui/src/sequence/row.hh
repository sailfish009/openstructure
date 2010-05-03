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
#ifndef OST_SEQUENCE_VIEWER_ROW
#define OST_SEQUENCE_VIEWER_ROW

/*
  Author: Stefan Scheuber
 */

#include <QObject>
#include <QModelIndex>
#include <QList>

#include "painter.hh"

namespace ost { namespace gui {

class Row : public QObject
{
  Q_OBJECT

public:
  Row(QObject *parent = 0);

  void InsertPainter(Painter* painter, int pos = -1);
  void RemovePainter(Painter* painter);

  Painter* GetPainter(int pos);
  int GetPainterCount();

private:
  bool IsPosValid(int pos);
  PainterList painter_;
};

typedef QList<Row*> RowList;

}}

#endif
