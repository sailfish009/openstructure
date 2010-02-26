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
#include <sstream>
#include "plot_data.hh"

namespace ost{namespace gui{

PlotData::PlotData():
  PlotDataBase(),
  minx_(std::numeric_limits<Real>().max()),
  maxx_(-std::numeric_limits<Real>().max()),
  miny_(std::numeric_limits<Real>().max()),
  maxy_(-std::numeric_limits<Real>().max()),
  pde_vector_()
{
}

PlotData::~PlotData()
{
}

void PlotData::AddXYQ(Real x, Real y, Real w, const QString& n)
{
  AddXEYEQ(x, 0.0, y, 0.0,w, n);
}
void PlotData::AddXYE(Real x, Real y, Real e, const QString& n)
{
  AddXEYEQ(x, 0.0, y, e,1.0, n);
}
void PlotData::AddXYEQ(Real x, Real y, Real e,Real q, const QString& n)
{
  AddXEYEQ(x, 0.0, y, e,q, n);
}
void PlotData::AddXEYE(Real x, Real ex, Real y, Real ey, const QString& n)
{
  AddXEYEQ(x, ex, y, ey ,1.0, n);
}
void PlotData::AddXY(Real x, Real y, const QString& n)
{
  AddXEYEQ(x, 0, y, 0,1, n);
}

void PlotData::AddYQ(Real y, Real w, const QString& n)
{
  AddYEQ(y, 0.0,w, n);
}
void PlotData::AddYE(Real y, Real e, const QString& n)
{
  AddYEQ(y, e,1.0, n);
}
void PlotData::AddY(Real y, const QString& n)
{
  AddYEQ(y, 0,1, n);
}

void PlotData::AddYEQ(Real y, Real ey, Real w, const QString& n)
{
  static int x=0;
  AddXEYEQ(x,0,y,ey,w,n);
  ++x;
}

void PlotData::AddXEYEQ(Real x, Real ex, Real y, Real ey, Real q, const QString& n)
{
  pde_vector_.push_back(PlotDataEntry(x, ex, y, ey,q, n));
  minx_=std::min(minx_, x);
  maxx_=std::max(maxx_, x);
  miny_=std::min(miny_, y);
  maxy_=std::max(maxy_, y);
}

void PlotData::Clear()
{
  pde_vector_.clear();
  minx_=std::numeric_limits<Real>().max();
  maxx_=-std::numeric_limits<Real>().max();
  miny_=std::numeric_limits<Real>().max();
  maxy_=-std::numeric_limits<Real>().max();
}
Real PlotData::GetMinimumX()
{
  if(minx_>maxx_){
    return 0.0;
  }else{
    return minx_;
  }
}
Real PlotData::GetMaximumX()
{
  if(minx_>maxx_){
    return 1.0;
  }else if(minx_==maxx_){
    return minx_+1.0;
  }else{
    return maxx_;
  }
}
Real PlotData::GetMinimumY()
{
  if(miny_>maxy_){
    return 0.0;
  }else{
    return miny_;
  }
}
Real PlotData::GetMaximumY()
{
  if(miny_>maxy_){
    return 1.0;
  }else if(miny_==maxy_){
    return miny_+1.0;
  }else{
    return maxy_;
  }
}


PlotDataEntry PlotData::GetPlotDataEntry(unsigned int idx)
{
  return pde_vector_[idx];
}

unsigned int PlotData::GetCount()
{
 return pde_vector_.size();
}

}}//ns
