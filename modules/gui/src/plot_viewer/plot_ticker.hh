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
#ifndef PLOT_TICKER_HH_
#define PLOT_TICKER_HH_

/*
  Author: Andreas Schenk
*/

#include <vector>
#include <utility>

#include <ost/gui/module_config.hh>

#include "plot_ticker_fw.hh"

typedef std::pair<int,Real> Tick;
typedef std::vector<Tick> TickList;

class DLLEXPORT_OST_GUI PlotTickerBase
{
public:
  PlotTickerBase();
  PlotTickerBase(Real min,Real max=1.0,int start=0,int end=100,unsigned int separation=20);
  virtual ~PlotTickerBase();
  Real GetMinimum(){return min_;}
  void SetMinimum(Real min=0.0){min_=min;}
  Real GetMaximum(){return max_;}
  void SetMaximum(Real max=1.0){max_=max;}
  void SetMinMax(Real min=0.0,Real max=1.0){min_=min;max_=max;}
  int GetStart(){return start_;}
  void SetStart(int start=0){start_=start;}
  int GetEnd(){return end_;}
  void SetEnd(int end=100){end_=end;}
  void SetStartEnd(int start=0,int end=100){start_=start;end_=end;}
  unsigned int GetSeparation(){return separation_;}
  void SetSeparation(unsigned int separation){separation_=separation;}
  virtual TickList GetTicks()=0;
protected:
  Real min_;
  Real max_;
  int start_;
  int end_;
  unsigned int separation_;
};

class DLLEXPORT_OST_GUI PlotTickerLinear: public PlotTickerBase
{
public:
  PlotTickerLinear();
  PlotTickerLinear(Real min,Real max=1.0,int start=0,int end=100,unsigned int separation=20);
  virtual ~PlotTickerLinear();
  virtual TickList GetTicks();
};

#endif /*PLOT_TICKER_HH_*/
