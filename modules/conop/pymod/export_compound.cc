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
#include <boost/python/register_ptr_to_python.hpp>
#include <boost/python/suite/indexing/vector_indexing_suite.hpp>
using namespace boost::python;
#include <ost/conop/compound.hh>
#include <ost/conop/compound_lib.hh>

using namespace ost::conop;

void export_Compound() {
  class_<Compound>("Compound", no_init)
    .def("GetID", &Compound::GetID, 
         return_value_policy<copy_const_reference>())
    .def("SetOneLetterCode", &Compound::SetOneLetterCode)
    .def("GetOneLetterCode", &Compound::GetOneLetterCode)
    .add_property("one_letter_code", &Compound::GetOneLetterCode, 
                  &Compound::SetOneLetterCode)
    .def("GetAtomSpecs", &Compound::GetAtomSpecs,
         return_value_policy<copy_const_reference>())
  ;
  
  class_<AtomSpec>("AtomSpec", no_init)
    .def_readonly("element", &AtomSpec::element)
    .def_readonly("name", &AtomSpec::name)    
    .def_readonly("alt_name", &AtomSpec::alt_name)
    .def_readonly("is_leaving", &AtomSpec::is_leaving)    
  ;
  
  class_<BondSpec>("BondSpec", no_init)
    .def_readonly("atom_one", &BondSpec::atom_one)
    .def_readonly("atom_two", &BondSpec::atom_two)    
  ;  
  register_ptr_to_python<CompoundPtr>();  
  
  class_<CompoundLib>("CompoundLib", no_init)
    .def("Load", &CompoundLib::Load).staticmethod("Load")
    .def("FindCompound", &CompoundLib::FindCompound)
    .def("ClearCache", &CompoundLib::ClearCache)
  ;
  
  class_<AtomSpecList>("AtomSpecList", init<>())
    .def(vector_indexing_suite<AtomSpecList>())
  ;  
  
  class_<BondSpecList>("BondSpecList", init<>())
    .def(vector_indexing_suite<BondSpecList>())
  ;  
  register_ptr_to_python<CompoundLibPtr>();    
}
