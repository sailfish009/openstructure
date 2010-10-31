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
#include <ost/test_utils/compare_files.hh>
#include <ost/mol/mol.hh>
#include <ost/conop/conop.hh>
#include <ost/io/mol/entity_io_pdb_handler.hh>
#include <ost/io/pdb_reader.hh>
#include <ost/io/pdb_writer.hh>
#include <ost/io/io_exception.hh>
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
  BOOST_CHECK_EQUAL(a1.GetBFactor(), 1.0);
  BOOST_CHECK_EQUAL(a1.GetOccupancy(), 0.5);  
  BOOST_CHECK_EQUAL(a1.GetElement(), "N");
  BOOST_CHECK_EQUAL(a1.IsHetAtom(), false);
  mol::AtomHandle a2=ent.FindAtom(" ", mol::ResNum(1), "CA");
  BOOST_REQUIRE(a2.IsValid());  
  BOOST_CHECK_EQUAL(a2.GetName(), "CA");
  BOOST_CHECK_EQUAL(a2.GetResidue().GetName(), "MET");
  BOOST_CHECK_EQUAL(a2.GetResidue().GetChain().GetName(), " ");  
  
  BOOST_CHECK_EQUAL(a2.GetPos(), geom::Vec3(32.0, -128.0, -2.5));
  BOOST_CHECK_EQUAL(a2.GetBFactor(), 128.0);
  BOOST_CHECK_EQUAL(a2.GetOccupancy(), 1.0);  
  BOOST_CHECK_EQUAL(a2.GetElement(), "C");
  BOOST_CHECK_EQUAL(a2.IsHetAtom(), true);
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

BOOST_AUTO_TEST_CASE(het_import)
{
  String fname("testfiles/pdb/het.pdb");
  PDBReader reader(fname);
  mol::EntityHandle ent=mol::CreateEntity();
  reader.Import(ent);
  BOOST_CHECK_EQUAL(ent.Select("ligand=true").GetResidueCount(), 1);
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
  const geom::Mat3& anisou=a1.GetAnisou();
  BOOST_CHECK_CLOSE(Real( 0.0100), anisou(0, 0), Real(1e-4));
  BOOST_CHECK_CLOSE(Real(-0.0016), anisou(1, 0), Real(1e-4));
  BOOST_CHECK_CLOSE(Real(-0.0026), anisou(2, 0), Real(1e-4));
  BOOST_CHECK_CLOSE(Real(-0.0016), anisou(0, 1), Real(1e-4));
  BOOST_CHECK_CLOSE(Real( 0.0110), anisou(1, 1), Real(1e-4));
  BOOST_CHECK_CLOSE(Real(-0.0054), anisou(2, 1), Real(1e-4));
  BOOST_CHECK_CLOSE(Real(-0.0026), anisou(0, 2), Real(1e-4));
  BOOST_CHECK_CLOSE(Real(-0.0054), anisou(1, 2), Real(1e-4));
  BOOST_CHECK_CLOSE(Real( 0.0120), anisou(2, 2), Real(1e-4));    
}

BOOST_AUTO_TEST_CASE(only_66_cols)
{
  String fname("testfiles/pdb/short.pdb");
  PDBReader reader(fname);
  mol::EntityHandle ent=mol::CreateEntity();
  reader.Import(ent);
}

BOOST_AUTO_TEST_CASE(no_endmdl_record)
{
  String fname("testfiles/pdb/model.pdb");
  PDBReader reader(fname);
  mol::EntityHandle ent=mol::CreateEntity();
  BOOST_CHECK_THROW(reader.Import(ent), IOException);
}

BOOST_AUTO_TEST_CASE(deuterium_import)
{
  String fname("testfiles/pdb/val-with-deuterium.pdb");
  PDBReader reader(fname);
  mol::EntityHandle ent=mol::CreateEntity();
  reader.Import(ent);
  // we use conopology to mark amino acids as peptide-linking. 
  conop::Conopology& conop_inst=conop::Conopology::Instance();
  conop_inst.ConnectAll(conop_inst.GetBuilder(), ent);
  // this check makes sure that we correctly detect deal with the deuterium
  // atoms in the residue.
  BOOST_CHECK(ent.FindResidue("A", 297).IsPeptideLinking());
}

BOOST_AUTO_TEST_CASE(faulty_lines)
{
  String fname("testfiles/pdb/faulty.pdb");
  PDBReader reader(fname);
  mol::EntityHandle ent=mol::CreateEntity();
  BOOST_CHECK_THROW(reader.Import(ent), IOException);

  PDB::PushFlags(PDB::SKIP_FAULTY_RECORDS);
  reader.Import(ent);
}

