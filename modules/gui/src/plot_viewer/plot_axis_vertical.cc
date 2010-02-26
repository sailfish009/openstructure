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

#include <QMouseEvent>
#include <QPainter>

#include "plot_axis_vertical.hh"

namespace ost{namespace gui{

PlotAxisVertical::PlotAxisVertical(QWidget * parent):
  PlotAxisBase(parent)
{
  SetStart(AXISWIDTH);
  SetSeparation(40);
}

PlotAxisVertical::~PlotAxisVertical()
{
}

void PlotAxisVertical::paintEvent(QPaintEvent* paint_event)
{
  SetEnd(height());
  TickList ticks=GetTicks();
  QPainter painter(this);
  painter.setRenderHint(QPainter::Antialiasing);
  for(int i=0;i<static_cast<int>(ticks.size());++i){
    Real ypos=height()-ticks[i].first;
    if(ticks[i].first<static_cast<int>(AXISWIDTH)){
      continue;
    }
    painter.drawLine(AXISWIDTH-10,ypos,AXISWIDTH-1,ypos);
    painter.drawText(QRect(2,ypos-GetSeparation()/2,AXISWIDTH-14,GetSeparation()),Qt::AlignVCenter,QString("%1").arg(ticks[i].second));
  }
}

void PlotAxisVertical::mouseMoveEvent( QMouseEvent * event )
{
  if(event->buttons() & Qt::LeftButton){
    emit translate(0.0,static_cast<Real>(event->globalPos().y()-global_mouse_pos_.y()));
    global_mouse_pos_=event->globalPos();
  }
}

}}//ns
