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

#include "plot_data_info.hh"
#include "plot_data_graphics_item.hh"
#include "plot_graphics_rectangle_item.hh"
#include <QCursor>
#include <QToolTip>

namespace ost {namespace gui {

PlotGraphicsRectangleItem::PlotGraphicsRectangleItem(const PlotDataEntry& entry,const PlotDataInfoPtr& info , QGraphicsItem * parent):
  QGraphicsRectItem(-static_cast<Real>(info->symbolsize)/2.0, -static_cast<Real>(info->symbolsize)/2.0, info->symbolsize, info->symbolsize,parent),
  pde_(entry)
{
  setPos(pde_.x,pde_.y);
  setPen(info->pen);
  setBrush(info->brush);
  setFlags(QGraphicsItem::ItemIgnoresTransformations);
  setAcceptHoverEvents(true);
  setCursor(Qt::CrossCursor);
}

PlotGraphicsRectangleItem::~PlotGraphicsRectangleItem()
{
  // TODO Auto-generated destructor stub
}

void PlotGraphicsRectangleItem::hoverEnterEvent ( QGraphicsSceneHoverEvent * event )
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

void PlotGraphicsRectangleItem::hoverLeaveEvent ( QGraphicsSceneHoverEvent * event )
{
  QToolTip::showText(event->screenPos(),QString(),event->widget());
}

void PlotGraphicsRectangleItem::mousePressEvent( QGraphicsSceneMouseEvent * event )
{
  dynamic_cast<PlotDataGraphicsItem*>(parentItem())->Callback(pde_);
}

}}//ns
