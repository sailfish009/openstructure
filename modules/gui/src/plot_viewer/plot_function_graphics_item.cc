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

#include <ost/dyn_cast.hh>

#include "plot_data_info.hh"
#include "plot_function_graphics_item.hh"

namespace ost{namespace gui{

PlotFunctionGraphicsItem::PlotFunctionGraphicsItem(const PlotFunctionInfoPtr& info,const PlotFunctionPtr& functionptr, QGraphicsItem * parent):
  PlotDataGraphicsItemBase(info,parent),
  functionptr_(functionptr)
{
}

PlotFunctionGraphicsItem::~PlotFunctionGraphicsItem()
{
}



void PlotFunctionGraphicsItem::Redraw()
{
  unsigned int zvalue=0;
  qDeleteAll(childItems());
  PlotFunctionInfoPtr infoptr=dyn_cast<PlotFunctionInfo>(infoptr_);
  switch(infoptr->mode){
  case PlotInfoBase::LINES:
  case PlotInfoBase::POINTSANDLINES:
  case PlotInfoBase::POINTS:
    //draw lines
  {
    Real xold=infoptr->start;
    Real yold=functionptr_->Func(xold);
    for (Real x=xold+infoptr->delta;x<=infoptr->end;x+=infoptr->delta) {
      Real y=functionptr_->Func(x);
      QGraphicsLineItem* line=new QGraphicsLineItem(xold,yold,x,y,this);
      line->setZValue(++zvalue);
      line->setPen(infoptr->pen);
      xold=x;
      yold=y;
    }
    break;
  }
  case PlotInfoBase::AREA:
  case PlotInfoBase::POINTSANDAREA:
    //draw area
    break;
  default:
    break;
  }
}




}}
