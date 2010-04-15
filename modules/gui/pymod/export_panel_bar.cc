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

#include <ost/gui/panel_bar/panels.hh>
#include <ost/gui/panel_bar/panel_bar.hh>
#include <ost/gui/panel_bar/button_bar.hh>
#include "ost/gui/widget.hh"

#include "menu_proxy.hh"
#include "sip_handler.hh"



using namespace boost::python;
using namespace ost;
using namespace ost::gui;

namespace{
void panels_add_widget_a(Panels * panels, PanelPosition pos, const SipHandlerBase& sh, bool hidden=false)
{
  panels->AddWidget(pos,reinterpret_cast<Widget*>(sh.GetSipHandle()),hidden);
}

void panels_add_widget_b(Panels * panels, PanelPosition pos, const SipHandlerBase& sh)
{
  panels->AddWidget(pos,reinterpret_cast<Widget*>(sh.GetSipHandle()));
}

void panels_add_widget_c(Panels * panels, PanelPosition pos, Widget* w, bool hidden=false)
{
  panels->AddWidget(pos,w,hidden);
}

void panels_add_widget_d(Panels * panels, PanelPosition pos, Widget* w)
{
  panels->AddWidget(pos,w);
}

void panels_remove_widget_a(Panels * panels, Widget* w)
{
  panels->RemoveWidget(w);
}

void panels_remove_widget_b(Panels * panels, const SipHandlerBase& sh)
{
  panels->RemoveWidget(reinterpret_cast<Widget*>(sh.GetSipHandle()));
}

void panels_add_widget_to_pool_a(Panels * panels, const QString& full_name, Widget* widget)
{
  panels->AddWidgetToPool(full_name,widget);
}

void panels_add_widget_to_pool_b(Panels * panels, const QString& name, int limit=-1)
{
  panels->AddWidgetToPool(name,limit);
}

object panels_get_menu(Panels* panels)
{
  static object sip_module=import("sip");
  static object pyqt4_module=import("PyQt4.QtGui");
  object obj(MenuProxy(panels->GetMenu()));
  object sip_handle=obj.attr("GetSipHandle")();
  object qmenu = pyqt4_module.attr("QMenu");
  object menu = sip_module.attr("wrapinstance")(sip_handle, qmenu);

  return menu;
}

object panels_get_qwidget(Panels* panels)
{
  static object sip_module=import("sip");
  static object pyqt4_module=import("PyQt4.QtGui");
  object sip_handle((size_t)panels);
  object qwidget = pyqt4_module.attr("QWidget");
  object widget = sip_module.attr("wrapinstance")(sip_handle, qwidget);

  return widget;
}

void panel_bar_add_widget(PanelBar * pb, const SipHandlerBase& sh, bool hidden)
{
  pb->AddWidget(reinterpret_cast<Widget*>(sh.GetSipHandle()),hidden);
}

}

void export_PanelBar()
{
  enum_<PanelPosition>("PanelPosition")
      .value("LEFT_PANEL", LEFT_PANEL)
      .value("RIGHT_PANEL", RIGHT_PANEL)
      .value("BOTTOM_PANEL", BOTTOM_PANEL)
      .value("NONE", NONE)
      .export_values()
      ;

  class_<Panels, boost::noncopyable>("Panels", no_init)
    .def("AddWidget", &panels_add_widget_a)
    .def("AddWidget", &panels_add_widget_b)
    .def("AddWidget", &panels_add_widget_c)
    .def("AddWidget", &panels_add_widget_d)
    .def("AddWidgetByName", &Panels::AddWidgetByName)
    .def("RemoveWidget", &panels_remove_widget_a)
    .def("RemoveWidget", &panels_remove_widget_b)
    .def("GetMenu", &panels_get_menu)
    .def("GetQWidget", &panels_get_qwidget)
    .def("AddWidgetToPool", &panels_add_widget_to_pool_a)
    .def("AddWidgetToPool", &panels_add_widget_to_pool_b)
    .def("Save", &Panels::Save)
    .def("Restore", &Panels::Restore)
    .add_property("menu", &panels_get_menu)
    .add_property("qwidget", &panels_get_qwidget)
  ;

  class_<PanelBar, boost::noncopyable>("PanelBar", no_init)
    .def("AddWidget", &panel_bar_add_widget)
    .def("AddWidget", &PanelBar::AddWidget)
    .def("Restore", &PanelBar::Restore)
    .def("Save", &PanelBar::Save)
    .def("Hide", &PanelBar::hide)
    .def("Show", &PanelBar::show)
  ;

  class_<ButtonBar, boost::noncopyable>("ButtonBar", no_init)
    .def("Hide", &ButtonBar::hide)
    .def("Show", &ButtonBar::show)
  ;
}

