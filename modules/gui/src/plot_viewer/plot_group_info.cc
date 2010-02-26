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
#include <ost/dyn_cast.hh>

#include "plot_group_info.hh"
#include "plot_data_info.hh"

namespace ost{namespace gui{

PlotGroupInfo::PlotGroupInfo(PlotViewerPanel * viewer):
  PlotInfoBase(viewer),
  children_()
{
  name="Group";
}

PlotGroupInfo::~PlotGroupInfo()
{
}

void PlotGroupInfo::Add(PlotInfoBasePtr info)
{
  children_.push_back(info);
}

unsigned int PlotGroupInfo:: GetSize()
{
  return children_.size();
}

PlotInfoBasePtr PlotGroupInfo::GetChild(unsigned int i)
{
  return children_[i];
}

PlotGroupInfo& PlotGroupInfo::SetSymbol(unsigned int val)
{
  for(PlotInfoBasePtrVector::iterator it=children_.begin();it!=children_.end();++it){
    PlotDataInfoPtr infoptr=dyn_cast<PlotDataInfo>(*it);
    if(infoptr!=PlotDataInfoPtr()){
      infoptr->SetSymbol(val);
    }
  }
  return *this;
}

PlotGroupInfo& PlotGroupInfo::SetSymbolSize(unsigned int val)
{
  for(PlotInfoBasePtrVector::iterator it=children_.begin();it!=children_.end();++it){
    PlotDataInfoPtr infoptr=dyn_cast<PlotDataInfo>(*it);
    if(infoptr!=PlotDataInfoPtr()){
      infoptr->SetSymbolSize(val);
    }
  }
  return *this;
}

PlotGroupInfo& PlotGroupInfo::SetErrorMode(unsigned int val)
{
  for(PlotInfoBasePtrVector::iterator it=children_.begin();it!=children_.end();++it){
    PlotDataInfoPtr infoptr=dyn_cast<PlotDataInfo>(*it);
    if(infoptr!=PlotDataInfoPtr()){
      infoptr->SetErrorMode(val);
    }
  }
  return *this;
}

PlotGroupInfo& PlotGroupInfo::SetQuality(bool val)
{
  for(PlotInfoBasePtrVector::iterator it=children_.begin();it!=children_.end();++it){
    PlotDataInfoPtr infoptr=dyn_cast<PlotDataInfo>(*it);
    if(infoptr!=PlotDataInfoPtr()){
      infoptr->SetQuality(val);
    }
  }
  return *this;
}

PlotGroupInfo& PlotGroupInfo::SetCallback(const boost::python::object & callback_)
{
  for(PlotInfoBasePtrVector::iterator it=children_.begin();it!=children_.end();++it){
    PlotDataInfoPtr infoptr=dyn_cast<PlotDataInfo>(*it);
    if(infoptr!=PlotDataInfoPtr()){
      infoptr->SetCallback(callback_);
    }
  }
  return *this;
}



PlotGroupInfo& PlotGroupInfo::SetMode(unsigned int val)
{
  for(PlotInfoBasePtrVector::iterator it=children_.begin();it!=children_.end();++it){
    (*it)->SetMode(val);
  }
  return *this;
}

PlotGroupInfo& PlotGroupInfo::SetColor(const QColor& color)
{
  for(PlotInfoBasePtrVector::iterator it=children_.begin();it!=children_.end();++it){
    (*it)->SetColor(color);
  }
  return *this;
}

PlotGroupInfo& PlotGroupInfo::SetColor(int r, int g, int b)
{
  for(PlotInfoBasePtrVector::iterator it=children_.begin();it!=children_.end();++it){
    (*it)->SetColor(r,g,b);
  }
  return *this;
}

PlotGroupInfo& PlotGroupInfo::SetLineColor(int r, int g, int b)
{
  for(PlotInfoBasePtrVector::iterator it=children_.begin();it!=children_.end();++it){
    (*it)->SetLineColor(r,g,b);
  }
  return *this;
}

PlotGroupInfo& PlotGroupInfo::SetFillColor(int r, int g, int b)
{
  for(PlotInfoBasePtrVector::iterator it=children_.begin();it!=children_.end();++it){
    (*it)->SetFillColor(r,g,b);
  }
  return *this;
}

PlotGroupInfo& PlotGroupInfo::SetLineColor(const QColor& color)
{
  for(PlotInfoBasePtrVector::iterator it=children_.begin();it!=children_.end();++it){
    (*it)->SetLineColor(color);
  }
  return *this;
}

PlotGroupInfo& PlotGroupInfo::SetFillColor(const QColor& color)
{
  for(PlotInfoBasePtrVector::iterator it=children_.begin();it!=children_.end();++it){
    (*it)->SetLineColor(color);
  }
  return *this;
}


}}//ns
