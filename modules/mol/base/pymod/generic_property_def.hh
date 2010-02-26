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

#ifndef GENERIC_PROPERTY_DEF_HH
#define GENERIC_PROPERTY_DEF_HH

/*
  Author: Marco Biasini
 */
 
template <typename C>
void generic_prop_def(class_<C>& bp_class)
{
  bool (C::*get_bool1)(const String&, bool) const=&C::GetGenericBoolProperty;
  bool (C::*get_bool2)(const String&)  const=&C::GetGenericBoolProperty;
    
  int (C::*get_int1)(const String&, int) const=&C::GetGenericIntProperty;
  int (C::*get_int2)(const String&) const=&C::GetGenericIntProperty;
    
  Real (C::*get_float1)(const String&, Real) const=&C::GetGenericFloatProperty;
  Real (C::*get_float2)(const String&) const=&C::GetGenericFloatProperty;
    
  String (C::*get_str1)(const String&, const String&) const=&C::GetGenericStringProperty;
  String (C::*get_str2)(const String&) const=&C::GetGenericStringProperty;  
  bp_class
    .def("HasGenericProperty", &C::HasGenericProperty)
    .def("SetGenericBoolProperty",
         &C::SetGenericBoolProperty)
    .def("ClearGenericProperties",  
         &C::ClearGenericProperties)
    .def("GetGenericPropertyStringRepresentation",
         &C::GetGenericPropertyStringRepresentation)
    .def("SetGenericFloatProperty",
        &C::SetGenericFloatProperty)
    .def("SetGenericIntProperty",
        &C::SetGenericIntProperty)
    .def("SetGenericStringProperty",
        &C::SetGenericStringProperty)          
    .def("GetGenericBoolProperty", get_bool1)
    .def("GetGenericBoolProperty", get_bool2)        
    .def("GetGenericFloatProperty", get_float1)
    .def("GetGenericFloatProperty", get_float2)    
    .def("GetGenericIntProperty", get_int1)
    .def("GetGenericIntProperty", get_int2)    
    .def("GetGenericStringProperty", get_str1)
    .def("GetGenericStringProperty", get_str2)    
  ;
}

#endif