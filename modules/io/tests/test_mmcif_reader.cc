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

#include <fstream>
#include <ost/io/io_exception.hh>
#include <ost/io/mol/mmcif_reader.hh>

#define BOOST_AUTO_TEST_DYN_LINK
#include <boost/test/unit_test.hpp>


using namespace ost;
using namespace ost::io;

class TestMMCifParserProtected : MMCifParser {
public:
  TestMMCifParserProtected(std::istream& stream, mol::EntityHandle& ent_handle):
    MMCifParser(stream, ent_handle, IOProfile())
  { }

  TestMMCifParserProtected(std::istream& stream, mol::EntityHandle& ent_handle,
                           const IOProfile& profile):
    MMCifParser(stream, ent_handle, profile)
  { }

  TestMMCifParserProtected(const String& filename,
                           mol::EntityHandle& ent_handle):
    MMCifParser(filename, ent_handle, IOProfile())
  { }

  using MMCifParser::OnBeginLoop;
  using MMCifParser::IsValidPDBIdent;
  using MMCifParser::ParseAtomIdent;
  using MMCifParser::ParseAndAddAtom;
  using MMCifParser::ParseEntity;
  using MMCifParser::ParseEntityPoly;
  using MMCifParser::TryStoreIdx;
};

BOOST_AUTO_TEST_SUITE( io );

BOOST_AUTO_TEST_CASE(mmcif_isvalidpdbident)
{
  mol::EntityHandle eh=mol::CreateEntity();

  // on changing the tests for a PDB id in mmcif files, extend this unit test
  BOOST_MESSAGE("  Running mmcif_isvalidpdbident tests...");
  std::ifstream s("testfiles/mmcif/atom_site.mmcif");
  TestMMCifParserProtected tmmcif_p(s, eh);
  StringRef id = StringRef("1FOO", 4);
  BOOST_MESSAGE("    Testing valid id ('"+ id.str() +"')...");
  BOOST_CHECK(tmmcif_p.IsValidPDBIdent(id));
  BOOST_MESSAGE("    done.");
  id = StringRef("this is to long for a PDB id", 28);
  BOOST_MESSAGE("    Testing oversized PDB id ('"+ id.str() +"')...");
  BOOST_CHECK(!tmmcif_p.IsValidPDBIdent(id));
  BOOST_MESSAGE("    done.");
  id = StringRef("nFOO", 4);
  BOOST_MESSAGE("    Testing PDB id with missing number ('"+ id.str() +"')...");
  BOOST_CHECK(!tmmcif_p.IsValidPDBIdent(id));
  BOOST_MESSAGE("    done.");
}

BOOST_AUTO_TEST_CASE(mmcif_trystoreidx)
{
  mol::EntityHandle eh = mol::CreateEntity();

  BOOST_MESSAGE("  Running mmcif_trystoreidx tests...");
  std::ifstream s("testfiles/mmcif/atom_site.mmcif");
  TestMMCifParserProtected tmmcif_p(s, eh, IOProfile());
  StarLoopDesc mmcif_h;
  mmcif_h.SetCategory(StringRef("Foo", 3));
  // negative
  BOOST_CHECK_THROW(tmmcif_p.TryStoreIdx(0, "bar", mmcif_h), IOException);
  // positive
  mmcif_h.Add(StringRef("bar", 3));
  BOOST_CHECK_NO_THROW(tmmcif_p.TryStoreIdx(0, "bar", mmcif_h));
}

