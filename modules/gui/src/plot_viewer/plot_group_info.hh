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
#ifndef PLOT_GROUP_INFO_HH_
#define PLOT_GROUP_INFO_HH_

#include "plot_info_base.hh"
#include <vector>
#include <boost/python.hpp>
#include <boost/shared_ptr.hpp>

namespace ost{namespace gui{

class DLLEXPORT_OST_GUI PlotGroupInfo: public PlotInfoBase
{
  typedef std::vector<PlotInfoBasePtr> PlotInfoBasePtrVector;
public:
  PlotGroupInfo(PlotViewerPanel * viewer=0);
  virtual ~PlotGroupInfo();
  virtual PlotGroupInfo& SetSymbol(unsigned int val);
  virtual PlotGroupInfo& SetSymbolSize(unsigned int val);
  virtual PlotGroupInfo& SetErrorMode(unsigned int val);
  virtual PlotGroupInfo& SetQuality(bool val);
  virtual PlotGroupInfo& SetCallback(const boost::python::object & callback_);

  virtual PlotGroupInfo& SetMode(unsigned int val);
  virtual PlotGroupInfo& SetColor(const QColor& color);
  virtual PlotGroupInfo& SetColor(int r, int g, int b);
  virtual PlotGroupInfo& SetLineColor(int r, int g, int b);
  virtual PlotGroupInfo& SetFillColor(int r, int g, int b);
  virtual PlotGroupInfo& SetLineColor(const QColor& color);
  virtual PlotGroupInfo& SetFillColor(const QColor& color);
  void Add(PlotInfoBasePtr info);
  unsigned int GetSize();
  PlotInfoBasePtr GetChild(unsigned int i);
protected:
  PlotInfoBasePtrVector children_;
};

typedef boost::shared_ptr<PlotGroupInfo> PlotGroupInfoPtr;


}}//ns


#endif /*PLOT_GROUP_INFO_HH_*/
