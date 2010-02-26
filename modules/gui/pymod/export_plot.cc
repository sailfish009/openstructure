//------------------------------------------------------------------------------
// This file is part of the OpenStructure project <www.openstructure.org>
//
// Copyright (C) 2008-2010 by the OpenStructure authors
// Copyright (C) 2003-2010 by the IPLT authors
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
  Author: Ansgar Philippsen, Andreas Schenk
*/

#include <boost/python.hpp>
#include <ost/gui/plot_viewer/plot_viewer_proxy.hh>
#include <ost/gui/plot_viewer/plot_data.hh>
#include <ost/gui/plot_viewer/plot_data_info.hh>
#include <ost/gui/plot_viewer/plot_function_info.hh>
#include <ost/gui/plot_viewer/plot_function.hh>
#include <ost/gui/plot_viewer/plot_group.hh>

using namespace boost::python;
using namespace ost;
using namespace ost::gui;

namespace {

struct PlotFunctionWrap : PlotFunction, wrapper<PlotFunction>
{
  Real  Func(Real val) const
  {
    return this->get_override("Func")(val);
  }
};


//PlotDataInfoWrappers
PlotInfoBasePtr plot_info_base_set_mode(PlotInfoBasePtr inptr, unsigned int val ){inptr->SetMode(val);return inptr;}
PlotInfoBasePtr plot_info_base_set_name(PlotInfoBasePtr inptr, const String& s ){inptr->SetName(QString::fromStdString(s));return inptr;}
PlotInfoBasePtr plot_info_base_set_visible(PlotInfoBasePtr inptr, bool val ){inptr->SetVisible(val);return inptr;}
PlotInfoBasePtr plot_info_base_set_color(PlotInfoBasePtr inptr, QColor color){inptr->SetColor(color);return inptr;}
PlotInfoBasePtr plot_info_base_set_color2(PlotInfoBasePtr inptr, int r, int g, int b){inptr->SetColor(r,g,b);return inptr;}
PlotInfoBasePtr plot_info_base_set_linecolor1(PlotInfoBasePtr inptr, QColor color ){inptr->SetLineColor(color);return inptr;}
PlotInfoBasePtr plot_info_base_set_fillcolor1(PlotInfoBasePtr inptr, QColor color){inptr->SetFillColor(color);return inptr;}
PlotInfoBasePtr plot_info_base_set_linecolor2(PlotInfoBasePtr inptr, int r, int g, int b){inptr->SetLineColor(r,g,b);return inptr;}
PlotInfoBasePtr plot_info_base_set_fillcolor2(PlotInfoBasePtr inptr, int r, int g, int b){inptr->SetFillColor(r,g,b);return inptr;}



PlotDataInfoPtr plot_data_info_set_quality(PlotDataInfoPtr inptr, bool val ){inptr->SetQuality(val);return inptr;}
PlotDataInfoPtr plot_data_info_set_symbol(PlotDataInfoPtr inptr, unsigned int val ){inptr->SetSymbol(val);return inptr;}
PlotDataInfoPtr plot_data_info_set_symbolsize(PlotDataInfoPtr inptr, unsigned int val ){inptr->SetSymbolSize(val);return inptr;}
PlotDataInfoPtr plot_data_info_set_errormode(PlotDataInfoPtr inptr, unsigned int val ){inptr->SetErrorMode(val);return inptr;}
PlotDataInfoPtr plot_data_info_set_callback(PlotDataInfoPtr inptr, object cb){inptr->SetCallback(cb);return inptr;}
PlotDataInfoPtr plot_data_info_set_name(PlotDataInfoPtr inptr, const String& name ){inptr->SetName(QString(name.c_str()));return inptr;}

PlotGroupInfoPtr plot_group_info_set_quality(PlotGroupInfoPtr inptr, bool val ){inptr->SetQuality(val);return inptr;}
PlotGroupInfoPtr plot_group_info_set_name(PlotGroupInfoPtr inptr, const String& name ){inptr->SetName(QString(name.c_str()));return inptr;}
PlotGroupInfoPtr plot_group_info_set_symbol(PlotGroupInfoPtr inptr, unsigned int val ){inptr->SetSymbol(val);return inptr;}
PlotGroupInfoPtr plot_group_info_set_symbolsize(PlotGroupInfoPtr inptr, unsigned int val ){inptr->SetSymbolSize(val);return inptr;}
PlotGroupInfoPtr plot_group_info_set_errormode(PlotGroupInfoPtr inptr, unsigned int val ){inptr->SetErrorMode(val);return inptr;}
PlotGroupInfoPtr plot_group_info_set_callback(PlotGroupInfoPtr inptr, object cb){inptr->SetCallback(cb);return inptr;}

} // ns

BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS(PlotData_AddY_Overloads,PlotData::AddY, 1, 2)
BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS(PlotData_AddYE_Overloads,PlotData::AddYE, 2,3)
BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS(PlotData_AddYQ_Overloads,PlotData::AddYQ, 2,3)
BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS(PlotData_AddYEQ_Overloads,PlotData::AddYEQ, 3,4)

BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS(PlotData_AddXY_Overloads,PlotData::AddXY, 2,3)
BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS(PlotData_AddXYQ_Overloads,PlotData::AddXYQ, 3,4)
BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS(PlotData_AddXYE_Overloads,PlotData::AddXYE, 3,4)
BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS(PlotData_AddXYEQ_Overloads,PlotData::AddXYEQ, 4,5)
BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS(PlotData_AddXEYE_Overloads,PlotData::AddXEYE, 4,5)
BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS(PlotData_AddXEYEQ_Overloads,PlotData::AddXEYEQ, 5,6)


void (PlotGroup::*PlotGroup_add1)(PlotDataPtr dataptr)=&PlotGroup::Add;
void (PlotGroup::*PlotGroup_add2)(PlotFunctionPtr functionptr)=&PlotGroup::Add;
void (PlotGroup::*PlotGroup_add3)(PlotGroupPtr groupptr)=&PlotGroup::Add;

PlotDataInfoPtr (PlotViewerProxy::*PlotViewerProxy_add1)(PlotDataPtr dataptr,bool)=&PlotViewerProxy::Add;
PlotFunctionInfoPtr (PlotViewerProxy::*PlotViewerProxy_add2)(PlotFunctionPtr functionptr)=&PlotViewerProxy::Add;
PlotGroupInfoPtr (PlotViewerProxy::*PlotViewerProxy_add3)(PlotGroupPtr groupptr,bool)=&PlotViewerProxy::Add;

BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS(PlotViewerProxy_Add_Overloads, Add, 1,2)

