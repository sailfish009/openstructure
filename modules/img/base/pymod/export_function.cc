//------------------------------------------------------------------------------
// This file is part of the OpenStructure project <www.openstructure.org>
//
// Copyright (C) 2008-2015 by the OpenStructure authors
// Copyright (C) 2003-2010 by the IPLT authors
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

/*
  Author: Ansgar Philippsen
*/

#include <boost/python.hpp>
#include <ost/img/function.hh>
using namespace boost::python;

namespace ost { namespace img {

class WrapRealFunction: public RealFunction
{
public:
  WrapRealFunction(PyObject* self, const RealFunction& f):
    RealFunction(f),
    self_(self) {}
    
  WrapRealFunction(PyObject* self)
    : RealFunction(SPATIAL),
      self_(self)
    { }
  WrapRealFunction(PyObject* self, DataDomain domain)
    : RealFunction(domain),
      self_(self)
    { }  
  virtual Real Func(const Vec3& v) const {
    return call_method<Real>(self_,"Func",v);
  }

private:
  PyObject* self_;
};

class WrapComplexFunction: public ComplexFunction
{
public:
  WrapComplexFunction(PyObject* self, const ComplexFunction& f):
    ComplexFunction(f),
    self_(self) {}
    
  WrapComplexFunction(PyObject* self)
    : ComplexFunction(SPATIAL),
      self_(self)
    { }
    
  WrapComplexFunction(PyObject* self, DataDomain domain)
    : ComplexFunction(domain),
      self_(self)
    { }    
  virtual Complex Func(const Vec3& v) const {
    return call_method<Complex>(self_,"Func",v);
  }

private:
  PyObject* self_;
};

}} // namespace

void export_Function()
{
  using namespace ost::img;
  using namespace ost;
  class_<Function, bases<Data>, boost::noncopyable>("Function", no_init)
    .def("SetExtent",&Function::SetExtent)
    ;

  class_<RealFunction, WrapRealFunction, bases<Function>, boost::noncopyable>("RealFunction")
    .def(init<DataDomain>())
    .def("Func",&RealFunction::Func)
    ;

  class_<ComplexFunction, WrapComplexFunction, bases<Function>, boost::noncopyable>("ComplexFunction")
    .def(init<DataDomain>())  
    .def("Func",&ComplexFunction::Func)
    ;

}
