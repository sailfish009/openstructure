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

#include "plot_function_info.hh"
#include "plot_viewer_panel.hh"

namespace ost{namespace gui{

PlotFunctionInfo::PlotFunctionInfo(PlotViewerPanel * viewer):
  PlotInfoBase(viewer),
  start(0.0),
  end(1.0),
  delta(1.0)
{
  name="Function";
  connect(viewer,SIGNAL(rangeChanged(Real,Real,Real)),this,SLOT(OnRangeChange(Real,Real,Real)));
}

PlotFunctionInfo::~PlotFunctionInfo()
{
}

void PlotFunctionInfo::OnRangeChange(Real s,Real e,Real d)
{
  start=s;
  end=e;
  delta=d;
  emit appearanceChanged();
}

}}//ns