BOOST_AUTO_TEST_CASE(mmcif_onbeginloop)
{
  mol::EntityHandle eh=mol::CreateEntity();

  // add more tests on new mandatory items
  BOOST_MESSAGE("  Running mmcif_onbeginloop tests...");
  std::ifstream s("testfiles/mmcif/atom_site.mmcif");
  MMCifParser mmcif_p(s, eh, IOProfile());
  StarLoopDesc mmcif_h;
  BOOST_MESSAGE("          testing atom_site items...");
  mmcif_h.SetCategory(StringRef("atom_site", 9));
  BOOST_MESSAGE("             auth_asym_id");
  BOOST_CHECK_THROW(mmcif_p.OnBeginLoop(mmcif_h), IOException);
  mmcif_h.Add(StringRef("auth_asym_id", 12));
  BOOST_MESSAGE("             id");
  BOOST_CHECK_THROW(mmcif_p.OnBeginLoop(mmcif_h), IOException);
  mmcif_h.Add(StringRef("id", 2));
  BOOST_MESSAGE("             label_alt_id");
  BOOST_CHECK_THROW(mmcif_p.OnBeginLoop(mmcif_h), IOException);
  mmcif_h.Add(StringRef("label_alt_id", 12));
  BOOST_MESSAGE("             label_asym_id");
  BOOST_CHECK_THROW(mmcif_p.OnBeginLoop(mmcif_h), IOException);
  mmcif_h.Add(StringRef("label_asym_id", 13));
  BOOST_MESSAGE("             label_atom_id");
  BOOST_CHECK_THROW(mmcif_p.OnBeginLoop(mmcif_h), IOException);
  mmcif_h.Add(StringRef("label_atom_id", 13));
  BOOST_MESSAGE("             label_comp_id");
  BOOST_CHECK_THROW(mmcif_p.OnBeginLoop(mmcif_h), IOException);
  mmcif_h.Add(StringRef("label_comp_id", 13));
  BOOST_MESSAGE("             label_entity_id");
  BOOST_CHECK_THROW(mmcif_p.OnBeginLoop(mmcif_h), IOException);
  mmcif_h.Add(StringRef("label_entity_id", 15));
  BOOST_MESSAGE("             label_seq_id");
  BOOST_CHECK_THROW(mmcif_p.OnBeginLoop(mmcif_h), IOException);
  mmcif_h.Add(StringRef("label_seq_id", 12));
  BOOST_MESSAGE("             type_symbol");
  BOOST_CHECK_THROW(mmcif_p.OnBeginLoop(mmcif_h), IOException);
  mmcif_h.Add(StringRef("type_symbol", 11));
  BOOST_MESSAGE("             Cartn_x");
  BOOST_CHECK_THROW(mmcif_p.OnBeginLoop(mmcif_h), IOException);
  mmcif_h.Add(StringRef("Cartn_x", 7));
  BOOST_MESSAGE("             Cartn_y");
  BOOST_CHECK_THROW(mmcif_p.OnBeginLoop(mmcif_h), IOException);
  mmcif_h.Add(StringRef("Cartn_y", 7));
  BOOST_MESSAGE("             Cartn_z");
  BOOST_CHECK_THROW(mmcif_p.OnBeginLoop(mmcif_h), IOException);
  mmcif_h.Add(StringRef("Cartn_z", 7));
  BOOST_CHECK_NO_THROW(mmcif_p.OnBeginLoop(mmcif_h));
  BOOST_MESSAGE("          done.");
  mmcif_h.Clear();
  BOOST_MESSAGE("          testing entity items...");
  mmcif_h.SetCategory(StringRef("entity", 6));
  BOOST_MESSAGE("             id");
  BOOST_CHECK_THROW(mmcif_p.OnBeginLoop(mmcif_h), IOException);
  mmcif_h.Add(StringRef("id", 2));
  BOOST_CHECK_NO_THROW(mmcif_p.OnBeginLoop(mmcif_h));
  BOOST_MESSAGE("          done.");
  BOOST_MESSAGE("  done.");
}

