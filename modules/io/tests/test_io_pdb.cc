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
#include <ost/mol/mol.hh>
#include <ost/io/mol/entity_io_pdb_handler.hh>
#include <ost/io/pdb_reader.hh>
#define BOOST_TEST_DYN_LINK
#include <boost/test/unit_test.hpp>
using boost::unit_test_framework::test_suite;

using namespace ost;
using namespace ost::io;

BOOST_AUTO_TEST_SUITE( io )

BOOST_AUTO_TEST_CASE(test_io_pdb) 
{
  String fname("testfiles/test_in.pdb");

  mol::EntityHandle eh=mol::CreateEntity();
  EntityIOPDBHandler pdbh;

  BOOST_CHECK(EntityIOPDBHandler::ProvidesImport("","pdb"));
  BOOST_CHECK(EntityIOPDBHandler::ProvidesImport(fname));
  BOOST_CHECK(EntityIOPDBHandler::ProvidesImport("test_in.PDB"));
  
  BOOST_CHECK(EntityIOPDBHandler::ProvidesExport("","pdb"));
  BOOST_CHECK(EntityIOPDBHandler::ProvidesExport(fname));
  BOOST_CHECK(EntityIOPDBHandler::ProvidesExport("test_in.PDB"));
    
  pdbh.Import(eh,"testfiles/pdb/simple.pdb");
}

BOOST_AUTO_TEST_CASE(join_spread_records_on)
{
  String fname("testfiles/pdb/join-spread-records.pdb");
  PDBReader reader(fname);
  reader.SetFlags(PDB::JOIN_SPREAD_ATOM_RECORDS);
  
  mol::EntityHandle ent=mol::CreateEntity();
  reader.Import(ent);
  BOOST_CHECK_EQUAL(ent.GetResidueCount(), 2);
  mol::ResidueHandle res1=ent.FindResidue("A", mol::ResNum(1));
  BOOST_CHECK(res1.IsValid());
  BOOST_CHECK_EQUAL(res1.GetAtomCount(), 2);
  BOOST_CHECK(res1.FindAtom("N"));
  BOOST_CHECK(res1.FindAtom("CE"));
  mol::ResidueHandle res2=ent.FindResidue("A", mol::ResNum(2));  
  BOOST_CHECK(res2.IsValid());
  BOOST_CHECK_EQUAL(res2.GetAtomCount(), 1);
  BOOST_CHECK(res2.FindAtom("N"));
}

BOOST_AUTO_TEST_CASE(join_spread_records_off)
{
  String fname("testfiles/pdb/join-spread-records.pdb");
  PDBReader reader(fname);
  mol::EntityHandle ent=mol::CreateEntity();
  reader.Import(ent);
  BOOST_CHECK_EQUAL(ent.GetResidueCount(), 2);
  mol::ResidueHandle res1=ent.FindResidue("A", mol::ResNum(1));
  BOOST_CHECK(res1.IsValid());
  BOOST_CHECK_EQUAL(res1.GetAtomCount(), 1);
  BOOST_CHECK(res1.FindAtom("N"));
  mol::ResidueHandle res2=ent.FindResidue("A", mol::ResNum(2));  
  BOOST_CHECK(res2.IsValid());
  BOOST_CHECK_EQUAL(res2.GetAtomCount(), 1);
  BOOST_CHECK(res2.FindAtom("N"));
}

BOOST_AUTO_TEST_SUITE_END()