//------------------------------------------------------------------------------
// This file is part of the OpenStructure project <www.openstructure.org>
//
// Copyright (C) 2008-2011 by the OpenStructure authors
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
#include <boost/python.hpp>
#include <cassert>
#include <ost/log.hh>
/*
  Author: Marco Biasini
 */
 
template <typename C>
String depr_get_string_a(C& c, const String& k, const String& v)
{
  WARN_DEPRECATED("GetGenericStringProperty is deprecated. Use GetStringProp");
  return c.GetStringProp(k, v);
}

template <typename C>
String depr_get_string_b(C& c, const String& k)
{
  WARN_DEPRECATED("GetGenericStringProperty is deprecated. Use GetStringProp");
  return c.GetStringProp(k);
}

template <typename C>
void depr_set_string(C& c, const String& k, const String& v)
{
  WARN_DEPRECATED("SetGenericStringProperty is deprecated. Use SetStringProp");
  return c.SetStringProp(k, v);
}

template <typename C>
int depr_get_int_a(C& c, const String& k, const int& v)
{
  WARN_DEPRECATED("GetGenericIntProperty is deprecated. Use GetIntProp");
  return c.GetIntProp(k, v);
}

template <typename C>
int depr_get_int_b(C& c, const String& k)
{
  WARN_DEPRECATED("GetGenericIntProperty is deprecated. Use GetIntProp");
  return c.GetIntProp(k);
}

template <typename C>
void depr_set_int(C& c, const String& k, const int& v)
{
  WARN_DEPRECATED("SetGenericIntProperty is deprecated. Use SetIntProp");
  return c.SetIntProp(k, v);
}

template <typename C>
bool depr_get_bool_a(C& c, const String& k, const bool& v)
{
  WARN_DEPRECATED("GetGenericBoolProperty is deprecated. Use GetBoolProp");
  return c.GetBoolProp(k, v);
}

template <typename C>
bool depr_get_bool_b(C& c, const String& k)
{
  WARN_DEPRECATED("GetGenericBoolProperty is deprecated. Use GetBoolProp");
  return c.GetBoolProp(k);
}

template <typename C>
void depr_set_bool(C& c, const String& k, const bool& v)
{
  WARN_DEPRECATED("SetGenericBoolProperty is deprecated. Use SetBoolProp");
  return c.SetBoolProp(k, v);
}

template <typename C>
Real depr_get_float_a(C& c, const String& k, const float& v)
{
  WARN_DEPRECATED("GetGenericFloatProperty is deprecated. Use GetFloatProp");
  return c.GetFloatProp(k, v);
}

template <typename C>
Real depr_get_float_b(C& c, const String& k)
{
  WARN_DEPRECATED("GetGenericFloatProperty is deprecated. Use GetFloatProp");
  return c.GetFloatProp(k);
}

template <typename C>
void depr_set_float(C& c, const String& k, const Real& v)
{
  WARN_DEPRECATED("SetGenericFloatProperty is deprecated. Use SetFloatProp");
  return c.SetFloatProp(k, v);
}

template <typename C>
void depr_clear_props(C& c)
{
  WARN_DEPRECATED("ClearGenericProperties is deprecated. Use ClearProps");
  c.ClearProps();  
}

template <typename C>
bool depr_has_prop(C& c, const String& k)
{
  WARN_DEPRECATED("HasGenericProperty is deprecated. Use HasProp");
  return c.HasProp(k);  
}

template <typename C>
String depr_prop_as_string(C& c, const String& k)
{
  WARN_DEPRECATED("GetGenericPropertyStringRepresentation is deprecated. Use GetPropAsString");
  return c.GetPropAsString(k);  
}

template <typename C>
boost::python::object get_attr(C& c, const String& k) {
  std::map<String,ost::GenericPropValue>::const_iterator 
      i = c.GetPropMap().find(k);
  boost::python::object obj;
  if (i != c.GetPropMap().end()) {
    switch (i->second.which()) {
      case 0:
        return boost::python::object(boost::get<String>(i->second));
      case 1:
        return boost::python::object(boost::get<Real>(i->second));
      case 2:
        return boost::python::object(boost::get<int>(i->second));
      case 3:
        return boost::python::object(boost::get<bool>(i->second));
      default:
        assert(0 && "unhandled type");
    }
  }
  std::string msg = "no such attribute '" + k + "'";
  PyErr_SetString(PyExc_AttributeError, msg.c_str());
  boost::python::throw_error_already_set();
  // throw_error_already_set is not marked noreturn in some versions of
  // boost. That's why we need a return statement at the end, even if we
  // never get here!
  return obj;
}

