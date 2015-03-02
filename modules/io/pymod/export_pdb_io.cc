//------------------------------------------------------------------------------
// This file is part of the OpenStructure project <www.openstructure.org>
//
// Copyright (C) 2008-2015 by the OpenStructure authors
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

#include <ost/io/mol/io_profile.hh>
#include <ost/io/mol/pdb_reader.hh>
#include <ost/io/mol/pdb_writer.hh>
using namespace ost;
using namespace ost::io;

BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS(X_import,
                                       PDBReader::Import, 1, 2)

BOOST_PYTHON_FUNCTION_OVERLOADS(load_PDB_ov, LoadPDB, 1, 2)

void (PDBWriter::*write_a)(const mol::EntityHandle&)=&PDBWriter::Write;
void (PDBWriter::*write_b)(const mol::EntityView&)=&PDBWriter::Write;

void export_pdb_io()
{
  class_<IOProfile>("IOProfile",
         init<String,bool,bool,bool,bool,bool,bool,bool>((arg("dialect")="PDB",
                                                          arg("strict_hydrogens")=false,
                                                          arg("quack_mode")=false,
                                                          arg("fault_tolerant")=false,
                                                          arg("join_spread_atom_records")=false,
                                                          arg("no_hetatms")=false,
                                                          arg("calpha_only")=false,
						          arg("bond_feasibility_check")=true)))
    .def_readwrite("dialect", &IOProfile::dialect)
    .def_readwrite("fault_tolerant", &IOProfile::fault_tolerant)
    .def_readwrite("quack_mode", &IOProfile::quack_mode)
    .def_readwrite("strict_hydrogens", &IOProfile::strict_hydrogens)
    .def_readwrite("no_hetatms", &IOProfile::no_hetatms)
    .def_readwrite("calpha_only", &IOProfile::calpha_only)
    .def_readwrite("join_spread_atom_records", &IOProfile::join_spread_atom_records)
    .def_readwrite("bond_feasibility_check", &IOProfile::bond_feasibility_check)
    .def("Copy", &IOProfile::Copy)
    .def(self_ns::str(self))
  ;
  class_<IOProfileRegistry>("IOProfileRegistry", no_init)
    .def("Get", &IOProfileRegistry::Get,  
         return_value_policy<reference_existing_object>())
    .def("Set", &IOProfileRegistry::Set)
    .def("Instance", &IOProfileRegistry::Instance,
         return_value_policy<reference_existing_object>()).staticmethod("Instance")
    .def("GetDefault", &IOProfileRegistry::GetDefault,
         return_value_policy<reference_existing_object>())
  ;
  class_<PDBReader, boost::noncopyable>("PDBReader", init<String, const IOProfile&>())
    .def("HasNext", &PDBReader::HasNext)
    .def("Import", &PDBReader::Import, 
         X_import(args("entity", "restrict_chains")))
    .add_property("read_seqres", &PDBReader::GetReadSeqRes, 
                  &PDBReader::SetReadSeqRes)
    .add_property("seqres", &PDBReader::GetSeqRes)
  ;
  
  class_<PDBWriter, boost::noncopyable>("PDBWriter", init<String, const IOProfile&>())
    .def("Write", write_a)
    .def("SetIsPQR", &PDBWriter::SetIsPQR)
    .def("IsPQR", &PDBWriter::IsPQR)
    .def("SetWriteMultiModel", &PDBWriter::SetWriteMultiModel)
    .def("GetWriteMultiModel", &PDBWriter::GetWriteMultiModel)
    .add_property("write_multi_model", &PDBWriter::GetWriteMultiModel, 
                  &PDBWriter::SetWriteMultiModel)
    .def("Write", write_b)    
  ;
}
