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

#include "plot_data_info.hh"

namespace ost{namespace gui{

PlotDataInfo::PlotDataInfo(PlotViewerPanel * viewer):
  PlotInfoBase(viewer),
  shape(CIRCLE),
  symbolsize(2),
  errormode(NONE),
  errorscalex(1),
  errorscaley(1),
  use_weights(false)
{
  name="Data";
}

PlotDataInfo::PlotDataInfo(const PlotDataInfo& pdi):
  PlotInfoBase(pdi),
  shape(pdi.shape),
  symbolsize(pdi.symbolsize),
  errormode(pdi.errormode),
  errorscalex(pdi.errorscalex),
  errorscaley(pdi.errorscaley),
  use_weights(pdi.use_weights)
{
}

PlotDataInfo& PlotDataInfo::SetSymbol(unsigned int val)
{
  shape=val;
  emit appearanceChanged();
  return *this;
}
PlotDataInfo& PlotDataInfo::SetSymbolSize(unsigned int val)
{
  symbolsize=val;
  emit appearanceChanged();
  return *this;
}
PlotDataInfo& PlotDataInfo::SetErrorMode(unsigned int val)
{
  errormode=val;
  emit appearanceChanged();
  return *this;
}

PlotDataInfo& PlotDataInfo::SetQuality(bool val)
{
  use_weights=val;
  emit appearanceChanged();
  return *this;
}



PlotDataInfo& PlotDataInfo::SetCallback(const boost::python::object& cb)
{
  callback=cb;
  return *this;
}


}}//ns