BOOST_AUTO_TEST_CASE(write_atom)
{
  std::stringstream out;
  PDBWriter writer(out);
  
  mol::EntityHandle ent=mol::CreateEntity();
  mol::XCSEditor edi=ent.EditXCS();
  mol::ChainHandle ch=edi.InsertChain("A");
  mol::ResidueHandle r=edi.AppendResidue(ch, "GLY");

  mol::AtomHandle a=edi.InsertAtom(r, "CA", geom::Vec3(32.0, -128.0, -2.5), "C");
  a.SetOccupancy(1.0);
  a.SetBFactor(128.0);
  writer.Write(ent);
  String s=out.str();
  BOOST_CHECK_EQUAL(s.substr(0, 54), 
                    "ATOM      1  CA  GLY A   1      32.000-128.000  -2.500");
  BOOST_CHECK_EQUAL(s.substr(54, 26), 
                    "  1.00128.00           C  ");
}

BOOST_AUTO_TEST_CASE(write_hetatom)
{
  std::stringstream out;
  PDBWriter writer(out);
  
  mol::EntityHandle ent=mol::CreateEntity();
  mol::XCSEditor edi=ent.EditXCS();
  mol::ChainHandle ch=edi.InsertChain("A");
  mol::ResidueHandle r=edi.AppendResidue(ch, "CA");
  mol::AtomHandle a=edi.InsertAtom(r, "CA", geom::Vec3(32.0, -128.0, -2.5), 
                                   "CA");
  a.SetHetAtom(true);
  a.SetMass(40.01);
  a.SetOccupancy(1.0);
  a.SetBFactor(40.75);
  writer.Write(ent);
  String s=out.str();
  BOOST_CHECK_EQUAL(s.substr(0, 54), 
                    "HETATM    1 CA    CA A   1      32.000-128.000  -2.500");
  BOOST_CHECK_EQUAL(s.substr(54, 26), 
                    "  1.00 40.75          CA  ");
}

BOOST_AUTO_TEST_CASE(no_endmdl_record_fault_tolerant)
{
  String fname("testfiles/pdb/model.pdb");
  PDBReader reader(fname);
  PDB::PushFlags(PDB::SKIP_FAULTY_RECORDS);  
  mol::EntityHandle ent=mol::CreateEntity();
  reader.Import(ent);
  BOOST_CHECK_EQUAL(ent.GetChainCount(), 1);
  BOOST_CHECK_EQUAL(ent.GetResidueCount(), 1);  
  BOOST_CHECK_EQUAL(ent.GetAtomCount(), 3);
  ent=mol::CreateEntity();
  reader.Import(ent);
  BOOST_CHECK_EQUAL(ent.GetChainCount(), 1);
  BOOST_CHECK_EQUAL(ent.GetResidueCount(), 1);  
  BOOST_CHECK_EQUAL(ent.GetAtomCount(), 2);  
  PDB::PopFlags();
}

BOOST_AUTO_TEST_CASE(alt_loc_import_export)
{
  String fname("testfiles/pdb/alt-loc.pdb");  
  // this scope is required to force the writer stream to be closed before 
  // opening the file again in compare_files. Avoids a race condition.
  {
    PDBReader reader(fname);
    PDBWriter writer(String("testfiles/pdb/alt-loc-out.pdb"));
    
    mol::EntityHandle ent=mol::CreateEntity();
    reader.Import(ent);
    writer.Write(ent);
  }
  BOOST_CHECK(compare_files("testfiles/pdb/alt-loc.pdb", 
                            "testfiles/pdb/alt-loc-out.pdb"));
}

BOOST_AUTO_TEST_CASE(write_ter)
{
  String fname("testfiles/pdb/ter.pdb");  
  // this scope is required to force the writer stream to be closed before 
  // opening the file again in compare_files. Avoids a race condition.
  {
    PDBReader reader(fname);
    PDBWriter writer(String("testfiles/pdb/ter-out.pdb"));
    
    mol::EntityHandle ent=mol::CreateEntity();
    reader.Import(ent);
    // we use conopology to mark amino acids as peptide-linking. this is require 
    // for proper TER output
    conop::Conopology& conop_inst=conop::Conopology::Instance();
    conop_inst.ConnectAll(conop_inst.GetBuilder(), ent);
    writer.Write(ent);
  }
  BOOST_CHECK(compare_files("testfiles/pdb/ter.pdb", 
                            "testfiles/pdb/ter-out.pdb"));
}

