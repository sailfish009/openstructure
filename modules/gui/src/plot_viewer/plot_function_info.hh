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
#ifndef PLOT_FUNCTION_INFO_HH_
#define PLOT_FUNCTION_INFO_HH_

/*
  Author: Andreas Schenk
*/

#include <boost/shared_ptr.hpp>
#include "plot_info_base.hh"

namespace ost{namespace gui{

class DLLEXPORT_OST_GUI PlotFunctionInfo: public PlotInfoBase
{
  Q_OBJECT
public:
  PlotFunctionInfo(PlotViewerPanel * viewer=0);
  virtual ~PlotFunctionInfo();
  Real start;
  Real end;
  Real delta;
public slots:
  void OnRangeChange(Real start,Real end,Real delta);
};

typedef boost::shared_ptr<PlotFunctionInfo> PlotFunctionInfoPtr;

}}//ns

#endif /*PLOT_FUNCTION_INFO_HH_*/