void export_Plot()
{
  class_<PlotFunctionWrap, boost::noncopyable>("PlotFunction")
    ;

  class_<PlotInfoBase,PlotInfoBasePtr>("PlotInfoBase",init<>())
    .def("SetMode",plot_info_base_set_mode)
    .def("SetName",plot_info_base_set_name)
    .def("SetVisible",plot_info_base_set_visible)
    .def("SetColor",plot_info_base_set_color)
    .def("SetColor",plot_info_base_set_color2)
    .def("SetFillColor",plot_info_base_set_fillcolor1)
    .def("SetLineColor",plot_info_base_set_linecolor1)
    .def("SetFillColor",plot_info_base_set_fillcolor2)
    .def("SetLineColor",plot_info_base_set_linecolor2)
    ;

  class_<PlotDataInfo,PlotDataInfoPtr, bases<PlotInfoBase> >("PlotDataInfo",init<>())
    .def("SetSymbol",plot_data_info_set_symbol)
    .def("SetSymbolSize",plot_data_info_set_symbolsize)
    .def("SetErrorMode",plot_data_info_set_errormode)
    .def("SetQuality",plot_data_info_set_quality)
    .def("SetName", plot_data_info_set_name)
    .def("SetCallback",plot_data_info_set_callback)
    ;

  class_<PlotGroupInfo,PlotGroupInfoPtr, bases<PlotInfoBase> >("PlotGroupInfo",init<>())
    .def("SetSymbol",plot_group_info_set_symbol)
    .def("SetSymbolSize",plot_group_info_set_symbolsize)
    .def("SetErrorMode",plot_group_info_set_errormode)
    .def("SetQuality",plot_group_info_set_quality)
    .def("SetCallback",plot_group_info_set_callback)
    ;

  class_<PlotFunctionInfo,PlotFunctionInfoPtr, bases<PlotInfoBase> >("PlotFunctionInfo",init<>())
    ;

  class_<PlotData,PlotDataPtr>("PlotData",init<>())
    .def("AddY",&PlotData::AddY,PlotData_AddY_Overloads())
    .def("AddYQ",&PlotData::AddYE,PlotData_AddYQ_Overloads())
    .def("AddYE",&PlotData::AddYQ,PlotData_AddYE_Overloads())
    .def("AddYEQ",&PlotData::AddYEQ,PlotData_AddYEQ_Overloads())
    .def("AddXY",&PlotData::AddXY,PlotData_AddXY_Overloads())
    .def("AddXYQ",&PlotData::AddXYQ,PlotData_AddXYQ_Overloads())
    .def("AddXYE",&PlotData::AddXYE,PlotData_AddXYE_Overloads())
    .def("AddXYEQ",&PlotData::AddXYEQ,PlotData_AddXYEQ_Overloads())
    .def("AddXEYE",&PlotData::AddXEYE,PlotData_AddXEYE_Overloads())
    .def("AddXEYEQ",&PlotData::AddXEYEQ,PlotData_AddXEYEQ_Overloads())
    .def("GetMinimumX",&PlotData::GetMinimumX)
    .def("GetMaximumX",&PlotData::GetMaximumX)
    .def("GetMinimumY",&PlotData::GetMinimumY)
    .def("GetMaximumY",&PlotData::GetMaximumY)
    ;

  class_<PlotGroup,PlotGroupPtr>("PlotGroup",init<>())
    .def("Add",PlotGroup_add1)
    .def("Add",PlotGroup_add2)
    .def("Add",PlotGroup_add3)
    ;


  {//scope

    scope PlotViewerScope =
      class_<PlotViewerProxy,PlotViewerProxyPtr>("PlotViewer", no_init)
        .def("Add",PlotViewerProxy_add1,PlotViewerProxy_Add_Overloads())
        .def("Add",PlotViewerProxy_add2)
        .def("Add",PlotViewerProxy_add3,PlotViewerProxy_Add_Overloads())
        .def("SetMinimumX", &PlotViewerProxy::SetMinimumX)
        .def("SetMaximumX", &PlotViewerProxy::SetMaximumX)
        .def("SetMinimumY", &PlotViewerProxy::SetMinimumY)
        .def("SetMaximumY", &PlotViewerProxy::SetMaximumY)
        .def("GetMinimumX", &PlotViewerProxy::GetMinimumX)
        .def("GetMaximumX", &PlotViewerProxy::GetMaximumX)
        .def("GetMinimumY", &PlotViewerProxy::GetMinimumY)
        .def("GetMaximumY", &PlotViewerProxy::GetMaximumY)
        .def("SetAutofit", &PlotViewerProxy::SetAutofit)
    ;

    enum_<PlotInfoBase::modes>("modes")
      .value("POINTS", PlotDataInfo::POINTS)
      .value("LINES", PlotDataInfo::LINES)
      .value("POINTSANDLINES", PlotDataInfo::POINTSANDLINES)
      .value("AREA", PlotDataInfo::AREA)
      .value("POINTSANDAREA", PlotDataInfo::POINTSANDAREA)
      .export_values()
      ;
    enum_<PlotInfoBase::shapes>("shapes")
      .value("CIRCLE", PlotDataInfo::CIRCLE)
      .value("RECTANGLE", PlotDataInfo::RECTANGLE)
      .value("CROSS", PlotDataInfo::CROSS)
      .value("DIAGONALCROSS", PlotDataInfo::DIAGONALCROSS)
      .export_values()
      ;
    enum_<PlotInfoBase::errormodes>("errormodes")
      .value("NONE", PlotDataInfo::NONE)
      .value("LINE", PlotDataInfo::LINE)
      .value("BOX", PlotDataInfo::BOX)
      .value("VECTOR", PlotDataInfo::VECTOR)
      .export_values()
      ;
  }//scope
}
