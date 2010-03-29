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
#include <iostream>
#include "spnav_input_proxy.hh"

using namespace boost::python;
using namespace ost;
using namespace ost::gui;

namespace {

object spnav_get_instance()
{
  static object sip_module=import("sip");
  static object pyqt4_module=import("PyQt4.QtCore");
  SpnavInput* spnav = SpnavInput::Instance();
  unsigned long addr = reinterpret_cast<unsigned long>(spnav);
  object obj(addr);
  object qthread = pyqt4_module.attr("QThread");
  object thread = sip_module.attr("wrapinstance")(addr, qthread);

  return thread;
}

}

void export_Input()
{
  class_<SpnavInputProxy, bases<SipHandlerBase> >("SpnavInput",no_init)
    .def("GetQThread", &spnav_get_instance).staticmethod("GetQThread")
    .def("Instance", &SpnavInputProxy::Instance,
     return_value_policy<reference_existing_object>()).staticmethod("Instance")
  ;
}

