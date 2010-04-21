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

#include <QObject>
#include <QMdiArea>

#include <ost/gui/main_area.hh>

#include "transfer_ownership.hh"
#include "sip_handler.hh"

using namespace boost::python;
using namespace ost;
using namespace ost::gui;

namespace {

void main_area_add_widget_a(MainArea* m, const QString& title,
                          const SipHandlerBase& sh)
{
  m->AddWidget(title, reinterpret_cast<QWidget*>(sh.GetSipHandle()));
}

void main_area_add_widget_b(MainArea* m, const QString& title, QObject* obj)
{
  if(QWidget* widget = reinterpret_cast<QWidget*>(obj)){
    m->AddWidget(title, widget);
  }
}


void main_area_add_pers_widget_a(MainArea* m, const QString& title,
                               const QString& name,
                               const SipHandlerBase& sh)
{
  m->AddPersistentWidget(title, name, 
                         reinterpret_cast<QWidget*>(sh.GetSipHandle()));
}

void main_area_add_pers_widget_b(MainArea* m, const QString& title,
                               const QString& name,
                               QWidget* widget)
{
  TransferOwnership(widget);
  m->AddPersistentWidget(title, name, 
                         reinterpret_cast<QWidget*>(widget));
}

void main_area_add_pers_widget_c(MainArea* m, const QString& title,
                               const QString& name,
                               const SipHandlerBase& sh, int width, int height, int x, int y)
{
  m->AddPersistentWidget(title, name,
                         reinterpret_cast<QWidget*>(sh.GetSipHandle()), width, height, x, y);
}

void main_area_add_pers_widget_d(MainArea* m, const QString& title,
                               const QString& name,
                               QWidget* widget, int width, int height, int x, int y)
{
  TransferOwnership(widget);
  m->AddPersistentWidget(title, name,
                         reinterpret_cast<QWidget*>(widget), width, height, x, y);
}

void main_area_add_pers_widget_e(MainArea* m, const QString& title,
                               const QString& name, const SipHandlerBase& sh, int window_states)
{
  Qt::WindowStates q_window_states =Qt::WindowStates(window_states);
  m->AddPersistentWidget(title, name,
                         reinterpret_cast<QWidget*>(sh.GetSipHandle()), q_window_states);
}

void main_area_add_pers_widget_f(MainArea* m, const QString& title,
                               const QString& name, QWidget* widget, int window_states)
{
  TransferOwnership(widget);
  Qt::WindowStates q_window_states =Qt::WindowStates(window_states);
  m->AddPersistentWidget(title, name,
                         reinterpret_cast<QWidget*>(widget), q_window_states);
}

void main_area_show_sub_window(MainArea* m, const SipHandlerBase& sh)
{
  m->ShowSubWindow(reinterpret_cast<QWidget*>(sh.GetSipHandle()));
}

void main_area_hide_sub_window(MainArea* m, const SipHandlerBase& sh)
{
  m->HideSubWindow(reinterpret_cast<QWidget*>(sh.GetSipHandle()));
}




void main_area_add_pers_widget_g(MainArea* m, const QString& title,
    const QString& name, object py_object, int window_states)
{
  static object sip_module=import("sip");
  static object gui_module=import("ost.gui");
  static object pyqt4_module=import("PyQt4.QtCore");

  unsigned long addr = extract<unsigned long>(sip_module.attr("unwrapinstance")(py_object));
  QWidget* widget = reinterpret_cast<QWidget*>(addr);
  if(widget){
    Qt::WindowStates q_window_states =Qt::WindowStates(window_states);
    m->AddPersistentWidget(title, name, widget, q_window_states);
  }
}







}

void export_MainArea()
{  
  class_<MainArea, boost::noncopyable>("MainArea", no_init)
    .def("AddWidget", &MainArea::AddWidget)
    .def("AddWidget", &main_area_add_widget_a)
    .def("AddWidget", &main_area_add_widget_b)
    .def("AddPersistentWidget", &main_area_add_pers_widget_a)    
    .def("AddPersistentWidget", &main_area_add_pers_widget_b)
    .def("AddPersistentWidget", &main_area_add_pers_widget_c)
    .def("AddPersistentWidget", &main_area_add_pers_widget_d)
    .def("AddPersistentWidget", &main_area_add_pers_widget_e)
    .def("AddPersistentWidget", &main_area_add_pers_widget_f)
    .def("AddPersistentWidget", &main_area_add_pers_widget_g)
    .def("width", &MainArea::width)
    .def("height", &MainArea::height)
    .def("ShowSubWindow", &MainArea::ShowSubWindow)
    .def("ShowSubWindow", &main_area_show_sub_window)        
    .def("HideSubWindow", &MainArea::HideSubWindow)
    .def("HideSubWindow", &main_area_hide_sub_window)            
    .def("EnableTabbedMode", &MainArea::EnableTabbedMode, 
         arg("flag")=true)
  ;
}

