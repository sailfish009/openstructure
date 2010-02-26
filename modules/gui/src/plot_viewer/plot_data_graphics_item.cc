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

#include <QtAlgorithms>

#include <ost/dyn_cast.hh>
#include <ost/gui/python_shell/python_interpreter.hh>
#include "plot_data_info.hh"
#include "plot_graphics_ellipse_item.hh"
#include "plot_graphics_rectangle_item.hh"
#include "plot_graphics_cross_item.hh"
#include "plot_graphics_diagonal_cross_item.hh"
#include "plot_data_graphics_item.hh"


namespace ost{namespace gui{

PlotDataGraphicsItem::PlotDataGraphicsItem(const PlotDataInfoPtr& info,const PlotDataPtr& dataptr, QGraphicsItem * parent):
  PlotDataGraphicsItemBase(info,parent),
  dataptr_(dataptr)
{
}

PlotDataGraphicsItem::~PlotDataGraphicsItem()
{
}


void PlotDataGraphicsItem::Redraw()
{
  unsigned int zvalue=0;
  qDeleteAll(childItems());
  if(dataptr_->GetCount()==0){
    return;
  }
  PlotDataInfoPtr infoptr=dyn_cast<PlotDataInfo>(infoptr_);
  switch(infoptr->errormode){
  case PlotDataInfo::NONE:
    break;
  case PlotDataInfo::LINE:
    break;
  case PlotDataInfo::BOX:
    break;
  case PlotDataInfo::VECTOR:
    break;
  default:
    break;
  }
  switch(infoptr->mode){
  case PlotDataInfo::LINES:
  case PlotDataInfo::POINTSANDLINES:
    //draw lines
  {
    PlotDataEntry old=dataptr_->GetPlotDataEntry(0);
    for (unsigned int i=1;i<dataptr_->GetCount();++i) {
      PlotDataEntry pde=dataptr_->GetPlotDataEntry(i);
      QGraphicsLineItem* line=new QGraphicsLineItem(old.x,old.y,pde.x,pde.y,this);
      line->setZValue(++zvalue);
      line->setPen(infoptr->pen);
      old=pde;
    }
    break;
  }
  case PlotDataInfo::AREA:
  case PlotDataInfo::POINTSANDAREA:
    //draw area
    break;
  default:
    break;
  }
  switch(infoptr->mode){
  case PlotDataInfo::POINTS:
  case PlotDataInfo::POINTSANDLINES:
  case PlotDataInfo::POINTSANDAREA:
    //draw points
    switch(infoptr->shape){
    case PlotDataInfo::CIRCLE:
      for (unsigned int i=0;i<dataptr_->GetCount();++i) {
        PlotDataEntry pde=dataptr_->GetPlotDataEntry(i);
        PlotGraphicsEllipseItem* ellipse=new PlotGraphicsEllipseItem(pde,infoptr,this);
        ellipse->setZValue(++zvalue);
      }
      break;
    case PlotDataInfo::RECTANGLE:
      for (unsigned int i=0;i<dataptr_->GetCount();++i) {
        PlotDataEntry pde=dataptr_->GetPlotDataEntry(i);
        PlotGraphicsRectangleItem* rect=new PlotGraphicsRectangleItem(pde,infoptr,this);
        rect->setZValue(++zvalue);
      }
      break;
    case PlotDataInfo::CROSS:
      for (unsigned int i=0;i<dataptr_->GetCount();++i) {
        PlotDataEntry pde=dataptr_->GetPlotDataEntry(i);
        PlotGraphicsCrossItem* cross=new PlotGraphicsCrossItem(pde,infoptr,this);
        cross->setZValue(++zvalue);
      }
      break;
    case PlotDataInfo::DIAGONALCROSS:
      for (unsigned int i=0;i<dataptr_->GetCount();++i) {
        PlotDataEntry pde=dataptr_->GetPlotDataEntry(i);
        PlotGraphicsDiagonalCrossItem* diagcross=new PlotGraphicsDiagonalCrossItem(pde,infoptr,this);
        diagcross->setZValue(++zvalue);
      }
      break;
    default:
      break;
    }
    break;
  default:
    break;
  }
}

void PlotDataGraphicsItem::Callback(const PlotDataEntry& pde)
{
  PlotDataInfoPtr infoptr=dyn_cast<PlotDataInfo>(infoptr_);
  if(infoptr->callback!=boost::python::object()){
    PythonInterpreter::Instance().CallFunction(infoptr->callback,boost::python::make_tuple(pde.x,pde.y,pde.ex,pde.ey,pde.q,boost::python::str(pde.info.toStdString().c_str())));
  }
}

}}//ns
