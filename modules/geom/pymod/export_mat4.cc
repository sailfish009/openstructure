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
#define BOOST_PYTHON_MAX_ARITY 20
#include <boost/python.hpp>
#include <boost/python/slice.hpp>
using namespace boost::python;

#include <ost/geom/geom.hh>
using namespace geom;



const Real Mat4_getitem(const geom::Mat4& m, tuple i) {return m(extract<int> (i[0]),extract<int> (i[1]));}
void Mat4_setitem(geom::Mat4& m,const  tuple i,const  Real val) {m(extract<int> (i[0]),extract<int> (i[1]))=val;}

Mat3 Mat4_getslice(const geom::Mat4& m, slice s) {
  tuple start=extract<tuple> (s.start());
  tuple end=extract<tuple> (s.stop());
  int start0=extract<int> (start[0]);
  int start1=extract<int> (start[1]);
  int end0=extract<int> (end[0]);
  int end1=extract<int> (end[1]);
  if(end0-start0==1 && end1-start1==1) throw GeomException("Invalid slice: Only Mat3 slices allowed");
  if(end0-start0!=2 || end1-start1!=2) throw GeomException("Invalid slice");
  return Mat3(m(start0,start1),m(start0,start1+1),m(start0,start1+2),m(start0+1,start1),m(start0+1,start1+1),m(start0+1,start1+2),m(start0+2,start1),m(start0+2,start1+1),m(start0+2,start1+2));
}
void Mat4_setslice2(geom::Mat4& m,const  slice s,const  Mat2& m2)
{
  tuple start=extract<tuple> (s.start());
  tuple end=extract<tuple> (s.stop());
  int start0=extract<int> (start[0]);
  int start1=extract<int> (start[1]);
  int end0=extract<int> (end[0]);
  int end1=extract<int> (end[1]);
  if(end0-start0!=1 || end1-start1!=1) throw GeomException("Invalid slice");
  m(start0,start1)=m2(0,0);
  m(start0,start1+1)=m2(0,1);
  m(start0+1,start1)=m2(1,0);
  m(start0+1,start1+1)=m2(1,1);
}
void Mat4_setslice3(geom::Mat4& m,const  slice s,const  Mat3& m2)
{
  tuple start=extract<tuple> (s.start());
  tuple end=extract<tuple> (s.stop());
  int start0=extract<int> (start[0]);
  int start1=extract<int> (start[1]);
  int end0=extract<int> (end[0]);
  int end1=extract<int> (end[1]);
  if(end0-start0!=2 || end1-start1!=2) throw GeomException("Invalid slice");
  m(start0,start1)=m2(0,0);
  m(start0,start1+1)=m2(0,1);
  m(start0,start1+2)=m2(0,2);
  m(start0+1,start1)=m2(1,0);
  m(start0+1,start1+1)=m2(1,1);
  m(start0+1,start1+2)=m2(1,2);
  m(start0+2,start1)=m2(2,0);
  m(start0+2,start1+1)=m2(2,1);
  m(start0+2,start1+2)=m2(2,2);
}

String mat4_repr(const geom::Mat4& m) {
  std::stringstream ss;

  ss << "geom.Mat4("
     << m(0,0) << ", " << m(0,1) << ", " << m(0,2) << ", " << m(0,3) << ", "
     << m(1,0) << ", " << m(1,1) << ", " << m(1,2) << ", " << m(1,3) << ", "
     << m(2,0) << ", " << m(2,1) << ", " << m(2,2) << ", " << m(2,3) << ", "
     << m(3,0) << ", " << m(3,1) << ", " << m(3,2) << ", " << m(3,3) << ")";
  return ss.str();
}


void export_Mat4()
{

  class_<Mat4>("Mat4",init<>())
    .def(init<Real,Real,Real,Real,Real,Real,Real,Real,Real,Real,Real,Real,Real,Real,Real,Real>())
    .def(init<const Mat2&>())
    .def(init<const Mat3&>())
    .def(self += self)
    .def(self -= self)
    .def(self + self)
    .def(self - self)
    .def(self *= Real())
    .def(self /= Real())
    .def(self * Real())
    .def(self * self)
    .def(self *= self)
    .def(self * Vec4())
    .def(self / Real())
    .def(self_ns::str(self))
    .def("__repr__", mat4_repr)
    .def("__getitem__",Mat4_getitem)
    .def("__getitem__",Mat4_getslice)
    .def("__setitem__",Mat4_setitem)
    .def("__getitem__",Mat4_setslice2)
    .def("__getitem__",Mat4_setslice3)
    .def("ExtractRotation",&Mat4::ExtractRotation)
    .def("PasteRotation",&Mat4::PasteRotation)
    .def("ExtractTranslation",&Mat4::ExtractTranslation)
    .def("PasteTranslation",&Mat4::PasteTranslation)
  ;
}
