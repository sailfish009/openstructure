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

namespace {
  typedef void (ChainHandle::*RnumMethod)(const ResNum&);
  typedef void (ChainHandle::*RhandleMethod)(const ResidueHandle&);  
  
  //RnumMethod delete_by_number=&ChainHandle::DeleteResidue;
  //RhandleMethod delete_by_handle=&ChainHandle::DeleteResidue;
  
  typedef ResidueHandle (ChainHandle::*SingleArgMethod)(const ResidueKey&);  
  typedef ResidueHandle (ChainHandle::*DoubleArgMethod)(const ResidueKey&, const ResNum&);    
  
  //SingleArgMethod append_one_arg=&ChainHandle::AppendResidue;
  //DoubleArgMethod append_two_arg=&ChainHandle::AppendResidue;  
}

void export_Chain()
{
  class_<ChainBase, bases<GenericPropertyContainer> >("ChainBase", no_init)
    .def("GetName", &ChainBase::GetName)
    .add_property("name", &ChainBase::GetName)
    .def("IsValid", &ChainBase::IsValid)    
    .def(self_ns::str(self))
  ;
  class_<ChainHandle, bases<ChainBase> >("ChainHandle", init<>())
    .def("GetResidueList", &ChainHandle::GetResidueList)
    .add_property("residues", &ChainHandle::GetResidueList)   
    .add_property("entity", &ChainHandle::GetEntity) 
    .def("GetEntity", &ChainHandle::GetEntity)
    .def("GetResidueByIndex", &ChainHandle::GetResidueByIndex)
    .def("FindResidue", &ChainHandle::FindResidue, arg("residue_number"))    
    .def("FindAtom", &ChainHandle::FindAtom, args("residue_number", "atom_name"))        
    //.def("AppendResidue", append_one_arg, args("residue_key"))
    //.def("AppendResidue", append_two_arg, args("residue_key", 
    //                                            "residue_number"))
    .def("Apply", &ChainHandle::Apply, args("visitor"))
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
    .def("GetGeometricCenter", &ChainHandle::GetGeometricCenter)
    .add_property("geometric_center", &ChainHandle::GetGeometricCenter)    
  ;
  
  class_<ChainHandleList>("ChainHandleList", no_init)
    .def(vector_indexing_suite<ChainHandleList>())
  ;
}
