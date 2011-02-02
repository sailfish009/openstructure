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

#include <ost/mol/entity_view.hh>
#include <ost/mol/query.hh>
#include <ost/mol/mol.hh>
#include "bounds.hh"

using namespace ost;
using namespace ost::mol;

#include <ost/export_helper/generic_property_def.hh>

#if OST_NUMPY_SUPPORT_ENABLED
#include <numpy/arrayobject.h>
#endif

namespace {
EntityHandle create1() {  return CreateEntity(); }

typedef EntityView (EntityHandle::*StringMethod)(const String&, uint) const;
typedef BondHandle (EntityHandle::*Connect1)(const AtomHandle&,
                                             const AtomHandle&);
typedef BondHandle (EntityHandle::*Connect2)(const AtomHandle&,
                                             const AtomHandle&, Real,
                                             Real, Real);

typedef EntityView (EntityHandle::*QueryMethod)(const Query&, uint) const;
QueryMethod select_query=&EntityHandle::Select;
StringMethod select_string=&EntityHandle::Select;
//Connect1 conn1=&EntityHandle::Connect;
//Connect2 conn2=&EntityHandle::Connect;


Real (EntityHandle::*get_angle1)(const AtomHandle&, const AtomHandle&, const AtomHandle&) const = &EntityHandle::GetAngle;
Real (EntityHandle::*get_angle2)(const AtomView&, const AtomView&, const AtomView&) const = &EntityHandle::GetAngle;
                                      
XCSEditor depr_request_xcs_editor(EntityHandle e, EditMode m)
{
  WARN_DEPRECATED("EntityHandle::RequestXCSEditor is deprecated. Use "
                  "EntityHandle::EditXCS instead");
  return e.EditXCS(m);
}

ICSEditor depr_request_ics_editor(EntityHandle e, EditMode m)
{
  WARN_DEPRECATED("EntityHandle::RequestICSEditor is deprecated. Use "
                  "EntityHandle::EditICS instead");
  return e.EditICS(m);
}


PyObject* get_pos(EntityHandle& entity)
{
#if OST_NUMPY_SUPPORT_ENABLED
  npy_intp dims[]={entity.GetAtomCount(),3};
  PyObject* na = PyArray_SimpleNew(2,dims,NPY_DOUBLE);
  npy_double* nad = reinterpret_cast<npy_double*>(PyArray_DATA(na));
  for(AtomHandleIter it=entity.AtomsBegin();it!=entity.AtomsEnd();++it,nad+=3) {
    geom::Vec3 pos=(*it).GetPos();
    nad[0]=static_cast<npy_double>(pos[0]);
    nad[1]=static_cast<npy_double>(pos[1]);
    nad[2]=static_cast<npy_double>(pos[2]);
  }
  return na;
#else
  throw std::runtime_error("GetPositions disabled, since numpy support is not compiled in");
  return 0;
#endif
}


#if OST_NUMPY_SUPPORT_ENABLED
template <typename T>
void set_pos_t(PyArrayObject* na, EntityHandle& entity)
{
  XCSEditor ed=entity.EditXCS(BUFFERED_EDIT);

  if(PyArray_ISCONTIGUOUS(na)) {
    T* data = reinterpret_cast<T*>(PyArray_DATA(na));
    size_t count=0;
    for(AtomHandleIter it=entity.AtomsBegin();it!=entity.AtomsEnd();++it,++count) {
      ed.SetAtomPos(*it,geom::Vec3(static_cast<Real>(data[count*3+0]),
                                   static_cast<Real>(data[count*3+1]),
                                   static_cast<Real>(data[count*3+2])));
    }
  } else {
    size_t count=0;
    for(AtomHandleIter it=entity.AtomsBegin();it!=entity.AtomsEnd();++it,++count) {
      ed.SetAtomPos(*it,geom::Vec3(static_cast<Real>(*reinterpret_cast<T*>(PyArray_GETPTR2(na,count,0))),
                                   static_cast<Real>(*reinterpret_cast<T*>(PyArray_GETPTR2(na,count,1))),
                                   static_cast<Real>(*reinterpret_cast<T*>(PyArray_GETPTR2(na,count,2)))));
    }
  }
}
#endif

void set_pos(EntityHandle& entity, object& pyobj)
{
#if OST_NUMPY_SUPPORT_ENABLED
  size_t acount = entity.GetAtomCount();
  
  if(!PyArray_Check(pyobj.ptr())) {
    throw std::runtime_error(std::string("expected a numpy array"));
    return;
  }
  PyArrayObject* na=reinterpret_cast<PyArrayObject*>(pyobj.ptr());

  if(PyArray_NDIM(na)!=2 || PyArray_DIM(na,0)!=acount || PyArray_DIM(na,1)!=3) {
    throw std::runtime_error("excpected a numpy array of shape (NAtoms, 3)");
    return;
  }

  switch(PyArray_TYPE(na)) {
  case NPY_FLOAT:
    set_pos_t<float>(na,entity); break;
  case NPY_DOUBLE:
    set_pos_t<double>(na,entity); break;
  case NPY_INT:
    set_pos_t<int>(na,entity); break;
  case NPY_LONG:
    set_pos_t<long>(na,entity); break;
  case NPY_SHORT:
    set_pos_t<short>(na,entity); break;
  default:
    throw std::runtime_error("excpected a numpy array of type float, double, int, long or short");
    return;
  };

#else
  throw std::runtime_error("SetPositions disabled, since numpy support is not compiled in");
#endif
}

} // ns

