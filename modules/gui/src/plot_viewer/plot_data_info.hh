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
#ifndef PLOT_DATA_INFO_HH_
#define PLOT_DATA_INFO_HH_


/*
  Author: Andreas Schenk
*/

#include <ost/gui/module_config.hh>
#include <ost/gui/plot_viewer/plot_data_info_fw.hh>
#include <boost/python.hpp>
#include <vector>

#include "plot_info_base.hh"


namespace ost{namespace gui{

class DLLEXPORT_OST_GUI PlotDataInfo : public PlotInfoBase
{
public:
    PlotDataInfo(PlotViewerPanel * viewer=0);
    PlotDataInfo(const PlotDataInfo&);
    virtual PlotDataInfo& SetSymbol(unsigned int val);
    virtual PlotDataInfo& SetSymbolSize(unsigned int val);
    virtual PlotDataInfo& SetErrorMode(unsigned int val);
    virtual PlotDataInfo& SetQuality(bool val);
    virtual PlotDataInfo& SetCallback(const boost::python::object & callback_);
    unsigned int shape;
    unsigned int symbolsize;
    unsigned int errormode;
    Real errorscalex;
    Real errorscaley;
    bool use_weights;
    boost::python::object callback;

};

typedef std::vector<PlotDataInfoPtr> PlotDataInfoVector;
}}//ns
#endif /*PLOT_DATA_INFO_HH_*/
