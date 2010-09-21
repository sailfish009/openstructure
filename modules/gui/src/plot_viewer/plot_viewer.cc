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

#include "plot_viewer.hh"
#include "plot_viewer_panel.hh"
#include "plot_legend_panel.hh"
#include "plot_data_info.hh"
#include "plot_data_graphics_item.hh"
#include "plot_function_graphics_item.hh"
#include "plot_group_graphics_item.hh"

#include <QHBoxLayout>
#include <QVBoxLayout>
namespace ost { namespace gui {

PlotViewer::PlotViewer(QWidget* parent,const QString& name):
  QWidget(parent),
  scene_(),
  viewer_(new PlotViewerPanel(&scene_,this)),
  legend_(new PlotLegendPanel(this)),
  model_(),
  ptr_list_()
{
  scene_.setSceneRect(-std::numeric_limits<Real>::max()/2.0,-std::numeric_limits<Real>::max()/2.0,std::numeric_limits<Real>::max(),std::numeric_limits<Real>::max());
  setWindowTitle(name);
  QHBoxLayout *hlayout = new QHBoxLayout;
  hlayout->addWidget(viewer_,1);
  hlayout->addWidget(legend_,0);
  setLayout(hlayout);
}

PlotViewer::~PlotViewer()
{
}

PlotDataInfoPtr PlotViewer::Add(PlotDataPtr dataptr, bool fit)
{
  PlotDataInfoPtr infoptr(new PlotDataInfo(viewer_));
  scene_.addItem(new PlotDataGraphicsItem(infoptr,dataptr));
  if(fit){
    viewer_->SetMinMax(dataptr->GetMinimumX(),dataptr->GetMaximumX(),dataptr->GetMinimumY(),dataptr->GetMaximumY());
  }
  legend_->AddData(infoptr);
  viewer_->update();
  return infoptr;
}

PlotFunctionInfoPtr PlotViewer::Add(PlotFunctionPtr functionptr)
{
  PlotFunctionInfoPtr infoptr(new PlotFunctionInfo(viewer_));
  scene_.addItem(new PlotFunctionGraphicsItem(infoptr,functionptr));
  legend_->AddFunction(infoptr);
  viewer_->update();
  return infoptr;
}
PlotGroupInfoPtr PlotViewer::Add(PlotGroupPtr groupptr,bool fit)
{
  PlotGroupInfoPtr infoptr(new PlotGroupInfo(viewer_));
  scene_.addItem(new PlotGroupGraphicsItem(infoptr,groupptr));
  if(fit){
    viewer_->SetMinMax(groupptr->GetMinimumX(),groupptr->GetMaximumX(),groupptr->GetMinimumY(),groupptr->GetMaximumY());
  }
  legend_->AddGroup(infoptr);
  viewer_->update();
  return infoptr;
}

void PlotViewer::SetMinimumX(Real x)
{
  viewer_->SetMinimumX(x);
}
void PlotViewer::SetMaximumX(Real x)
{
  viewer_->SetMaximumX(x);
}
void PlotViewer::SetMinimumY(Real y)
{
  viewer_->SetMinimumY(y);
}
void PlotViewer::SetMaximumY(Real y)
{
  viewer_->SetMaximumY(y);
}
Real PlotViewer::GetMinimumX() const
{
  return viewer_->GetMinimumX();
}
Real PlotViewer::GetMaximumX() const
{
  return viewer_->GetMaximumX();
}
Real PlotViewer::GetMinimumY() const
{
  return viewer_->GetMinimumY();
}
Real PlotViewer::GetMaximumY() const
{
  return viewer_->GetMaximumY();
}

void PlotViewer::SetAutofit(bool fit){
  viewer_->SetAutofit(fit);
}

}}//ns
