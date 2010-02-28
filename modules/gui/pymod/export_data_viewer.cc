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
  Authors: Ansgar Philippsen, Andreas Schenk
*/

#include <boost/python.hpp>
#include <ost/base.hh>
#include <ost/gui/data_viewer/data_viewer.hh>
#include "data_viewer_proxy.hh"
#include <ost/gui/data_viewer/overlay_manager.hh>

using namespace boost::python;
using namespace ost::img;
using namespace ost;
using namespace ost::img::gui;

namespace {

void add_dock1(DataViewerProxy* p, QWidget* w, const ost::String& name) 
{
  p->AddDockWidget(w,QString::fromStdString(name));
}

void add_dock2(DataViewerProxy* p, QWidget* w, const QString& name) 
{
  p->AddDockWidget(w,name);
}

void add_dock3(DataViewerProxy* p, QWidget* w, const ost::String& name, bool s) 
{
  p->AddDockWidget(w,QString::fromStdString(name),s);
}

void add_dock4(DataViewerProxy* p, QWidget* w, const QString& name, bool s) 
{
  p->AddDockWidget(w,name,s);
}

} // anon ns

BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS(o_AddOverlay, AddOverlay, 1, 2)

void export_data_viewer()
{
  class_<DataViewerProxy, DataViewerProxyPtr, bases<ost::gui::SipHandlerBase> >("DataViewer",no_init)
    .def("SetData",&DataViewerProxy::SetData)
    .def("SetName",&DataViewerProxy::SetName)
    .def("GetOverlayManager",&DataViewerProxy::GetOverlayManager)
    .def("GetNormalizer",&DataViewerProxy::GetNormalizer,
	 return_value_policy<return_by_value>())
    .def("Renormalize",&DataViewerProxy::Renormalize)
    .def("AddOverlay",&DataViewerProxy::AddOverlay,o_AddOverlay())
    .def("ClearOverlays",&DataViewerProxy::ClearOverlays)
    .def("GetSelection",&DataViewerProxy::GetSelection)
    .def("UpdateView",&DataViewerProxy::UpdateView)
    .def("Recenter",&DataViewerProxy::Recenter)
    .def("AddDockWidget",add_dock1)
    .def("AddDockWidget",add_dock2)
    .def("AddDockWidget",add_dock3)
    .def("AddDockWidget",add_dock4)
    .def("RemoveDockWidget",&DataViewerProxy::RemoveDockWidget)
    .def("SetAntialiasing",&DataViewerProxy::SetAntialiasing)
    .def("Show",&DataViewerProxy::Show)
    .def("Hide", &DataViewerProxy::Hide)
    /*
    .def("SetAutoNormalize",&DataViewerProxy::SetAutoNormalize)
    .def("ShowClickedPosition",&DataViewerProxy::ShowClickedPosition,o_ShowClickedPosition())
    .def("GetClickedPosition",&DataViewerProxy::GetClickedPosition)
    */    
    ;
}
