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


BOOST_AUTO_TEST_CASE(test_pdb_import_handler) 
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

BOOST_AUTO_TEST_CASE(atom_record)
{
  String fname("testfiles/pdb/atom.pdb");
  PDBReader reader(fname); 
  mol::EntityHandle ent=mol::CreateEntity();
  reader.Import(ent);
  BOOST_REQUIRE_EQUAL(ent.GetChainCount(), 2);
  BOOST_REQUIRE_EQUAL(ent.GetResidueCount(), 2);
  BOOST_REQUIRE_EQUAL(ent.GetAtomCount(), 2);
  mol::AtomHandle a1=ent.FindAtom("A", mol::ResNum(1), "N");
  BOOST_REQUIRE(a1.IsValid());
  BOOST_CHECK_EQUAL(a1.GetName(), "N");
  BOOST_CHECK_EQUAL(a1.GetResidue().GetName(), "MET");
  BOOST_CHECK_EQUAL(a1.GetResidue().GetChain().GetName(), "A");  
  
  BOOST_CHECK_EQUAL(a1.GetPos(), geom::Vec3(16.0, 64.0, 8.0));
  BOOST_CHECK_EQUAL(a1.GetProp().b_factor, 1.0);
  BOOST_CHECK_EQUAL(a1.GetProp().occupancy, 0.5);  
  BOOST_CHECK_EQUAL(a1.GetProp().element, "N");
  BOOST_CHECK_EQUAL(a1.GetProp().is_hetatm, false);
  mol::AtomHandle a2=ent.FindAtom(" ", mol::ResNum(1), "CA");
  BOOST_REQUIRE(a2.IsValid());  
  BOOST_CHECK_EQUAL(a2.GetName(), "CA");
  BOOST_CHECK_EQUAL(a2.GetResidue().GetName(), "MET");
  BOOST_CHECK_EQUAL(a2.GetResidue().GetChain().GetName(), " ");  
  
  BOOST_CHECK_EQUAL(a2.GetPos(), geom::Vec3(32.0, -128.0, -2.5));
  BOOST_CHECK_EQUAL(a2.GetProp().b_factor, 128.0);
  BOOST_CHECK_EQUAL(a2.GetProp().occupancy, 1.0);  
  BOOST_CHECK_EQUAL(a2.GetProp().element, "C");
  BOOST_CHECK_EQUAL(a2.GetProp().is_hetatm, true);
}

BOOST_AUTO_TEST_CASE(end_record)
{
  String fname("testfiles/pdb/end.pdb");
  PDBReader reader(fname); 
  mol::EntityHandle ent=mol::CreateEntity();
  reader.Import(ent);
  BOOST_CHECK_EQUAL(ent.GetAtomCount(), 1);
}

BOOST_AUTO_TEST_CASE(join_spread_records_on)
{
  String fname("testfiles/pdb/join-spread-records.pdb");
  PDBReader reader(fname);
  
  mol::EntityHandle ent=mol::CreateEntity();
  PDB::PushFlags(PDB::JOIN_SPREAD_ATOM_RECORDS);
  reader.Import(ent);
  PDB::PopFlags();
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
  BOOST_CHECK_EQUAL(ent.GetResidueCount(), 3);
  mol::ResidueHandle res1=ent.FindResidue("A", mol::ResNum(1));
  BOOST_CHECK(res1.IsValid());
  BOOST_CHECK_EQUAL(res1.GetAtomCount(), 1);
  BOOST_CHECK(res1.FindAtom("N"));
  mol::ResidueHandle res2=ent.FindResidue("A", mol::ResNum(2));  
  BOOST_REQUIRE(res2.IsValid());
  BOOST_CHECK_EQUAL(res2.GetAtomCount(), 1);
  BOOST_CHECK(res2.FindAtom("N"));
}

BOOST_AUTO_TEST_CASE(calpha_only_import_on)
{
  String fname("testfiles/pdb/calpha.pdb");
  PDBReader reader(fname);
  PDB::PushFlags(PDB::CALPHA_ONLY);
  mol::EntityHandle ent=mol::CreateEntity();
  reader.Import(ent);
  PDB::PopFlags();
  BOOST_CHECK_EQUAL(ent.GetResidueCount(), 1);
  BOOST_CHECK_EQUAL(ent.GetAtomCount(), 1);
}

BOOST_AUTO_TEST_CASE(calpha_only_import_off)
{
  String fname("testfiles/pdb/calpha.pdb");
  PDBReader reader(fname);
  mol::EntityHandle ent=mol::CreateEntity();
  reader.Import(ent);
  BOOST_CHECK_EQUAL(ent.GetResidueCount(), 2);
  BOOST_CHECK_EQUAL(ent.GetAtomCount(), 4);
}

BOOST_AUTO_TEST_CASE(anisou_record)
{
  String fname("testfiles/pdb/anisou.pdb");
  PDBReader reader(fname);
  mol::EntityHandle ent=mol::CreateEntity();
  reader.Import(ent);
  BOOST_REQUIRE(ent.GetAtomCount()==1);
  mol::AtomHandle a1=ent.FindAtom("A", mol::ResNum(7), "N");
  BOOST_REQUIRE(a1.IsValid());
  mol::AtomProp props=a1.GetProp();
  BOOST_CHECK_CLOSE( 0.0100, props.anisou(0, 0), 1e-4);
  BOOST_CHECK_CLOSE(-0.0016, props.anisou(1, 0), 1e-4);
  BOOST_CHECK_CLOSE(-0.0026, props.anisou(2, 0), 1e-4);
  BOOST_CHECK_CLOSE(-0.0016, props.anisou(0, 1), 1e-4);
  BOOST_CHECK_CLOSE( 0.0110, props.anisou(1, 1), 1e-4);
  BOOST_CHECK_CLOSE(-0.0054, props.anisou(2, 1), 1e-4);
  BOOST_CHECK_CLOSE(-0.0026, props.anisou(0, 2), 1e-4);
  BOOST_CHECK_CLOSE(-0.0054, props.anisou(1, 2), 1e-4);
  BOOST_CHECK_CLOSE( 0.0120, props.anisou(2, 2), 1e-4);    
}


BOOST_AUTO_TEST_CASE(only_66_cols)
{
  String fname("testfiles/pdb/short.pdb");
  PDBReader reader(fname);
  mol::EntityHandle ent=mol::CreateEntity();
  reader.Import(ent);
}

BOOST_AUTO_TEST_SUITE_END()
