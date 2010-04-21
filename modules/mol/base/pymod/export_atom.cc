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
#include <boost/python/suite/indexing/vector_indexing_suite.hpp>

using namespace boost::python;

#include <ost/mol/mol.hh>

using namespace ost;
using namespace ost::mol;

#include "generic_property_def.hh"
void export_Atom()
{
  class_<AtomBase> atom_base("AtomBase", no_init);
  atom_base
    .def("GetPos", &AtomBase::GetPos,
         return_value_policy<copy_const_reference>())
    .def("GetOriginalPos", &AtomBase::GetOriginalPos,
         return_value_policy<copy_const_reference>())
    .def("GetName", &AtomBase::GetName,
         return_value_policy<copy_const_reference>())
    .def("GetQualifiedName", &AtomBase::GetQualifiedName)
    .add_property("qualified_name", &AtomBase::GetQualifiedName)
    .def("IsValid", &AtomBase::IsValid)
    .def(self_ns::str(self))
    .add_property("hash_code", &AtomBase::GetHashCode)
    .def("GetHashCode", &AtomBase::GetHashCode)
    .def("GetProp", &AtomBase::GetProp,
         return_value_policy<copy_const_reference>())
    .def("SetProp", &AtomBase::SetProp, args("prop"))
    .def("GetIndex", &AtomBase::GetIndex)
    .add_property("prop",
                  make_function(&AtomBase::GetProp,
                                return_value_policy<copy_const_reference>()))
    .add_property("pos",
                  make_function(&AtomBase::GetPos,
                                return_value_policy<copy_const_reference>()))
    .add_property("name",
                  make_function(&AtomBase::GetName,
                                return_value_policy<copy_const_reference>()),
                  &AtomBase::SetName)
    .add_property("index",&AtomBase::GetIndex)

  ;
  generic_prop_def<AtomBase>(atom_base);

  class_<AtomHandle, bases<AtomBase> >("AtomHandle", init<>())
    .def("GetResidue",&AtomHandle::GetResidue)
    .add_property("residue",&AtomHandle::GetResidue)
    .def("Apply", &AtomHandle::Apply, args("visitor"))
    .def("GetBondList", &AtomHandle::GetBondList)
    .def("GetBondCount", &AtomHandle::GetBondCount)
    .def("GetEntity", &AtomHandle::GetEntity)
    .def("GetHandle", &AtomHandle::GetHandle)
    .add_property("handle", &AtomHandle::GetHandle)
    .add_property("entity", &AtomHandle::GetEntity)
    .def("GetBondPartners", &AtomHandle::GetBondPartners)
    .def("FindBondToAtom", &AtomHandle::FindBondToAtom, args("other_atom"))
    .def(self==self)
    .def(self!=self)
    .def("__hash__", &AtomHandle::GetHashCode)
  ;

  class_<AtomHandleList>("AtomHandleList", no_init)
    .def(vector_indexing_suite<AtomHandleList>())
  ;
  class_<AtomProp>("AtomProp", init<>())
    .def_readwrite("element", &AtomProp::element)
    .def_readwrite("radius", &AtomProp::radius)
    .def_readwrite("charge", &AtomProp::charge)
    .def_readwrite("mass", &AtomProp::mass)
    .def_readwrite("occupancy", &AtomProp::occupancy)
    .def_readwrite("b_factor", &AtomProp::b_factor)
    .def_readwrite("is_hetatm", &AtomProp::is_hetatm)
    .def_readwrite("anisou", &AtomProp::anisou)
  ;
}

