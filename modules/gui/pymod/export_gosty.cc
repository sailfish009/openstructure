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
#include <boost/python.hpp>
#include <boost/python/register_ptr_to_python.hpp>
#include <boost/python/suite/indexing/vector_indexing_suite.hpp>
using namespace boost::python;

#include <ost/gui/gosty_app.hh>

#include <ost/gui/gl_win.hh>
#include <ost/gui/perspective.hh>
#include <ost/gui/tools/tool_options_win.hh>

#include "transfer_ownership.hh"
#include "python_shell_proxy.hh"
#include "tool_options_win_proxy.hh"
#include "gl_win_proxy.hh"
#include "scene_win_proxy.hh"
#include "sequence_viewer_proxy.hh"
#if OST_IPLT_ENABLED
#include "data_viewer_proxy.hh"
using namespace ost::iplt::gui;
#endif

using namespace ost;
using namespace ost::gui;

namespace {

PythonShellProxy app_get_py_shell(GostyApp* app)
{
  return PythonShellProxy(app->GetPyShell());
}

ToolOptionsWinProxy app_get_tool_options_win(GostyApp* app)
{
  return ToolOptionsWinProxy(app->GetToolOptionsWin());
}

GLWinProxy app_get_gl_win(GostyApp* app)
{
  return GLWinProxy(app->GetGLWin());
}

SceneWinProxy app_get_scene_win(GostyApp* app)
{
  return SceneWinProxy(app->GetSceneWin());
}

SequenceViewerProxy app_get_seq_viewer(GostyApp* app)
{
  return SequenceViewerProxy(app->GetSequenceViewer());
}

#if OST_IPLT_ENABLED
DataViewerProxyPtr app_create_data_viewer1(GostyApp* app, const ost::iplt::Data& d, const QString& name)
{
  return DataViewerProxyPtr(new DataViewerProxy(app->CreateDataViewer(d,name)));
}
DataViewerProxyPtr app_create_data_viewer2(GostyApp* app, const ost::iplt::Data& d)
{
  return DataViewerProxyPtr(new DataViewerProxy(app->CreateDataViewer(d)));
}
#endif

}

void app_add_widget_to_app_a(GostyApp* app, const QString& ident,
                               QObject* object)
{
  if(QWidget* widget = dynamic_cast<QWidget*>(object)){
    TransferOwnership(widget);
    app->AddWidgetToApp(ident, reinterpret_cast<QWidget*>(widget));
  }
}


void export_Gosty()
{
  class_<GostyApp, boost::noncopyable>("GostyApp", no_init)
    .def("Instance", &GostyApp::Instance,
         return_value_policy<reference_existing_object>()).staticmethod("Instance")
    .def("SetAppTitle", &GostyApp::SetAppTitle)
    .def("GetPyShell", &app_get_py_shell)
    .add_property("py_shell", &app_get_py_shell)
    .def("GetGLWin", &app_get_gl_win)
    .add_property("gl_win", &app_get_gl_win)                                
    .def("GetSceneWin", &app_get_scene_win)
    .add_property("scene_win", &app_get_scene_win)                                
    .def("GetSequenceViewer", &app_get_seq_viewer)
    .add_property("seq_viewer", &app_get_seq_viewer)
    .def("GetToolOptionsWin", &app_get_tool_options_win)
    .add_property("tool_options_win", &app_get_tool_options_win)     
     #if OST_IPLT_ENABLED
    .def("CreateDataViewer", &app_create_data_viewer1)
    .def("CreateDataViewer", &app_create_data_viewer2)
    #endif
    .def("ProcessEvents", &GostyApp::ProcessEvents)           
    .add_property("perspective", 
                  make_function(&GostyApp::GetPerspective, 
                                return_value_policy<reference_existing_object>()))
    .def("AddWidgetToApp", &app_add_widget_to_app_a)
    .def("GetPerspective", &GostyApp::GetPerspective, 
         return_value_policy<reference_existing_object>())
  ;
  register_ptr_to_python<GostyApp*>();
}