BOOST_AUTO_TEST_CASE(mmcif_parse_models)
{
  BOOST_MESSAGE("  Running mmcif_parse_models tests...");
  IOProfile profile;

  // positive w models
  BOOST_MESSAGE("          true positive test for models...");
  {
    mol::EntityHandle eh = mol::CreateEntity();
    MMCifParser mmcif_p("testfiles/mmcif/model_truepos.mmcif", eh, profile);
    BOOST_CHECK_NO_THROW(mmcif_p.Parse());
    BOOST_REQUIRE_EQUAL(eh.GetChainCount(),   2);
    BOOST_REQUIRE_EQUAL(eh.GetResidueCount(), 2);
    BOOST_REQUIRE_EQUAL(eh.GetAtomCount(),   26);
  }
  BOOST_MESSAGE("          done.");

  // positive wo models atom_site.mmcif
  BOOST_MESSAGE("          test absent atom_site.pdbx_PDB_model_num entry...");
  {
    mol::EntityHandle eh = mol::CreateEntity();
    MMCifParser mmcif_p("testfiles/mmcif/atom_site.mmcif", eh, profile);
    BOOST_CHECK_NO_THROW(mmcif_p.Parse());
  }
  BOOST_MESSAGE("          done.");
  // negative, more than 1 atom_site category
  BOOST_MESSAGE("          testing more than one atom_site block...");
  {
    mol::EntityHandle eh = mol::CreateEntity();
    MMCifParser mmcif_p("testfiles/mmcif/model_multi_atom_site.mmcif", eh,
                        profile);
    BOOST_CHECK_THROW(mmcif_p.Parse(), IOException);
  }
  {
    mol::EntityHandle eh = mol::CreateEntity();
    MMCifParser mmcif_p("testfiles/mmcif/model_multi_atom_site_inverted.mmcif",
                        eh, profile);
    BOOST_CHECK_THROW(mmcif_p.Parse(), IOException);
  }
  BOOST_MESSAGE("          done.");

  BOOST_MESSAGE("  done.");
}

BOOST_AUTO_TEST_CASE(mmcif_changing_label_entity_id)
{
  BOOST_MESSAGE("  Running mmcif_changing_label_entity_id tests...");
  IOProfile profile;

  // positive
  BOOST_MESSAGE("          true positive test...");
  {
    mol::EntityHandle eh = mol::CreateEntity();
    MMCifParser mmcif_p("testfiles/mmcif/atom_site.mmcif", eh, profile);
    BOOST_CHECK_NO_THROW(mmcif_p.Parse());
  }
  BOOST_MESSAGE("          done.");

  // negative
  BOOST_MESSAGE("          true negative test...");
  {
    mol::EntityHandle eh = mol::CreateEntity();
    MMCifParser mmcif_p("testfiles/mmcif/changing_label_entity_id.mmcif", eh,
                        profile);
    BOOST_CHECK_THROW(mmcif_p.Parse(), IOException);
  }
  BOOST_MESSAGE("          done.");

  BOOST_MESSAGE("  done.");
}

BOOST_AUTO_TEST_CASE(mmcif_unknown_entity_type)
{
  BOOST_MESSAGE("  Running mmcif_unknown_entity_type tests...");

  mol::EntityHandle eh = mol::CreateEntity();
  std::vector<StringRef> columns;
  TestMMCifParserProtected tmmcif_p("testfiles/mmcif/atom_site.mmcif", eh);
  StarLoopDesc tmmcif_h;

  // build dummy header
  tmmcif_h.SetCategory(StringRef("entity", 6));
  tmmcif_h.Add(StringRef("id", 2));
  tmmcif_h.Add(StringRef("type", 4));
  tmmcif_p.OnBeginLoop(tmmcif_h);

  // positive
  BOOST_MESSAGE("          known type...");
  // build datarow
  columns.push_back(StringRef("1", 1));
  columns.push_back(StringRef("polymer", 7));
  BOOST_CHECK_NO_THROW(tmmcif_p.ParseEntity(columns));
  columns.pop_back();
  columns.push_back(StringRef("non-polymer", 11));
  BOOST_CHECK_NO_THROW(tmmcif_p.ParseEntity(columns));
  columns.pop_back();
  columns.push_back(StringRef("water", 5));
  BOOST_CHECK_NO_THROW(tmmcif_p.ParseEntity(columns));
  BOOST_MESSAGE("          done.");

  // negative
  BOOST_MESSAGE("          unknown type...");
  columns.pop_back();
  columns.push_back(StringRef("foo", 3));
  BOOST_CHECK_THROW(tmmcif_p.ParseEntity(columns), IOException);
  BOOST_MESSAGE("          done.");

  BOOST_MESSAGE("  done.");
}

