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
#include "plot_data_graphics_item_base.hh"

namespace ost{namespace gui{

PlotDataGraphicsItemBase::PlotDataGraphicsItemBase(const PlotInfoBasePtr& info, QGraphicsItem * parent):
  QObject(),
  QGraphicsItem(parent),
  infoptr_(info),
  dirty_(true)
{  
  connect(infoptr_.get(),SIGNAL(visibilityChanged()),this,SLOT(OnVisibilityChange()));
  connect(infoptr_.get(),SIGNAL(appearanceChanged()),this,SLOT(OnAppearanceChange()));
}

PlotDataGraphicsItemBase::~PlotDataGraphicsItemBase()
{
}



void PlotDataGraphicsItemBase::paint( QPainter * painter, const QStyleOptionGraphicsItem * option, QWidget * widget)
{
  
}

QRectF PlotDataGraphicsItemBase::boundingRect() const
{
  return childrenBoundingRect();
}

void PlotDataGraphicsItemBase::Redraw()
{
  dirty_=false;
}
void PlotDataGraphicsItemBase::update ( const QRectF & rect)
{
  if(dirty_){
    dirty_=false;
    Redraw();
  }
}
void PlotDataGraphicsItemBase::OnAppearanceChange()
{
  dirty_=true;
  update();
}

void PlotDataGraphicsItemBase::OnVisibilityChange()
{
  setVisible(infoptr_->visible);
  update();
}


}}//ns
