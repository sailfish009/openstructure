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
#include <ost/export_helper/vector.hh>
using namespace ost;
using namespace ost::mol;
#include <ost/export_helper/generic_property_def.hh>
#include "bounds.hh"

namespace {
  typedef void (ChainHandle::*RnumMethod)(const ResNum&);
  typedef void (ChainHandle::*RhandleMethod)(const ResidueHandle&);  
  
  
  typedef ResidueHandle (ChainHandle::*SingleArgMethod)(const ResidueKey&);  
  typedef ResidueHandle (ChainHandle::*DoubleArgMethod)(const ResidueKey&, const ResNum&);    
 
  typedef EntityView (ChainHandle::*QueryMethod)(const Query&, uint) const;
  typedef EntityView (ChainHandle::*StringMethod)(const String&, uint) const;  
  QueryMethod select_query=&ChainHandle::Select;
  StringMethod select_string=&ChainHandle::Select; 
}

void export_Chain()
{
  class_<ChainBase> chain_base("ChainBase", no_init);
  chain_base
    .def("GetName", &ChainBase::GetName)
    .add_property("name", &ChainBase::GetName)
    .def(self_ns::str(self))
    .add_property("valid", &ChainBase::IsValid)
    .def("IsValid", &ChainBase::IsValid)
  ;
  generic_prop_def<ChainBase>(chain_base);
  class_<ChainHandle, bases<ChainBase> >("ChainHandle", init<>())
    .def("GetAtomList", &ChainHandle::GetAtomList)
    .add_property("atoms", &ChainHandle::GetAtomList)
    .def("GetResidueList", &ChainHandle::GetResidueList)
    .add_property("residues", &ChainHandle::GetResidueList)   
    .add_property("entity", &ChainHandle::GetEntity) 
    .def("GetEntity", &ChainHandle::GetEntity)
    .def("GetResidueByIndex", &ChainHandle::GetResidueByIndex)
    .def("GetHandle", &ChainHandle::GetHandle)
    .add_property("handle", &ChainHandle::GetHandle)    
    .def("FindResidue", &ChainHandle::FindResidue, arg("residue_number"))    
    .def("FindAtom", &ChainHandle::FindAtom, args("residue_number", "atom_name"))        
    //.def("AppendResidue", append_one_arg, args("residue_key"))
    //.def("AppendResidue", append_two_arg, args("residue_key", 
    //                                            "residue_number"))
    .def("GetNext", &ChainHandle::GetNext)
    .def("GetPrev", &ChainHandle::GetPrev)
    //.def("InsertResidueBefore", &ChainHandle::InsertResidueBefore, 
    //     args("residue_number", "key"))
    .def("GetPrev", &ChainHandle::GetPrev)
    //.def("DeleteResidue", delete_by_number, args("residue_number")) 
    //.def("DeleteResidue", delete_by_handle, args("residue_handle"))        
    .def("GetResidueCount", &ChainHandle::GetResidueCount)
    .def("GetAtomCount", &ChainHandle::GetAtomCount)
    .def("GetBondCount", &ChainHandle::GetBondCount)   
    .add_property("residue_count", &ChainHandle::GetResidueCount)
    .add_property("atom_count", &ChainHandle::GetAtomCount)    
    .def("InSequence", &ChainHandle::InSequence)
    .def("Select", select_string, arg("flags")=0)
    .def("Select", select_query, arg("flags")=0)
    .def("GetMass", &ChainHandle::GetMass)
    .def("GetCenterOfMass", &ChainHandle::GetCenterOfMass)
    .def("GetCenterOfAtoms", &ChainHandle::GetCenterOfAtoms)
    .def("GetGeometricCenter", geom_center<ChainHandle>)
    .add_property("geometric_center", geom_center<ChainHandle>)
    .add_property("mass", &ChainHandle::GetMass)
    .add_property("center_of_mass", &ChainHandle::GetCenterOfMass)
    .add_property("center_of_atoms", &ChainHandle::GetCenterOfAtoms)  
    .add_property("in_sequence", &ChainHandle::InSequence)  
    .def("GetBounds", &ChainHandle::GetBounds)
    .add_property("bounds", &ChainHandle::GetBounds)
    .def("GetGeometricStart", geom_start<ChainHandle>)
    .def("GetGeometricEnd", geom_end<ChainHandle>)
    .def(self==self)
    .def(self!=self)
  ;
  
  class_<ChainHandleList>("ChainHandleList", no_init)
    .def(vector_indexing_suite<ChainHandleList>())
    .def(ost::VectorAdditions<ChainHandleList>())    
  ;
}