BOOST_AUTO_TEST_CASE(write_ter2)
{
  String fname("testfiles/pdb/ter2.pdb");  
  // this scope is required to force the writer stream to be closed before 
  // opening the file again in compare_files. Avoids a race condition.
  {
    PDBReader reader(fname);
    PDBWriter writer(String("testfiles/pdb/ter2-out.pdb"));
    
    mol::EntityHandle ent=mol::CreateEntity();
    reader.Import(ent);
    // we use conopology to mark amino acids as peptide-linking. this is 
    // require for proper TER output
    conop::Conopology& conop_inst=conop::Conopology::Instance();
    conop_inst.ConnectAll(conop_inst.GetBuilder(), ent);
    writer.Write(ent);
  }
  BOOST_CHECK(compare_files("testfiles/pdb/ter2.pdb", 
                            "testfiles/pdb/ter2-out.pdb"));
}

BOOST_AUTO_TEST_CASE(write_ter3)
{
  String fname("testfiles/pdb/ter3.pdb");  
  // this scope is required to force the writer stream to be closed before 
  // opening the file again in compare_files. Avoids a race condition.
  {
    PDBReader reader(fname);
    PDBWriter writer(String("testfiles/pdb/ter3-out.pdb"));
    
    mol::EntityHandle ent=mol::CreateEntity();
    reader.Import(ent);
    // we use conopology to mark amino acids as peptide-linking. this is 
    // require for proper TER output
    conop::Conopology& conop_inst=conop::Conopology::Instance();
    conop_inst.ConnectAll(conop_inst.GetBuilder(), ent);
    writer.Write(ent);
  }
  BOOST_CHECK(compare_files("testfiles/pdb/ter3.pdb", 
                            "testfiles/pdb/ter3-out.pdb"));
}

BOOST_AUTO_TEST_CASE(write_conect)
{
  // this scope is required to force the writer stream to be closed before
  // opening the file again in compare_files. Avoids a race condition.
  {
    PDBReader reader(String("testfiles/pdb/conect.pdb"));
    PDBWriter writer(String("testfiles/pdb/conect-out.pdb"));
    mol::EntityHandle ent=mol::CreateEntity();
    reader.Import(ent);
    conop::Conopology& conop_inst=conop::Conopology::Instance();
    conop_inst.ConnectAll(conop_inst.GetBuilder(), ent);
    writer.Write(ent);
  }
  BOOST_CHECK(compare_files("testfiles/pdb/conect.pdb",
                            "testfiles/pdb/conect-out.pdb"));
}

BOOST_AUTO_TEST_CASE(res_name_too_long)
{
  std::stringstream out;
  PDBWriter writer(out);
  
  mol::EntityHandle ent=mol::CreateEntity();
  mol::XCSEditor edi=ent.EditXCS();
  mol::ChainHandle ch=edi.InsertChain("A");
  mol::ResidueHandle r=edi.AppendResidue(ch, "CALCIUM");
  mol::AtomHandle a=edi.InsertAtom(r, "CA", geom::Vec3(32.0, -128.0, -2.5));
  BOOST_CHECK_THROW(writer.Write(ent), IOException);
}

BOOST_AUTO_TEST_CASE(chain_name_too_long)
{
  std::stringstream out;
  PDBWriter writer(out);
  
  mol::EntityHandle ent=mol::CreateEntity();
  mol::XCSEditor edi=ent.EditXCS();
  mol::ChainHandle ch=edi.InsertChain("AB");
  mol::ResidueHandle r=edi.AppendResidue(ch, "CA");
  mol::AtomHandle a=edi.InsertAtom(r, "CA", geom::Vec3(32.0, -128.0, -2.5));
  BOOST_CHECK_THROW(writer.Write(ent), IOException);
}

BOOST_AUTO_TEST_CASE(atom_name_too_long)
{
  std::stringstream out;
  PDBWriter writer(out);
  
  mol::EntityHandle ent=mol::CreateEntity();
  mol::XCSEditor edi=ent.EditXCS();
  mol::ChainHandle ch=edi.InsertChain("A");
  mol::ResidueHandle r=edi.AppendResidue(ch, "CA");
  mol::AtomHandle a=edi.InsertAtom(r, "CALCIUM", geom::Vec3(32.0, -128.0, -2.5));
  BOOST_CHECK_THROW(writer.Write(ent), IOException);
}



BOOST_AUTO_TEST_SUITE_END()
