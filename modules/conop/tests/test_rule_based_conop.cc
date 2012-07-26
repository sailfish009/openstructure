
// -----------------------------------------------------------------------------
// This file is part of the OpenStructure project <www.openstructure.org>

// Copyright (C) 2008-2011 by the OpenStructure authors

// This library is free software; you can redistribute it and/or modify it under
// the terms of the GNU Lesser General Public License as published by the Free
// Software Foundation; either version 3.0 of the License, or (at your option)
// any later version.
// This library is distributed in the hope that it will be useful, but WITHOUT
// ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
// FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public License for more
// details.

// You should have received a copy of the GNU Lesser General Public License
// along with this library; if not, write to the Free Software Foundation, Inc.,
// 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
// -----------------------------------------------------------------------------

#define BOOST_TEST_DYN_LINK
#include <boost/test/unit_test.hpp>
#include <boost/test/auto_unit_test.hpp>

#include <ost/mol/mol.hh>
#include <ost/platform.hh>
#include <ost/log.hh>
#include <ost/conop/rule_based.hh>
#include <ost/conop/conop.hh>
#include "helper.hh"

using boost::unit_test_framework::test_suite;
using namespace ost;
using namespace ost::conop;
//using namespace ost::io;
using namespace ost::mol;

CompoundLibPtr load_lib()
{
  if (!getenv("OST_ROOT")) {
    LOG_ERROR("OST_ROOT environment variable not set. Can't load "
              "compound library without a proper OST_ROOT");
    return CompoundLibPtr();
  }
  SetPrefixPath(getenv("OST_ROOT"));
  String lib_path=GetSharedDataPath()+"/compounds.chemlib";
  CompoundLibPtr compound_lib=CompoundLib::Load(lib_path);  
  return compound_lib;
}

BOOST_AUTO_TEST_SUITE(conop);

BOOST_AUTO_TEST_CASE(rule_based_set_get_flags)
{
   CompoundLibPtr lib=load_lib(); 
   if (!lib) { return; }
   RuleBasedProcessor rbc(lib);
   // check the defaults
   BOOST_CHECK_EQUAL(rbc.GetConnect(), true);
   BOOST_CHECK_EQUAL(rbc.GetCheckBondFeasibility(), false);
   BOOST_CHECK_EQUAL(rbc.GetUnkAtomTreatment(), CONOP_WARN);
   BOOST_CHECK_EQUAL(rbc.GetUnkResidueTreatment(), CONOP_WARN);
   BOOST_CHECK_EQUAL(rbc.GetStrictHydrogens(), false);
   rbc.PushFlags();
   rbc.SetConnect(false);
   rbc.SetCheckBondFeasibility(true);
   rbc.SetUnkResidueTreatment(CONOP_FATAL);
   rbc.SetUnkAtomTreatment(CONOP_REMOVE);
   BOOST_CHECK_EQUAL(rbc.GetConnect(), false);
   BOOST_CHECK_EQUAL(rbc.GetCheckBondFeasibility(),true);
   BOOST_CHECK_EQUAL(rbc.GetUnkResidueTreatment(),CONOP_FATAL);
   BOOST_CHECK_EQUAL(rbc.GetUnkAtomTreatment(),CONOP_REMOVE);
   rbc.PopFlags();
}

BOOST_AUTO_TEST_CASE(rule_based_push_pop_flags)
{
  CompoundLibPtr lib=load_lib();
  if (!lib) { return; }
  RuleBasedProcessor rbc(lib);
  BOOST_CHECK_THROW(rbc.PopFlags(), std::runtime_error);
  rbc.PushFlags();
  rbc.PopFlags();
  rbc.PushFlags();
  rbc.SetConnect(false);
  BOOST_CHECK(!rbc.GetConnect());
  rbc.PopFlags();
  BOOST_CHECK(rbc.GetConnect());
}

BOOST_AUTO_TEST_CASE(rule_based_connect)
{
  CompoundLibPtr lib=load_lib();
  if (!lib) { return; }
  RuleBasedProcessor rbc(lib);
  EntityHandle ent = CreateEntity();
  ChainHandle ch=ent.EditXCS().InsertChain("A");
  ResidueHandle r = make_cytosine(ch);
  rbc.SetConnect(false); 
  rbc.Process(ent);
  BOOST_CHECK_EQUAL(static_cast<size_t>(0),
                    ent.GetBondList().size());
  rbc.SetConnect(true); 
  rbc.Process(ent);
  verify_nucleotide_connectivity(r);
}


BOOST_AUTO_TEST_CASE(rule_based_unk_atoms)
{
  CompoundLibPtr lib = load_lib(); 
  if (!lib) { return; }
  RuleBasedProcessor rbc(lib);
  EntityHandle ent = CreateEntity();
  XCSEditor edi=ent.EditXCS();
  ChainHandle ch = edi.InsertChain("A");
  ResidueHandle gly = edi.AppendResidue(ch, "GLY");
  edi.InsertAtom(gly, "N", geom::Vec3());
  edi.InsertAtom(gly, "CA", geom::Vec3());
  edi.InsertAtom(gly, "C", geom::Vec3());
  edi.InsertAtom(gly, "O", geom::Vec3());
  edi.InsertAtom(gly, "CB", geom::Vec3());
  EntityHandle ent2 = ent.Copy();
  rbc.SetUnkAtomTreatment(CONOP_SILENT);
  DiagnosticsPtr diags=rbc.Process(ent2);
  BOOST_CHECK_EQUAL(diags->diag_count(), static_cast<size_t>(0));
  rbc.SetUnkAtomTreatment(CONOP_WARN);
  diags = rbc.Process(ent2);
  BOOST_CHECK_EQUAL(diags->diag_count(), static_cast<size_t>(1));
  ent2 = ent.Copy();
  rbc.SetUnkAtomTreatment(CONOP_REMOVE);
  rbc.Process(ent2);
  BOOST_CHECK(!ent2.FindAtom("A", 1, "CB"));
  ent2 = ent.Copy();
  rbc.SetUnkAtomTreatment(CONOP_REMOVE_RESIDUE);
  rbc.Process(ent2);
  BOOST_CHECK_EQUAL(ent2.GetResidueCount(), 0);
}

BOOST_AUTO_TEST_CASE(rule_based_unk_res)
{
  CompoundLibPtr lib = load_lib(); 
  if (!lib) { return; }
  RuleBasedProcessor rbc(lib);
  EntityHandle ent = CreateEntity();
  XCSEditor edi=ent.EditXCS();
  ChainHandle ch = edi.InsertChain("A");
  ResidueHandle gly = edi.AppendResidue(ch, "???");
  EntityHandle ent2 = ent.Copy();
  rbc.SetUnkResidueTreatment(CONOP_SILENT); 
  DiagnosticsPtr diags=rbc.Process(ent2);
  BOOST_CHECK_EQUAL(diags->diag_count(), static_cast<size_t>(0));
  rbc.SetUnkResidueTreatment(CONOP_WARN);
  diags = rbc.Process(ent2);
  BOOST_CHECK_EQUAL(diags->diag_count(), static_cast<size_t>(1));
  ent2 = ent.Copy();
  rbc.SetUnkResidueTreatment(CONOP_REMOVE);
  rbc.Process(ent2);
  BOOST_CHECK(!ent2.FindResidue("A", 1));
}

BOOST_AUTO_TEST_SUITE_END();