BOOST_AUTO_TEST_CASE(mmcif_entity_tests)
{
  BOOST_MESSAGE("  Running mmcif_entity_tests...");
  mol::ChainHandle ch;
  IOProfile profile;

  // positive
  BOOST_MESSAGE("          fetching chain type...");
  {
    mol::EntityHandle eh = mol::CreateEntity();
    MMCifParser mmcif_p("testfiles/mmcif/atom_site.mmcif", eh, profile);
    mmcif_p.Parse();
    ch = eh.FindChain("A");
    BOOST_CHECK(ch.IsValid());
    BOOST_CHECK(ch.GetType() == CHAINTYPE_POLY_PEPTIDE_L);
    ch = eh.FindChain("C");
    BOOST_CHECK(ch.IsValid());
    BOOST_CHECK(ch.GetType() == CHAINTYPE_POLY_PEPTIDE_L);
    ch = eh.FindChain("O");
    BOOST_CHECK(ch.IsValid());
    BOOST_CHECK(ch.GetType() == CHAINTYPE_WATER);
  }
  BOOST_MESSAGE("          done.");
  // negative: no entity description
  BOOST_MESSAGE("          check missing entity description...");
  {
    mol::EntityHandle eh = mol::CreateEntity();
    MMCifParser mmcif_p("testfiles/mmcif/model_truepos.mmcif",
                        eh,
                        profile);
    mmcif_p.Parse();
    ch = eh.FindChain("A");
    BOOST_CHECK(ch.IsValid());
    BOOST_CHECK(ch.GetType() == CHAINTYPE_UNKNOWN);
    ch = eh.FindChain("B");
    BOOST_CHECK(ch.IsValid());
    BOOST_CHECK(ch.GetType() == CHAINTYPE_UNKNOWN);
  }
  BOOST_MESSAGE("          done.");
  BOOST_MESSAGE("          fetch details...");
  {
    mol::EntityHandle eh = mol::CreateEntity();
    MMCifParser mmcif_p("testfiles/mmcif/atom_site.mmcif", eh, profile);
    mmcif_p.Parse();
    ch = eh.FindChain("A");
    BOOST_CHECK(ch.IsValid());
    BOOST_CHECK(ch.GetDescription() == "Very important information.");
  }
  BOOST_MESSAGE("          done.");

  BOOST_MESSAGE("  done.");
}

