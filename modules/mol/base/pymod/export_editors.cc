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
#include <boost/python/suite/indexing/vector_indexing_suite.hpp>

using namespace boost::python;

#include <ost/mol/mol.hh>

using namespace ost;
using namespace ost::mol;

#if OST_NUMPY_SUPPORT_ENABLED
#include <numpy/arrayobject.h>
#endif

namespace {

BondHandle (EditorBase::*connect_a)(const AtomHandle&, 
                                    const AtomHandle&)=&EditorBase::Connect;
BondHandle (EditorBase::*connect_c)(const AtomHandle&, 
                                    const AtomHandle&,
                                    unsigned char)=&EditorBase::Connect;
BondHandle (EditorBase::*connect_b)(const AtomHandle&, const AtomHandle&, 
                                    Real, Real, Real)=&EditorBase::Connect;
BondHandle (EditorBase::*connect_d)(const AtomHandle&, const AtomHandle&, 
                                    Real, Real, Real,
                                    unsigned char)=&EditorBase::Connect;
                                    
ResidueHandle (EditorBase::*append_a)(ChainHandle ch, 
                                      const ResidueKey&)=&EditorBase::AppendResidue;
ResidueHandle (EditorBase::*append_b)(ChainHandle ch, const ResidueKey&, 
                                      const ResNum&)=&EditorBase::AppendResidue;

void (ICSEditor::*set_torsion_a)(TorsionHandle, Real)=&ICSEditor::SetTorsionAngle;
void (ICSEditor::*set_torsion_b)(const AtomHandle&, const AtomHandle&,
                                 const AtomHandle&, const AtomHandle&,
                                 Real)=&ICSEditor::SetTorsionAngle;

void (ICSEditor::*rotate_torsion_a)(TorsionHandle, Real)=&ICSEditor::RotateTorsionAngle;
void (ICSEditor::*rotate_torsion_b)(const AtomHandle&, const AtomHandle&,
                                    const AtomHandle&, const AtomHandle&,
                                    Real)=&ICSEditor::RotateTorsionAngle;

void (XCSEditor::*set_pos1)(const AtomHandle&, const geom::Vec3&) = &XCSEditor::SetAtomPos;
void (XCSEditor::*set_t_pos1)(const AtomHandle&, const geom::Vec3&) = &XCSEditor::SetAtomTransformedPos;
void (XCSEditor::*set_o_pos1)(const AtomHandle&, const geom::Vec3&) = &XCSEditor::SetAtomOriginalPos;

#if OST_NUMPY_SUPPORT_ENABLED
template<typename T, bool O>
void set_pos2_nc_t(XCSEditor& e, const AtomHandleList& alist, PyArrayObject* na)
{
  size_t count=0;
  for(AtomHandleList::const_iterator ait=alist.begin();ait!=alist.end();++ait,++count) {
    if(O) {
      e.SetAtomOriginalPos(*ait,geom::Vec3(static_cast<Real>(*reinterpret_cast<T*>(PyArray_GETPTR2(na,count,0))),
					   static_cast<Real>(*reinterpret_cast<T*>(PyArray_GETPTR2(na,count,1))),
					   static_cast<Real>(*reinterpret_cast<T*>(PyArray_GETPTR2(na,count,2)))));
    } else {
      e.SetAtomTransformedPos(*ait,geom::Vec3(static_cast<Real>(*reinterpret_cast<T*>(PyArray_GETPTR2(na,count,0))),
					      static_cast<Real>(*reinterpret_cast<T*>(PyArray_GETPTR2(na,count,1))),
					      static_cast<Real>(*reinterpret_cast<T*>(PyArray_GETPTR2(na,count,2)))));
    }
  }
}

template<bool O>
void set_pos2_t(XCSEditor& e, const AtomHandleList& alist, object pyobj)
{
  size_t acount = alist.size();
 
  if(!PyArray_Check(pyobj.ptr())) {
    throw std::runtime_error("expected a numpy array");
    return;
  }
  PyArrayObject* na=reinterpret_cast<PyArrayObject*>(pyobj.ptr());

  if(PyArray_NDIM(na)!=2 || PyArray_DIM(na,0)!=acount || PyArray_DIM(na,1)!=3) {
    throw std::runtime_error("excpected a numpy array of shape (NAtoms, 3)");
    return;
  }

  if(PyArray_ISCONTIGUOUS(na)) {
    if(PyArray_TYPE(na)==NPY_FLOAT) {
      if(O) {
	e.SetAtomOriginalPos(alist,reinterpret_cast<float*>(PyArray_DATA(na)));
      } else {
	e.SetAtomTransformedPos(alist,reinterpret_cast<float*>(PyArray_DATA(na)));
      }
    } else if(PyArray_TYPE(na)==NPY_DOUBLE) {
      if(O) {
	e.SetAtomOriginalPos(alist,reinterpret_cast<double*>(PyArray_DATA(na)));
      } else {
	e.SetAtomTransformedPos(alist,reinterpret_cast<double*>(PyArray_DATA(na)));
      }
    } else {
      throw std::runtime_error("excpected a numpy array of type float or double");
      return;
    }
  } else {
    // non-contiguous
#if 0
    throw std::runtime_error("excpected contiguous numpy array");
#else
    if(PyArray_TYPE(na)==NPY_FLOAT) {
      set_pos2_nc_t<float,O>(e,alist,na);
    } else if(PyArray_TYPE(na)==NPY_DOUBLE) {
      set_pos2_nc_t<double,O>(e,alist,na);
    } else {
      throw std::runtime_error("excpected a numpy array of type float or double");
      return;
    }
#endif
  }
}
#endif

void set_t_pos2(XCSEditor& e, const AtomHandleList& alist, object pyobj)
{
#if OST_NUMPY_SUPPORT_ENABLED
  set_pos2_t<false>(e,alist,pyobj);
#else
  throw std::runtime_error("SetAtomTransformedPos(alist,ndarray) disabled, since numpy support is not compiled in");
#endif
}

void set_o_pos2(XCSEditor& e, const AtomHandleList& alist, object pyobj)
{
#if OST_NUMPY_SUPPORT_ENABLED
  set_pos2_t<true>(e,alist,pyobj);
#else
  throw std::runtime_error("SetAtomOriginalPos(alist,ndarray) disabled, since numpy support is not compiled in");
#endif
}

void set_pos2(XCSEditor& e, const AtomHandleList& alist, object pyobj)
{
  set_t_pos2(e,alist,pyobj);
}

}

