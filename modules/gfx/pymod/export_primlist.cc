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

#include <boost/shared_ptr.hpp>

#include <ost/gfx/prim_list.hh>
using namespace ost;
using namespace ost::gfx;

#if OST_NUMPY_SUPPORT_ENABLED
#include <numpy/arrayobject.h>
#endif

namespace {
  void add_mesh(PrimList& p, object ova, object ona, object oca, object oia)
  {
#if OST_NUMPY_SUPPORT_ENABLED
    if(!PyArray_Check(ova.ptr())) {
      throw std::runtime_error("ova is not a numpy array");
    }
    PyArrayObject* va=reinterpret_cast<PyArrayObject*>(ova.ptr());
    if(!PyArray_ISCONTIGUOUS(va)) {
      throw std::runtime_error("expected vertex array to be contiguous");
    }
    if(!PyArray_TYPE(va)==NPY_FLOAT) {
      throw std::runtime_error("expected vertex array to be of dtype=float32");
    }
    size_t v_size=PyArray_SIZE(va);
    if(v_size%3!=0) {
      throw std::runtime_error("expected vertex array size to be divisible by 3");
    }
    size_t v_count=v_size/3;
    float* vp=reinterpret_cast<float*>(PyArray_DATA(va));
    float* np=0;
    float* cp=0;
    if(ona!=object()) {
      if(!PyArray_Check(ona.ptr())) {
        throw std::runtime_error("ona is not a numpy array");
      }
      PyArrayObject* na=reinterpret_cast<PyArrayObject*>(ona.ptr());
      if(!PyArray_ISCONTIGUOUS(na)) {
        throw std::runtime_error("expected normal array to be contiguous");
      }
      if(!PyArray_TYPE(na)==NPY_FLOAT) {
        throw std::runtime_error("expected normal array to be of dtype=float32");
      }
      if(PyArray_SIZE(na)!=v_size) {
        throw std::runtime_error("expected normal array size to match vertex array size");
      }
      np=reinterpret_cast<float*>(PyArray_DATA(na));
    }
    if(oca!=object()) {
      if(!PyArray_Check(oca.ptr())) {
        throw std::runtime_error("oca is not a numpy array");
      }
      PyArrayObject* ca=reinterpret_cast<PyArrayObject*>(oca.ptr());
      if(!PyArray_ISCONTIGUOUS(ca)) {
        throw std::runtime_error("expected color array to be contiguous");
      }
      if(!PyArray_TYPE(ca)==NPY_FLOAT) {
        throw std::runtime_error("expected color array to be of dtype=float32");
      }
      if(PyArray_SIZE(ca)!=v_count*4) {
        throw std::runtime_error("expected color array size to equal vertex-count x 4");
      }
      cp=reinterpret_cast<float*>(PyArray_DATA(ca));
    }
    if(!PyArray_Check(oia.ptr())) {
      throw std::runtime_error("oia is not a numpy array");
    }
    PyArrayObject* ia=reinterpret_cast<PyArrayObject*>(oia.ptr());
    if(!PyArray_ISCONTIGUOUS(ia)) {
      throw std::runtime_error("expected vertex array to be contiguous");
    }
    if(!PyArray_TYPE(ia)==NPY_UINT) {
      throw std::runtime_error("expected vertex array to be of dtype=uint32");
    }
    size_t i_size=PyArray_SIZE(ia);
    unsigned int* ip=reinterpret_cast<unsigned int*>(PyArray_DATA(ia));

    p.AddMesh(vp,np,cp,v_count,ip,i_size/3);
#else
    throw std::runtime_error("AddMesh requires compiled-in numpy support");
#endif
  }
}

void export_primlist()
{
  import_array(); // magic handshake for numpy module

  class_<PrimList, bases<GfxObj>, boost::shared_ptr<PrimList>, boost::noncopyable>("PrimList", init<const String& >())
    .def("Clear",&PrimList::Clear)
    .def("_add_line",&PrimList::AddLine)
    .def("_add_point",&PrimList::AddPoint)
    .def("_add_sphere",&PrimList::AddSphere)
    .def("_add_cyl",&PrimList::AddCyl)
    .def("_add_text",&PrimList::AddText)
    .def("AddMesh",add_mesh)
    .def("SetColor",&PrimList::SetColor)
    .def("SetDiameter",&PrimList::SetDiameter)
    .def("SetRadius",&PrimList::SetRadius)
  ;

}
