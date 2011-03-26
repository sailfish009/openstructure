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
#include <boost/python.hpp>
using namespace boost::python;

#include <ost/geom/geom.hh>



const Real Mat2_getitem(const geom::Mat2& m, tuple i) {return m(extract<int> (i[0]),extract<int> (i[1]));}
void Mat2_setitem(geom::Mat2& m,const  tuple i,const  Real val) {m(extract<int> (i[0]),extract<int> (i[1]))=val;}

String mat2_repr(const geom::Mat2& m) {
  std::stringstream ss;

  ss << "geom.Mat2(" << m(0,0) << ", " << m(0,1) << ", "
     << m(1,0) << ", " << m(1,1) << ")";
  return ss.str();
}
void export_Mat2()
{
  using namespace geom;

  class_<Mat2>("Mat2",init<>())
    .def(init<Real,Real,Real,Real>())
    .def(self += self)
    .def(self -= self)
    .def(self + self)
    .def("__repr__", mat2_repr)
    .def(self - self)
    .def(self *= Real())
    .def(self /= Real())
    .def(self * Real())
    .def(self *= self)
    .def(self * self)
    .def(self * Vec2())
    .def(self / Real())
    .def(self_ns::str(self))
    .def("__getitem__",Mat2_getitem)
    .def("__setitem__",Mat2_setitem)
  ;
}