void export_Editors()
{
#if OST_NUMPY_SUPPORT_ENABLED
  import_array();
#endif

  class_<EditorBase>("EditorBase", no_init)
    .def("InsertChain", &EditorBase::InsertChain)
    .def("InsertAtom", &EditorBase::InsertAtom,
         (arg("residue"), arg("name"), arg("pos"), arg("element")="", 
          arg("occupancy")=1.0, arg("b_factor")=0.0, arg("is_hetatm")=false))
    .def("InsertAltAtom", &EditorBase::InsertAltAtom)
    .def("DeleteResidue", &EditorBase::DeleteResidue)
    .def("DeleteChain", &EditorBase::DeleteChain)
    .def("DeleteAtom", &EditorBase::DeleteAtom)
    .def("InsertResidueBefore", &EditorBase::InsertResidueBefore)
    .def("InsertResidueAfter", &EditorBase::InsertResidueAfter)    
    .def("AppendResidue", append_a)
    .def("AppendResidue", append_b)    
    .def("Connect", connect_a)
    .def("Connect", connect_b)    
    .def("Connect", connect_c)
    .def("Connect", connect_d)    
    .def("RenameChain", &EditorBase::RenameChain)
    .def("AddTorsion", &EditorBase::AddTorsion)
    .def("ReorderResidues",&EditorBase::ReorderResidues)
    .def("ReorderAllResidues",&EditorBase::ReorderAllResidues)
  ;
  
  class_<XCSEditor, bases<EditorBase> >("XCSEditor", no_init)
    .def("SetAtomPos", set_pos1)
    .def("SetAtomPos", set_pos2)
    .def("SetAtomTransformedPos", set_t_pos1)
    .def("SetAtomTransformedPos", set_t_pos2)
    .def("SetAtomOriginalPos", set_o_pos1)
    .def("SetAtomOriginalPos", set_o_pos2)
    .def("ApplyTransform", &XCSEditor::ApplyTransform)
    .def("SetTransform", &XCSEditor::SetTransform)
    .def("UpdateICS", &XCSEditor::UpdateICS)
    .def("__exit__", &XCSEditor::UpdateICS)    
  ;
  
  class_<ICSEditor, bases<EditorBase> >("ICSEditor", no_init)
    .def("SetAngle", &ICSEditor::SetAngle)
    .def("SetBondLength", &ICSEditor::SetBondLength)
    .def("SetTorsionAngle", set_torsion_a)
    .def("SetTorsionAngle", set_torsion_b)    
    .def("RotateTorsionAngle", rotate_torsion_a)
    .def("RotateTorsionAngle", rotate_torsion_b)
    .def("UpdateXCS", &ICSEditor::UpdateXCS)
    .def("__exit__", &ICSEditor::UpdateXCS)
  ;  
}
