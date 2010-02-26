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

#include <ost/gui/module_config.hh> // for round

#include <cmath>
#include "plot_ticker.hh"

PlotTickerBase::PlotTickerBase():
  min_(0.0),
  max_(1.0),
  start_(0),
  end_(100),
  separation_(20)
{
}
PlotTickerBase::PlotTickerBase(Real min,Real max,int start,int end,unsigned int separation):
  min_(min),
  max_(max),
  start_(start),
  end_(end),
  separation_(separation)
{
}

PlotTickerBase::~PlotTickerBase()
{
}


PlotTickerLinear::PlotTickerLinear():
  PlotTickerBase()
{
}

PlotTickerLinear::PlotTickerLinear(Real min,Real max,int start,int end,unsigned int separation):
  PlotTickerBase(min,max,start,end,separation)
{
}
PlotTickerLinear::~PlotTickerLinear()
{
}

TickList PlotTickerLinear::GetTicks()
{
  TickList result;
  Real sampling=static_cast<Real>(end_-start_)/(max_-min_);
  int exponent=static_cast<int>(floor(log10( 1.0/sampling )));
  Real prefac=1;
  while(1){
    //std::cout << sampling << " " << exponent << std::endl;
    int prefacs[]={1,2,5};
    for(int i=0;i<3 && prefac*pow(10.0,exponent)*sampling<separation_;++i){
      prefac=prefacs[i];
    }
    if(prefac*pow(10.0,exponent)*sampling>=separation_){
      break;
    }else {
      prefac=1;
      ++exponent;
    }
  }
  Real stepsize=prefac*pow(Real(10.0),static_cast<Real>(exponent));
  Real val=rint(min_/stepsize)*stepsize;
  while(val<=max_){
    int pos=static_cast<int>(round(sampling*(val-min_)))+start_;
    if(pos>=0){
      result.push_back(Tick(pos,val));
    }
    val+=stepsize;
  }
  return result;
}

