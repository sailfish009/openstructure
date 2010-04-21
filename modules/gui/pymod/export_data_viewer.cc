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
#include <ost/gui/data_viewer/overlay_manager.hh>

#include "sip_handler.hh"

using namespace boost::python;
using namespace ost::img;
using namespace ost;
using namespace ost::img::gui;
using namespace ost::gui;

namespace {

void add_dock1(DataViewer* p, QWidget* w, const String& name)
{
  p->AddDockWidget(w,QString::fromStdString(name));
}

void add_dock2(DataViewer* p, QWidget* w, const QString& name)
{
  p->AddDockWidget(w,name);
}

void add_dock3(DataViewer* p, QWidget* w, const String& name, bool s)
{
  p->AddDockWidget(w,QString::fromStdString(name),s);
}

void add_dock4(DataViewer* p, QWidget* w, const QString& name, bool s)
{
  p->AddDockWidget(w,name,s);
}

} // anon ns

BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS(o_AddOverlay, AddOverlay, 1, 2)

void export_data_viewer()
{
  class_<DataViewer, boost::noncopyable >("DataViewer",no_init)
    .def("SetData",&DataViewer::SetData)
    .def("SetName",&DataViewer::SetName)
    .def("GetOverlayManager",&DataViewer::GetOverlayManager)
    .def("GetNormalizer",&DataViewer::GetNormalizer,
	 return_value_policy<return_by_value>())
    .def("Renormalize",&DataViewer::Renormalize)
    .def("AddOverlay",&DataViewer::AddOverlay,o_AddOverlay())
    .def("ClearOverlays",&DataViewer::ClearOverlays)
    .def("GetSelection",&DataViewer::GetSelection)
    .def("UpdateView",&DataViewer::UpdateView)
    .def("Recenter",&DataViewer::Recenter)
    .def("AddDockWidget",add_dock1)
    .def("AddDockWidget",add_dock2)
    .def("AddDockWidget",add_dock3)
    .def("AddDockWidget",add_dock4)
    .def("RemoveDockWidget",&DataViewer::RemoveDockWidget)
    .def("SetAntialiasing",&DataViewer::SetAntialiasing)
    .def("Show",&DataViewer::show)
    .def("Hide", &DataViewer::hide)
    .def("GetQObject",&get_py_qobject<DataViewer>)
    .add_property("qobject", &get_py_qobject<DataViewer>)
    /*
    .def("SetAutoNormalize",&DataViewer::SetAutoNormalize)
    .def("ShowClickedPosition",&DataViewer::ShowClickedPosition,o_ShowClickedPosition())
    .def("GetClickedPosition",&DataViewer::GetClickedPosition)
    */
    ;
}
