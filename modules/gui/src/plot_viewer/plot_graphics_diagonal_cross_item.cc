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
  Author: Andreas Schenk
*/
#include <QCursor>
#include <QToolTip>
#include "plot_data_info.hh"
#include "plot_data_graphics_item.hh"
#include "plot_graphics_diagonal_cross_item.hh"

namespace ost {namespace gui {

PlotGraphicsDiagonalCrossItem::PlotGraphicsDiagonalCrossItem(const PlotDataEntry& entry,const PlotDataInfoPtr& info , QGraphicsItem * parent):
  QGraphicsPathItem(parent),
  pde_(entry)
{
  setPos(pde_.x,pde_.y);
  setPen(info->pen);
  setBrush(info->brush);
  setFlags(QGraphicsItem::ItemIgnoresTransformations);
  setAcceptHoverEvents(true);
  setCursor(Qt::CrossCursor);
  QPainterPath path;
  path.moveTo(-static_cast<Real>(info->symbolsize)/2.0, -static_cast<Real>(info->symbolsize)/2.0);
  path.lineTo(static_cast<Real>(info->symbolsize)/2.0, static_cast<Real>(info->symbolsize)/2.0);
  path.moveTo(static_cast<Real>(info->symbolsize)/2.0,-static_cast<Real>(info->symbolsize)/2.0);
  path.lineTo(-static_cast<Real>(info->symbolsize)/2.0,static_cast<Real>(info->symbolsize)/2.0);
  setPath(path);
}

PlotGraphicsDiagonalCrossItem::~PlotGraphicsDiagonalCrossItem()
{
  // TODO Auto-generated destructor stub
}

void PlotGraphicsDiagonalCrossItem::hoverEnterEvent ( QGraphicsSceneHoverEvent * event )
{
  QToolTip::showText(event->screenPos(),QString("x:  %1\ny:  %2\nex: %3\ney: %4\nq:  %5\ni:  %6")
                                                .arg(pde_.x)
                                                .arg(pde_.y)
                                                .arg(pde_.ex)
                                                .arg(pde_.ey)
                                                .arg(pde_.q)
                                                .arg(pde_.info),
                                                event->widget());
}

void PlotGraphicsDiagonalCrossItem::hoverLeaveEvent ( QGraphicsSceneHoverEvent * event )
{
  QToolTip::showText(event->screenPos(),QString(),event->widget());
}

void PlotGraphicsDiagonalCrossItem::mousePressEvent( QGraphicsSceneMouseEvent * event )
{
  dynamic_cast<PlotDataGraphicsItem*>(parentItem())->Callback(pde_);
}

}}//ns
