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


#ifndef PLOT_DATA_HH_
#define PLOT_DATA_HH_


/*
  Author: Andreas Schenk
*/

#include <vector>
#include <boost/shared_ptr.hpp>
#include <ost/gui/module_config.hh>
#include "plot_data_base.hh"
#include "plot_data_entry.hh"

namespace ost{namespace gui{

class DLLEXPORT_OST_GUI PlotData: public PlotDataBase
{
public:
  PlotData();
  virtual ~PlotData();

  virtual Real GetMinimumX();
  virtual Real GetMaximumX();
  virtual Real GetMinimumY();
  virtual Real GetMaximumY();
  
  void AddXY(Real x, Real y, const QString& info = "");
  void AddXYQ(Real x, Real y, Real Q, const QString& info = "");
  void AddXYEQ(Real x, Real y,Real e, Real q, const QString& info = "");
  void AddXYE(Real x, Real y, Real e, const QString& info = "");
  void AddXEYE(Real x, Real ex, Real y, Real ey, const QString& info = "");
  void AddXEYEQ(Real x, Real ex, Real y, Real ey,Real Q, const QString& info = "");
  void AddY(Real y,  const QString& info = "");
  void AddYQ(Real y, Real Q,  const QString& info = "");
  void AddYE(Real y, Real e,  const QString& info = "");
  void AddYEQ(Real y, Real ey,Real Q,  const QString& info = "");
  void Clear();

  PlotDataEntry GetPlotDataEntry(unsigned int idx);
  unsigned int GetCount();
  
protected:
  Real minx_;
  Real maxx_;
  Real miny_;
  Real maxy_;
  std::vector<PlotDataEntry> pde_vector_;
};

typedef boost::shared_ptr<PlotData> PlotDataPtr;

}}//ns

#endif /*PLOT_DATA_HH_*/
