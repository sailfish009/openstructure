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

#include "plot_axis_base.hh"

#include <QWheelEvent>
#include <QMouseEvent>
namespace ost{namespace gui{

PlotAxisBase::PlotAxisBase(QWidget * parent):
  QWidget(parent),
  ticker_(),
  global_mouse_pos_()
{
}

PlotAxisBase::~PlotAxisBase()
{
}

void PlotAxisBase::wheelEvent ( QWheelEvent * e )
{
  emit zoom(e->delta(),e->pos());
}
void PlotAxisBase::SetMinimum(Real min)
{
  ticker_.SetMinimum(min);
  update();
}

void PlotAxisBase::SetMaximum(Real max)
{
  ticker_.SetMaximum(max);
  update();
}

void PlotAxisBase::SetSeparation(int separation)
{
  ticker_.SetSeparation(separation);
  update();
}
int PlotAxisBase::GetSeparation()
{
  return ticker_.GetSeparation();

}

void PlotAxisBase::SetStart(int start)
{
  ticker_.SetStart(start);
}

void PlotAxisBase::SetEnd(int end)
{
  ticker_.SetEnd(end);
}

TickList PlotAxisBase::GetTicks()
{
  return ticker_.GetTicks();
}

void PlotAxisBase::mousePressEvent( QMouseEvent * event )
{
  if(event->button()== Qt::LeftButton){
    global_mouse_pos_=event->globalPos();
  }
}


}}//ns
