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
#include <QAction>

#include <boost/python.hpp>

#include "scene_win_proxy.hh"

#include <ost/gui/scene_win/context_menu.hh>

using namespace boost::python;
using namespace ost;
using namespace ost::gui;

namespace {

void context_menu_add_action(ContextMenu* cm, object action, int types)
{
  static object sip_module=import("sip");
  static object gui_module=import("ost.gui");
  static object pyqt4_module=import("PyQt4.QtCore");

  unsigned long addr = extract<unsigned long>(sip_module.attr("unwrapinstance")(action));
  QAction* act = reinterpret_cast<QAction*>(addr);
  if(act){
    ContextActionTypes type = ContextActionTypes(types);
    cm->AddAction(act,type);
  }
}

object context_menu_get_qobject(ContextMenu* cm)
{
  static object sip_module=import("sip");
  static object pyqt4_module=import("PyQt4.QtCore");
  size_t addr = reinterpret_cast<size_t>(cm);
  object obj(addr);
  object sip_handle=obj;
  object qobject = pyqt4_module.attr("QObject");
  object object = sip_module.attr("wrapinstance")(sip_handle, qobject);

  return object;
}


}

void export_SceneWin()
{
  enum_<ContextActionType>("ContextActionType")
      .value("GFX_OBJECT", GFX_OBJECT)
      .value("ENTITY", ENTITY)
      .value("ENTITY_VIEW", ENTITY_VIEW)
      .value("CUSTOM_VIEW", CUSTOM_VIEW)
      .value("NOT_VISIBLE", NOT_VISIBLE)
      .value("NOT_HIDDEN", NOT_HIDDEN)
      .value("NOT_SCENE", NOT_SCENE)
      .value("SINGLE", SINGLE)
      .value("MULTI", MULTI)
#if OST_IMG_ENABLED
      .value("MAP", MAP)
#endif
      .export_values()
      ;

  class_<ContextMenu, boost::noncopyable>("ContextMenu", no_init)
    .def("AddAction", &context_menu_add_action)
    .def("GetQObject", &context_menu_get_qobject)
    .add_property("qobject",&context_menu_get_qobject)
  ;

  class_<SceneWinProxy, bases<SipHandlerBase> >("SceneWin")
    .def("Show", &SceneWinProxy::Show)
    .def("Hide", &SceneWinProxy::Hide)
    .def("GetContextMenu", &SceneWinProxy::GetContextMenu, return_value_policy<reference_existing_object>())
  ;

}