BOOST_AUTO_TEST_CASE(mmcif_entity_poly_tests)
{
  BOOST_MESSAGE("  Running mmcif_entity_poly_tests...");
  mol::ChainHandle ch;
  IOProfile profile;
  StarLoopDesc tmmcif_h;
  // positive
  // negative: unknown polymer type
  mol::EntityHandle eh = mol::CreateEntity();
  MMCifParser mmcif_p("testfiles/mmcif/atom_site.mmcif", eh, profile);

  mmcif_p.Parse();


  BOOST_MESSAGE("          testing missing corresponding entity entry...");
  {
    mol::EntityHandle eh = mol::CreateEntity();
    std::vector<StringRef> columns;
    TestMMCifParserProtected tmmcif_p("testfiles/mmcif/atom_site.mmcif", eh);

    tmmcif_h.SetCategory(StringRef("entity_poly", 11));
    tmmcif_h.Add(StringRef("entity_id", 9));
    tmmcif_p.OnBeginLoop(tmmcif_h);

    columns.push_back(StringRef("1", 1));
    BOOST_CHECK_THROW(tmmcif_p.ParseEntityPoly(columns), IOException);
  }
  BOOST_MESSAGE("          done.");
  BOOST_MESSAGE("          testing type recognition...");
  {
    TestMMCifParserProtected tmmcif_p("testfiles/mmcif/atom_site.mmcif", eh);
    std::vector<StringRef> columns;

    // create corresponding entity entry
    tmmcif_h.Clear();
    tmmcif_h.SetCategory(StringRef("entity", 6));
    tmmcif_h.Add(StringRef("id", 2));
    tmmcif_h.Add(StringRef("type", 4));
    tmmcif_p.OnBeginLoop(tmmcif_h);
    columns.push_back(StringRef("1", 1));
    columns.push_back(StringRef("polymer", 7));
    tmmcif_p.ParseEntity(columns);
    columns.pop_back();
    columns.pop_back();

    // build dummy entity_poly header
    tmmcif_h.Clear();
    tmmcif_h.SetCategory(StringRef("entity_poly", 11));
    tmmcif_h.Add(StringRef("entity_id", 9));
    tmmcif_h.Add(StringRef("type", 4));
    tmmcif_p.OnBeginLoop(tmmcif_h);

    columns.push_back(StringRef("1", 1));
    columns.push_back(StringRef("polypeptide(D)", 14));
    BOOST_CHECK_NO_THROW(tmmcif_p.ParseEntityPoly(columns));
    columns.pop_back();
    columns.push_back(StringRef("polypeptide(L)", 14));
    BOOST_CHECK_NO_THROW(tmmcif_p.ParseEntityPoly(columns));
    columns.pop_back();
    columns.push_back(StringRef("polydeoxyribonucleotide", 23));
    BOOST_CHECK_NO_THROW(tmmcif_p.ParseEntityPoly(columns));
    columns.pop_back();
    columns.push_back(StringRef("polyribonucleotide", 18));
    BOOST_CHECK_NO_THROW(tmmcif_p.ParseEntityPoly(columns));
    columns.pop_back();
    columns.push_back(StringRef("polysaccharide(D)", 17));
    BOOST_CHECK_NO_THROW(tmmcif_p.ParseEntityPoly(columns));
    columns.pop_back();
    columns.push_back(StringRef("polysaccharide(L)", 17));
    BOOST_CHECK_NO_THROW(tmmcif_p.ParseEntityPoly(columns));
    columns.pop_back();
columns.push_back(StringRef("polydeoxyribonucleotide/polyribonucleotide hybrid",
                                49));
    BOOST_CHECK_NO_THROW(tmmcif_p.ParseEntityPoly(columns));
    columns.pop_back();
    columns.push_back(StringRef("other", 5));
    BOOST_CHECK_NO_THROW(tmmcif_p.ParseEntityPoly(columns));
    columns.pop_back();
    columns.pop_back();
    columns.push_back(StringRef("badbadprion", 11));
    BOOST_CHECK_THROW(tmmcif_p.ParseEntityPoly(columns), IOException);
    columns.pop_back();
  }
  BOOST_MESSAGE("          done.");

  BOOST_MESSAGE("  done.");
}