template <typename C>
void set_attr(boost::python::object self,
              const String& k, boost::python::object value)
{
  C* c = boost::python::extract<C*>(self);
  if (PyObject_HasAttrString(self.ptr(), k.c_str()) && !c->HasProp(k)) {
    boost::python::object name(k);
    PyObject_GenericSetAttr(self.ptr(), name.ptr(), value.ptr());
    return;
  }
  boost::python::extract<Real> f(value);
  if (f.check()) {
    c->SetFloatProp(k, f);
    return;
  }
  boost::python::extract<int> i(value);
  if (i.check()) {
    c->SetIntProp(k, i);
    return;
  }
  boost::python::extract<bool> b(value);
  if (b.check()) {
    c->SetBoolProp(k, b);
    return;
  }
  boost::python::extract<String> s(value);
  if (s.check()) {
    c->SetStringProp(k, s);
    return;
  }
}



template <typename C, typename O>
void const_generic_prop_def(O& bp_class)
{
  bool (C::*get_bool1)(const String&, bool) const=&C::GetBoolProp;
  bool (C::*get_bool2)(const String&)  const=&C::GetBoolProp;
    
  int (C::*get_int1)(const String&, int) const=&C::GetIntProp;
  int (C::*get_int2)(const String&) const=&C::GetIntProp;
    
  Real (C::*get_float1)(const String&, Real) const=&C::GetFloatProp;
  Real (C::*get_float2)(const String&) const=&C::GetFloatProp;
    
  String (C::*get_str1)(const String&, const String&) const=&C::GetStringProp;
  String (C::*get_str2)(const String&) const=&C::GetStringProp;  
  bp_class
    .def("HasProp", &C::HasProp)
    .def("GetPropAsString",
         &C::GetPropAsString)          
    .def("GetBoolProp", get_bool1)
    .def("GetBoolProp", get_bool2)        
    .def("GetFloatProp", get_float1)
    .def("GetFloatProp", get_float2)    
    .def("GetIntProp", get_int1)
    .def("GetIntProp", get_int2)    
    .def("GetStringProp", get_str1)
    .def("GetStringProp", get_str2)    
    .def("GetPropList",&C::GetPropList)
    .def("GetGenericBoolProperty", &depr_get_bool_a<C>)
    .def("GetGenericBoolProperty", &depr_get_bool_b<C>)
    .def("GetGenericFloatProperty", &depr_get_float_a<C>)
    .def("GetGenericFloatProperty", &depr_get_float_b<C>)
    .def("GetGenericIntProperty", &depr_get_int_a<C>)
    .def("GetGenericIntProperty", &depr_get_int_b<C>)
    .def("GetGenericStringProperty", &depr_get_string_a<C>)
    .def("GetGenericStringProperty", &depr_get_string_b<C>)
    .def("HasGenericProperty", &depr_has_prop<C>)
    .def("GetGenericPropertyStringRepresentation", &depr_prop_as_string<C>)
    .def("__getattr__", &get_attr<C>)
  ;
}

template <typename C, typename O>
void generic_prop_def(O& bp_class)
{
  const_generic_prop_def<C, O>(bp_class);
  bp_class
    .def("SetBoolProp",&C::SetBoolProp)
    .def("ClearProps",  &C::ClearProps)
    .def("GetPropAsString", &C::GetPropAsString)
    .def("SetFloatProp", &C::SetFloatProp)
    .def("SetIntProp", &C::SetIntProp)
    .def("SetStringProp", &C::SetStringProp)
    .def("GetPropList",&C::GetPropList)
    .def("ClearGenericProperties", &depr_clear_props<C>)
    .def("SetGenericIntProperty", &depr_set_int<C>)
    .def("SetGenericFloatProperty", &depr_set_float<C>)
    .def("SetGenericBoolProperty", &depr_set_bool<C>)
    .def("SetGenericStringProperty", &depr_set_string<C>)
    .def("RemoveProp", &C::RemoveProp)
    .def("__setattr__", &set_attr<C>)
  ;
}

#endif