void export_Entity()
{
#if OST_NUMPY_SUPPORT_ENABLED
  import_array();
#endif

  class_<EntityBase> ent_base("EntityBase", no_init);
  ent_base
    .def(self_ns::str(self))
    .def("GetName", &EntityBase::GetName,
        return_value_policy<copy_const_reference>())
    .def("SetName", &EntityBase::SetName)
    .def("IsValid", &EntityBase::IsValid)
    .add_property("valid", &EntityBase::IsValid)
  ;
  generic_prop_def<EntityBase>(ent_base);
  
  class_<EntityHandle, bases<EntityBase> >("EntityHandle", init<>())
    .def("Select",select_query, arg("flags")=0)
    .def("Select",select_string, arg("flags")=0)
    .def("FindChain", &EntityHandle::FindChain)
    .def("FindResidue", &EntityHandle::FindResidue)
    .def("FindAtom", &EntityHandle::FindAtom)
    .def("GetChainList", &EntityHandle::GetChainList)
    .def("CreateFullView",&EntityHandle::CreateFullView)
    .def("CreateEmptyView",&EntityHandle::CreateEmptyView)
    .def("GetMass", &EntityHandle::GetMass)
    .def("GetCenterOfMass", &EntityHandle::GetCenterOfMass)
    .def("GetCenterOfAtoms", &EntityHandle::GetCenterOfAtoms)
    .def("GetGeometricCenter", geom_center<EntityHandle>)
    .add_property("geometric_center", geom_center<EntityHandle>)

    .add_property("geometric_end", geom_end<EntityHandle>)
    .add_property("geometric_start", geom_start<EntityHandle>)
    .def("GetGeometricStart", geom_start<EntityHandle>)
    .def("GetGeometricEnd", geom_end<EntityHandle>)
    .def("GetBoundarySize", geom_size<EntityHandle>)
    .def("GetResidueCount", &EntityHandle::GetResidueCount)
    .def("GetAtomCount", &EntityHandle::GetAtomCount)
    .def("GetBondCount", &EntityHandle::GetBondCount)
    .def("GetChainCount", &EntityHandle::GetChainCount)
    .def("GetHandle", &EntityHandle::GetHandle)
    .add_property("handle", &EntityHandle::GetHandle)
    .add_property("chain_count", &EntityHandle::GetChainCount)
    .add_property("residue_count", &EntityHandle::GetResidueCount)
    .add_property("atom_count", &EntityHandle::GetAtomCount)
    .add_property("mass", &EntityHandle::GetMass)
    .add_property("center_of_mass", &EntityHandle::GetCenterOfMass)
    .add_property("center_of_atoms", &EntityHandle::GetCenterOfAtoms)
    .def("FindWithin", &EntityHandle::FindWithin)
    .def("GetAngle", get_angle1)
    .def("GetAngle", get_angle2)
    .def("FindTorsion", &EntityHandle::FindTorsion)
    .def("Copy", &EntityHandle::Copy)
    .def("GetBondList", &EntityHandle::GetBondList)
    .def("GetAtomList", &EntityHandle::GetAtomList)
    .def("GetResidueList", &EntityHandle::GetResidueList)
    .add_property("residues", &EntityHandle::GetResidueList)
    .add_property("atoms", &EntityHandle::GetAtomList)
    .add_property("chains", &EntityHandle::GetChainList)
    .add_property("bonds", &EntityHandle::GetBondList)
    .def("GetBounds", &EntityHandle::GetBounds)
    .add_property("bounds", &EntityHandle::GetBounds)
    .def("GetTransformationMatrix", &EntityHandle::GetTransformationMatrix,
         return_value_policy<copy_const_reference>())
    .add_property("transform", 
                   make_function(&EntityHandle::GetTransformationMatrix, 
                                 return_value_policy<copy_const_reference>()))    

    .def("EditXCS", &EntityHandle::EditXCS, arg("mode")=UNBUFFERED_EDIT)
    .def("EditICS", &EntityHandle::EditICS, arg("mode")=UNBUFFERED_EDIT)
    .def("RequestXCSEditor", &depr_request_xcs_editor, arg("mode")=UNBUFFERED_EDIT)
    .def("RequestICSEditor", &depr_request_ics_editor, arg("mode")=UNBUFFERED_EDIT)  
    .def("IsTransformationIdentity",&EntityHandle::IsTransformationIdentity)
    .def(self==self)
    .def(self!=self)
#if OST_NUMPY_SUPPORT_ENABLED
    .def("SetPositions",set_pos)
    .def("GetPositions",get_pos)
    .add_property("positions",get_pos,set_pos)
#endif
  ;

  def("CreateEntity",create1);

  class_<EntityHandleList>("EntityHandleList", init<>())
    .def(vector_indexing_suite<EntityHandleList>())
  ;
}
