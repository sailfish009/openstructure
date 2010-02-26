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

#include <iostream>

#include "python_shell_proxy.hh"
#include <ost/gui/python_shell/python_shell_widget.hh>


using namespace ost;
using namespace ost::gui;

void export_PyShell()
{ 
  class_<PythonShellProxy, bases<SipHandlerBase> >("PythonShell", no_init)
    .def("Show", &PythonShellProxy::Show)
    .def("Hide", &PythonShellProxy::Hide)
    .def("SetTabWidth", &PythonShellProxy::SetTabWidth)
    .def("GetTabWidth", &PythonShellProxy::GetTabWidth)
  ;
}
