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
  Authors: Andreas Schenk, Johan Hebert, Ansgar Philippsen
*/

#include "plot_viewer.hh"
#include "plot_viewer_proxy.hh"
#include "plot_data_info.hh"

namespace ost{namespace gui{

PlotViewerProxy::PlotViewerProxy():
  viewer_(0)
{
}
PlotViewerProxy::PlotViewerProxy(PlotViewer* v):
  viewer_(v)
{
}

PlotViewerProxy::~PlotViewerProxy()
{
}



void PlotViewerProxy::Release(PlotViewer* v)
{
  if(viewer_==v) {
    viewer_=0;
  }
}
PlotDataInfoPtr PlotViewerProxy::Add(PlotDataPtr dataptr, bool fit)
{
  if(! viewer_) return PlotDataInfoPtr(new PlotDataInfo());
  return viewer_->Add(dataptr,fit);
}
PlotFunctionInfoPtr PlotViewerProxy::Add(PlotFunctionPtr functionptr)
{
  if(! viewer_) return PlotFunctionInfoPtr(new PlotFunctionInfo());
  return viewer_->Add(functionptr);
}
PlotGroupInfoPtr PlotViewerProxy::Add(PlotGroupPtr groupptr, bool fit)
{
  if(! viewer_) return PlotGroupInfoPtr(new PlotGroupInfo());
  return viewer_->Add(groupptr,fit);
}

void PlotViewerProxy::SetMinimumX(Real x)
{
  viewer_->SetMinimumX(x);
}
void PlotViewerProxy::SetMaximumX(Real x)
{
  viewer_->SetMaximumX(x);
}
void PlotViewerProxy::SetMinimumY(Real y)
{
  viewer_->SetMinimumY(y);
}
void PlotViewerProxy::SetMaximumY(Real y)
{
  viewer_->SetMaximumY(y);
}
Real PlotViewerProxy::GetMinimumX() const
{
  return viewer_->GetMinimumX();
}
Real PlotViewerProxy::GetMaximumX() const
{
  return viewer_->GetMaximumX();
}
Real PlotViewerProxy::GetMinimumY() const
{
  return viewer_->GetMinimumY();
}
Real PlotViewerProxy::GetMaximumY() const
{
  return viewer_->GetMaximumY();
}

void PlotViewerProxy::SetAutofit(bool fit)
{
  viewer_->SetAutofit(fit);
}
}}//ns
