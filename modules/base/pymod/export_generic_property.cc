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

using namespace boost::python;

#include <ost/generic_property.hh>

using namespace ost;

void export_GenericProperty()
{
  String (GenericPropertyContainer::* get_string1)(const String&) const = &GenericPropertyContainer::GetGenericStringProperty;
  String (GenericPropertyContainer::* get_string2)(const String&, const String&) const = &GenericPropertyContainer::GetGenericStringProperty;
  Real (GenericPropertyContainer::* get_float1)(const String&) const = &GenericPropertyContainer::GetGenericFloatProperty;
  Real (GenericPropertyContainer::* get_float2)(const String&, Real) const = &GenericPropertyContainer::GetGenericFloatProperty;
  int (GenericPropertyContainer::* get_int1)(const String&) const = &GenericPropertyContainer::GetGenericIntProperty;
  int (GenericPropertyContainer::* get_int2)(const String&, int) const = &GenericPropertyContainer::GetGenericIntProperty;
  bool (GenericPropertyContainer::* get_bool1)(const String&) const = &GenericPropertyContainer::GetGenericBoolProperty;
  bool (GenericPropertyContainer::* get_bool2)(const String&, bool) const = &GenericPropertyContainer::GetGenericBoolProperty;

  class_<GenericPropertyContainer, boost::noncopyable>("GenericPropertyContainer",no_init)
    .def("HasGenericProperty",&GenericPropertyContainer::HasGenericProperty)
    .def("SetGenericStringProperty",&GenericPropertyContainer::SetGenericStringProperty)
    .def("GetGenericStringProperty",get_string1)
    .def("GetGenericStringProperty",get_string2)
    .def("SetGenericFloatProperty",&GenericPropertyContainer::SetGenericFloatProperty)
    .def("GetGenericFloatProperty",get_float1)
    .def("GetGenericFloatProperty",get_float2)
    .def("SetGenericIntProperty",&GenericPropertyContainer::SetGenericIntProperty)
    .def("GetGenericIntProperty",get_int1)
    .def("GetGenericIntProperty",get_int2)
    .def("SetGenericBoolProperty",&GenericPropertyContainer::SetGenericBoolProperty)
    .def("GetGenericBoolProperty",get_bool1)
    .def("GetGenericBoolProperty",get_bool2)
    .def("ClearGenericProperties",&GenericPropertyContainer::ClearGenericProperties)
    .def("GetGenericPropertyStringRepresentation",&GenericPropertyContainer::GetGenericPropertyStringRepresentation)
    ;
}
