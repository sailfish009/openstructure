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
#ifndef PLOT_VEWER_PROXY_HH_
#define PLOT_VEWER_PROXY_HH_

/*
  Authors: Andreas Schenk, Johan Hebert, Ansgar Philippsen
*/

#include <boost/shared_ptr.hpp>

#include "plot_viewer_proxy_fw.hh"

//TODO add forward header to PlotData, etc. and include that
#include "plot_data.hh"
#include "plot_function.hh"
#include "plot_group.hh"
#include "plot_data_info_fw.hh"
#include "plot_function_info.hh"
#include "plot_group_info.hh"

namespace ost{namespace gui{

//fw decl
class PlotViewer;

class DLLEXPORT_OST_GUI PlotViewerProxy
{
public:
  PlotViewerProxy();
  PlotViewerProxy(PlotViewer* v);
  virtual ~PlotViewerProxy();
  void Release(PlotViewer* v);
  PlotDataInfoPtr Add(PlotDataPtr dataptr, bool fit=false);
  PlotFunctionInfoPtr Add(PlotFunctionPtr functionptr);
  PlotGroupInfoPtr Add(PlotGroupPtr groupptr, bool fit=false);
  void SetMinimumX(Real x);
  void SetMaximumX(Real x);
  void SetMinimumY(Real y);
  void SetMaximumY(Real y);
  Real GetMinimumX() const;
  Real GetMaximumX() const;
  Real GetMinimumY() const;
  Real GetMaximumY() const;
  void SetAutofit(bool fit=true);
private:
  PlotViewer* viewer_;
};

}}//ns

#endif /*PLOT_VEWER_PROXY_HH_*/
