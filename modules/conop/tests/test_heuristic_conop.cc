// ------------------------------------------------------------------------------
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
// ------------------------------------------------------------------------------

#include <ost/log.hh>
#include <ost/mol/mol.hh>
#include <ost/conop/heuristic.hh>

#define BOOST_TEST_DYN_LINK
#include <boost/test/unit_test.hpp>
#include <boost/test/auto_unit_test.hpp>
#include "helper.hh"
using boost::unit_test_framework::test_suite;
using namespace ost;
using namespace ost::conop;
using namespace ost::mol;



void verify_connectivity_x(const ResidueHandle& res) 
{
  BOOST_CHECK(BondExists(res.FindAtom("XN"),
                         res.FindAtom("XCA")));
  BOOST_CHECK(BondExists(res.FindAtom("XCA"),
                         res.FindAtom("XC")));
  BOOST_CHECK(BondExists(res.FindAtom("XC"),
                         res.FindAtom("XO")));
  BOOST_CHECK(BondExists(res.FindAtom("XCA"),
                         res.FindAtom("XCB")));
  BOOST_CHECK(BondExists(res.FindAtom("XCB"),
                         res.FindAtom("XCG")));
  if (res.GetKey()=="ARG") {

    BOOST_CHECK(BondExists(res.FindAtom("XCB"),
                           res.FindAtom("XCG")));
    BOOST_CHECK(BondExists(res.FindAtom("XCG"),
                           res.FindAtom("XCD")));
    BOOST_CHECK(BondExists(res.FindAtom("XCD"),
                           res.FindAtom("XNE")));
    BOOST_CHECK(BondExists(res.FindAtom("XNE"),
                           res.FindAtom("XCZ")));
    BOOST_CHECK(BondExists(res.FindAtom("XCZ"),
                           res.FindAtom("XNH1")));
    BOOST_CHECK(BondExists(res.FindAtom("XCZ"),
                           res.FindAtom("XNH2")));
    // TODO: Check that no other atoms are connected!
  }
  if (res.GetKey()=="ILE") {
    BOOST_CHECK(BondExists(res.FindAtom("XCG"),
                           res.FindAtom("XCD1")));
    BOOST_CHECK(BondExists(res.FindAtom("XCG"),
                           res.FindAtom("XCD2")));
    // TODO: Check that no other atoms are connected!
  }
}

void verify_connectivity(const ResidueHandle& res) 
{
  BOOST_CHECK(BondExists(res.FindAtom("N"),
                        res.FindAtom("CA")));
  BOOST_CHECK(BondExists(res.FindAtom("CA"),
                         res.FindAtom("C")));
  BOOST_CHECK(BondExists(res.FindAtom("C"),
                         res.FindAtom("O")));
  BOOST_CHECK(BondExists(res.FindAtom("CA"),
                         res.FindAtom("CB")));
  BOOST_CHECK(BondExists(res.FindAtom("CB"),
                         res.FindAtom("CG")));
  if (res.GetKey()=="ARG") {

    BOOST_CHECK(BondExists(res.FindAtom("CB"),
                           res.FindAtom("CG")));
    BOOST_CHECK(BondExists(res.FindAtom("CG"),
                           res.FindAtom("CD")));
    BOOST_CHECK(BondExists(res.FindAtom("CD"),
                           res.FindAtom("NE")));
    BOOST_CHECK(BondExists(res.FindAtom("NE"),
                           res.FindAtom("CZ")));
    BOOST_CHECK(BondExists(res.FindAtom("CZ"),
                           res.FindAtom("NH1")));
    BOOST_CHECK(BondExists(res.FindAtom("CZ"),
                           res.FindAtom("NH2")));
    // TODO: Check that no other atoms are connected!
  }
  if (res.GetKey()=="ILE") {
    BOOST_CHECK(BondExists(res.FindAtom("CG"),
                           res.FindAtom("CD1")));
    BOOST_CHECK(BondExists(res.FindAtom("CG"),
                           res.FindAtom("CD2")));
    // TODO: Check that no other atoms are connected!
  }
}

BOOST_AUTO_TEST_SUITE( conop );


BOOST_AUTO_TEST_CASE(does_name_based_connect) 
{
  EntityHandle e=CreateEntity();
  ChainHandle c=e.EditXCS().InsertChain("A");
  ResidueHandle ile=make_leu(c);
  ResidueHandle arg=make_arg(c);
  HeuristicProcessor proc;
  proc.Process(e);

  verify_connectivity(arg);
  verify_connectivity(ile);
}

BOOST_AUTO_TEST_CASE(does_assign_torsions) {
  EntityHandle e=CreateEntity();
  ChainHandle c=e.EditXCS().InsertChain("A");
  ResidueHandle l1=make_leu(c);
  ResidueHandle a2=make_arg(c);
  ResidueHandle l3=make_leu(c);
  e.EditXCS().Connect(l1.FindAtom("C"), a2.FindAtom("N"));
  e.EditXCS().Connect(a2.FindAtom("C"), l3.FindAtom("N"));
  HeuristicProcessor proc;
  proc.SetAssignTorsions(true);
  proc.Process(e);
  BOOST_CHECK(a2.GetPhiTorsion().IsValid());
  BOOST_CHECK(l3.GetPhiTorsion().IsValid());

  BOOST_CHECK(l1.GetPsiTorsion().IsValid());
  BOOST_CHECK(a2.GetPsiTorsion().IsValid());
}

BOOST_AUTO_TEST_SUITE_END();

