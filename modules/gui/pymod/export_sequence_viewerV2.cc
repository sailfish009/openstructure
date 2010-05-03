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

#include <ost/gui/sequence/sequence_viewer.hh>

#include "sip_handler.hh"

using namespace boost::python;
using namespace ost;
using namespace ost::gui;


void export_SequenceViewerV2()
{
  class_<SequenceViewerV2, boost::noncopyable >("SequenceViewerV2",init<>())
    .def("Show", &SequenceViewerV2::show)
    .def("Hide", &SequenceViewerV2::hide)
    .def("GetQObject",&get_py_qobject<SequenceViewerV2>)
    .add_property("qobject", &get_py_qobject<SequenceViewerV2>)
  ;
}

