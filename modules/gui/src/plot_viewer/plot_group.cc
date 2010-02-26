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
#include <limits>

#include <ost/dyn_cast.hh>

#include "plot_group.hh"

namespace ost{namespace gui{

PlotGroup::PlotGroup():
  PlotDataBase(),
  ptr_list_()
{
}

PlotGroup::~PlotGroup()
{
}

void PlotGroup::Add(PlotDataPtr dataptr)
{
  ptr_list_.push_back(dataptr);
}
void PlotGroup::Add(PlotFunctionPtr functionptr)
{
  ptr_list_.push_back(functionptr);
}
void PlotGroup::Add(PlotGroupPtr groupptr)
{
  ptr_list_.push_back(groupptr);
}

unsigned int PlotGroup::GetSize()
{
  return ptr_list_.size();
}
PlotDataBasePtr PlotGroup::GetChild(unsigned int index)
{
  return ptr_list_[index];
}
Real PlotGroup::GetMinimumX()
{
  Real minx=std::numeric_limits<Real>::max();
  unsigned int count=0;
  for(unsigned int i=0;i<GetSize();++i){
    PlotDataPtr dataptr=dyn_cast<PlotData>(GetChild(i));
    if(dataptr!=PlotDataPtr()){
      ++count;
      minx=std::min<Real>(minx,dataptr->GetMinimumX());
    }else{
      PlotGroupPtr groupptr=dyn_cast<PlotGroup>(GetChild(i));
      if(groupptr!=PlotGroupPtr()){
        ++count;
        minx=std::min<Real>(minx,groupptr->GetMinimumX());
      }else{
        //ignore
      }
    }
  }
  if(count==0){
    return 0.0;
  }else{
    return minx;
  }
}
Real PlotGroup::GetMaximumX()
{
  Real maxx=-std::numeric_limits<Real>::max();
  unsigned int count=0;
  for(unsigned int i=0;i<GetSize();++i){
    PlotDataPtr dataptr=dyn_cast<PlotData>(GetChild(i));
    if(dataptr!=PlotDataPtr()){
      ++count;
      maxx=std::max<Real>(maxx,dataptr->GetMaximumX());
    }else{
      PlotGroupPtr groupptr=dyn_cast<PlotGroup>(GetChild(i));
      if(groupptr!=PlotGroupPtr()){
        ++count;
        maxx=std::max<Real>(maxx,groupptr->GetMaximumX());
      }else{
        //ignore
      }
    }
  }
  if(count==0){
    return 1.0;
  }else{
    return maxx;
  }
}
Real PlotGroup::GetMinimumY()
{
  Real miny=std::numeric_limits<Real>::max();
  unsigned int count=0;
  for(unsigned int i=0;i<GetSize();++i){
    PlotDataPtr dataptr=dyn_cast<PlotData>(GetChild(i));
    if(dataptr!=PlotDataPtr()){
      ++count;
      miny=std::min<Real>(miny,dataptr->GetMinimumY());
    }else{
      PlotGroupPtr groupptr=dyn_cast<PlotGroup>(GetChild(i));
      if(groupptr!=PlotGroupPtr()){
        ++count;
        miny=std::min<Real>(miny,groupptr->GetMinimumY());
      }else{
        //ignore
      }
    }
  }
  if(count==0){
    return 0.0;
  }else{
    return miny;
  }
}
Real PlotGroup::GetMaximumY()
{
  Real maxy=-std::numeric_limits<Real>::max();
  unsigned int count=0;
  for(unsigned int i=0;i<GetSize();++i){
    PlotDataPtr dataptr=dyn_cast<PlotData>(GetChild(i));
    if(dataptr!=PlotDataPtr()){
      ++count;
      maxy=std::max<Real>(maxy,dataptr->GetMaximumY());
    }else{
      PlotGroupPtr groupptr=dyn_cast<PlotGroup>(GetChild(i));
      if(groupptr!=PlotGroupPtr()){
        ++count;
        maxy=std::max<Real>(maxy,groupptr->GetMaximumY());
      }else{
        //ignore
      }
    }
  }
  if(count==0){
    return 1.0;
  }else{
    return maxy;
  }
}

}}//ns
