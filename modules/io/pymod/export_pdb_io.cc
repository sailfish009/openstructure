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
using namespace boost::python;

#include <ost/io/mol/pdb_reader.hh>
#include <ost/io/mol/pdb_writer.hh>
using namespace ost;
using namespace ost::io;

BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS(X_import,
                                       PDBReader::Import, 1, 2)

BOOST_PYTHON_FUNCTION_OVERLOADS(load_PDB_ov, LoadPDB, 1, 2)

void (PDBWriter::*write_a)(const mol::EntityHandle&)=&PDBWriter::Write;
void (PDBWriter::*write_b)(const mol::EntityView&)=&PDBWriter::Write;

namespace {
  void push_flags(unsigned int x) {PDB::PushFlags(x);}
  unsigned int flags() {return PDB::Flags();}
  void pop_flags() {PDB::PopFlags();}
}

void export_pdb_io()
{
  {
    scope pdb_scope=class_<PDB>("PDB",no_init)
      .def("PushFlags",&PDB::PushFlags)
      .staticmethod("PushFlags")
      .def("Flags",&PDB::Flags)
      .staticmethod("Flags")
      .def("PopFlags",&PDB::PopFlags)
      .staticmethod("PopFlags")
      ;
    pdb_scope.attr("NO_HETATMS")=PDB::NO_HETATMS;
    pdb_scope.attr("SKIP_FAULTY_RECORDS")=PDB::SKIP_FAULTY_RECORDS;
    pdb_scope.attr("WRITE_MULTIPLE_MODELS")=PDB::WRITE_MULTIPLE_MODELS;
    pdb_scope.attr("JOIN_SPREAD_ATOM_RECORDS")=PDB::JOIN_SPREAD_ATOM_RECORDS;
    pdb_scope.attr("CALPHA_ONLY")=PDB::CALPHA_ONLY;    
  }

  class_<PDBReader, boost::noncopyable>("PDBReader", init<String>())
    .def("HasNext", &PDBReader::HasNext)
    .def("Import", &PDBReader::Import, 
         X_import(args("entity", "restrict_chains")))
  ;
  
  class_<PDBWriter, boost::noncopyable>("PDBWriter", init<String>())
    .def("Write", write_a)
    .def("Write", write_b)    
  ;
}