BOOST_AUTO_TEST_CASE(mmcif_parseatomident)
{
  BOOST_MESSAGE("  Running mmcif_parseatomident tests...");

  mol::EntityHandle eh = mol::CreateEntity();

  std::ifstream s("testfiles/mmcif/atom_site.mmcif");
  IOProfile profile;
  TestMMCifParserProtected tmmcif_p(s, eh, profile);
  std::vector<StringRef> columns;
  String chain_name;
  StringRef res_name;
  mol::ResNum resnum(0);
  //StringRef atom_name;
  //char alt_loc;

  BOOST_MESSAGE("          testing valid line");
  //tmmcif_p.ParseAtomIdent(columns, chain_name, res_name);
  BOOST_MESSAGE("          done.");
  // negative
  //cols.push_back(StringRef("ATOM", 4));
  //BOOST_CHECK_THROW(tmmcif_p.ParseAtomIdent(cols,
  //                                          chain_name,
  //                                          res_name,
  //                                          resnum,
  //                                          atom_name,
  //                                         alt_loc), IOException);
  // positive
  //StarLoopDesc tmmcif_h;
  //tmmcif_h.SetCategory(StringRef("atom_site", 9));
  // build header
  //mmcif_h.Add(StringRef("AUTH_ASYM_ID", 12));
  /*
    this->TryStoreIdx(AUTH_ASYM_ID,    "auth_asym_id",    header);
    this->TryStoreIdx(ID,              "id",              header);
    this->TryStoreIdx(LABEL_ALT_ID,    "label_alt_id",    header);
    this->TryStoreIdx(LABEL_ASYM_ID,   "label_asym_id",   header);
    this->TryStoreIdx(LABEL_ATOM_ID,   "label_atom_id",   header);
    this->TryStoreIdx(LABEL_COMP_ID,   "label_comp_id",   header);
    this->TryStoreIdx(LABEL_ENTITY_ID, "label_entity_id", header);
    this->TryStoreIdx(LABEL_SEQ_ID,    "label_seq_id",    header);
    this->TryStoreIdx(TYPE_SYMBOL,     "type_symbol",     header);
    this->TryStoreIdx(CARTN_X, "Cartn_x", header);
    this->TryStoreIdx(CARTN_Y, "Cartn_y", header);
    this->TryStoreIdx(CARTN_Z, "Cartn_z", header);
*/

  BOOST_MESSAGE("  done.");
}

BOOST_AUTO_TEST_CASE(mmcif_parseandaddatom)
{
  mol::EntityHandle eh = mol::CreateEntity();

  BOOST_MESSAGE("  Running mmcif_parseandaddatom tests...");
  std::ifstream s("testfiles/mmcif/atom_site.mmcif");
  TestMMCifParserProtected tmmcif_p(s, eh, IOProfile());
  std::vector<StringRef> cols;

  //BOOST_MESSAGE("    testing short atom_site entry");
  //cols.push_back(StringRef("ATOM", 4));
  //BOOST_CHECK_THROW(tmmcif_p.ParseAndAddAtom(cols), IOException);
  //BOOST_MESSAGE("  done.");
}

BOOST_AUTO_TEST_CASE(mmcif_testreader)
{
  BOOST_MESSAGE("  Running mmcif_testreader tests...");
  mol::EntityHandle eh = mol::CreateEntity();
  std::ifstream s("testfiles/mmcif/atom_site.mmcif");
  IOProfile profile;
  MMCifParser mmcif_p(s, eh, profile);

  BOOST_MESSAGE("          testing Parse()...");
  BOOST_CHECK_NO_THROW(mmcif_p.Parse());

  BOOST_REQUIRE_EQUAL(eh.GetChainCount(),    3);
  BOOST_REQUIRE_EQUAL(eh.GetResidueCount(), 14);
  BOOST_REQUIRE_EQUAL(eh.GetAtomCount(),    35);

  mol::ChainHandle ch = eh.FindChain("A");
  BOOST_CHECK(ch.IsValid());
  BOOST_MESSAGE("          done.");

  BOOST_MESSAGE("          testing numbering water...");
  ch = eh.FindChain("O");
  BOOST_CHECK(ch.IsValid());
  mol::ResidueHandleList rl = ch.GetResidueList();
  mol::ResidueHandleList::const_iterator rs;
  int i = 1;
  for (rs = rl.begin(); rs != rl.end(); ++rs, ++i) {
    BOOST_CHECK_EQUAL(rs->GetNumber().GetNum(), i);
  }

  BOOST_MESSAGE("          done.");

  BOOST_MESSAGE("  done.");
}

BOOST_AUTO_TEST_SUITE_END();
