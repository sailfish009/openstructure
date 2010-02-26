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
#include "plot_function_info.hh"
#include "plot_data_graphics_item.hh"
#include "plot_function_graphics_item.hh"
#include "plot_group_graphics_item.hh"

namespace ost{namespace gui{

PlotGroupGraphicsItem::PlotGroupGraphicsItem(const PlotGroupInfoPtr& info,const PlotGroupPtr& groupptr, QGraphicsItem * parent):
  PlotDataGraphicsItemBase(info,parent),
  groupptr_(groupptr)
{
  for(unsigned int i=0;i<groupptr->GetSize();++i){
    PlotDataPtr dataptr=dyn_cast<PlotData>(groupptr->GetChild(i));
    if(dataptr!=PlotDataPtr()){
      PlotDataInfoPtr plotdatainfoptr(new PlotDataInfo());
      new PlotDataGraphicsItem(plotdatainfoptr,dataptr,this);
      info->Add(plotdatainfoptr);
    }else{
      PlotFunctionPtr functionptr=dyn_cast<PlotFunction>(groupptr->GetChild(i));
      if(functionptr!=PlotFunctionPtr()){
        PlotFunctionInfoPtr plotfunctioninfoptr(new PlotFunctionInfo());
        new PlotFunctionGraphicsItem(plotfunctioninfoptr,functionptr,this);
        info->Add(plotfunctioninfoptr);
      }else{
        PlotGroupPtr cgroupptr=dyn_cast<PlotGroup>(groupptr->GetChild(i));
        if(cgroupptr!=PlotGroupPtr()){
          PlotGroupInfoPtr plotgroupinfoptr(new PlotGroupInfo());
          new PlotGroupGraphicsItem(plotgroupinfoptr,groupptr);
          info->Add(plotgroupinfoptr);
        }else{
          //should never be reached
        }
      }
    }
  }
}

PlotGroupGraphicsItem::~PlotGroupGraphicsItem()
{
}

void PlotGroupGraphicsItem::Redraw()
{
  QList<QGraphicsItem *> children=childItems();
  for(int i=0;i<children.size();++i){
    dynamic_cast<PlotDataGraphicsItemBase*>(children[i])->Redraw();
  }
}

}}//ns
