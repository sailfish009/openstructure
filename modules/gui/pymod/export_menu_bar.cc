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

#include "menu_proxy.hh"
#include "menu_bar_proxy.hh"

using namespace boost::python;
using namespace ost;
using namespace ost::gui;


void export_MenuBar()
{
  class_<MenuBarProxy, bases<SipHandlerBase> >("MenuBar")
    .def("Show", &MenuBarProxy::Show)
    .def("Hide", &MenuBarProxy::Hide)
  ;

  class_<MenuProxy, bases<SipHandlerBase> >("Menu");
}

