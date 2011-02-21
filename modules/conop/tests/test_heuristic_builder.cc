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

#include <ost/mol/mol.hh>
#include <ost/conop/heuristic_builder.hh>

#define BOOST_TEST_DYN_LINK
#include <boost/test/unit_test.hpp>

#include <ost/log.hh>
using boost::unit_test_framework::test_suite;
using namespace ost;
using namespace ost::conop;
using namespace ost::mol;
namespace {
} // anon ns

ResidueHandle make_arg(ChainHandle chain) 
{
  XCSEditor e=chain.GetEntity().EditXCS();
  ResidueHandle res = e.AppendResidue(chain, "ARG");
  e.InsertAtom(res, "N",geom::Vec3(20.202,33.112,58.011));
  e.InsertAtom(res, "CA",geom::Vec3(19.396,31.903,58.033));
  e.InsertAtom(res, "C",geom::Vec3(18.608,31.739,59.328));
  e.InsertAtom(res, "O",geom::Vec3(17.651,30.965,59.381));
  e.InsertAtom(res, "CB",geom::Vec3(20.284,30.681,57.801));
  e.InsertAtom(res, "CG",geom::Vec3(20.665,30.488,56.342));
  e.InsertAtom(res, "CD",geom::Vec3(21.557,29.281,56.154));
  e.InsertAtom(res, "NE",geom::Vec3(22.931,29.557,56.551));
  e.InsertAtom(res, "CZ",geom::Vec3(23.901,28.653,56.528));
  e.InsertAtom(res, "NH1",geom::Vec3(23.640,27.417,56.130));
  e.InsertAtom(res, "NH2",geom::Vec3(25.132,28.980,56.893));
  return res;
}

ResidueHandle make_leu(ChainHandle chain) 
{
  XCSEditor e=chain.GetEntity().EditXCS();  
  ResidueHandle res=e.AppendResidue(chain, "LEU");

  e.InsertAtom(res, "N", geom::Vec3(19.003,32.473,60.366));
  e.InsertAtom(res, "CA", geom::Vec3(18.330,32.402,61.664));
  e.InsertAtom(res, "C", geom::Vec3(17.884,33.787,62.117));
  e.InsertAtom(res, "O", geom::Vec3(17.853,34.091,63.308));
  e.InsertAtom(res, "CB", geom::Vec3(19.269,31.793,62.710));
  e.InsertAtom(res, "CG", geom::Vec3(19.695,30.340,62.501));
  e.InsertAtom(res, "CD1", geom::Vec3(20.585,29.897,63.648));
  e.InsertAtom(res, "CD2", geom::Vec3(18.461,29.459,62.420));
  return res;
}


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

BOOST_AUTO_TEST_SUITE( conop )


BOOST_AUTO_TEST_CASE(name_based_connect) 
{
  EntityHandle e=CreateEntity();
  ChainHandle c=e.EditXCS().InsertChain("A");
  ResidueHandle ile=make_leu(c);
  ResidueHandle arg=make_arg(c);
  HeuristicBuilder heuristic_builder;  
  for (AtomHandleIter i=e.AtomsBegin(),x=e.AtomsEnd(); i!=x; ++i) {
    heuristic_builder.FillAtomProps(*i);
  }

  BOOST_MESSAGE("running distance based checks on arginine");
  heuristic_builder.ConnectAtomsOfResidue(arg);
  verify_connectivity(arg);
  BOOST_MESSAGE("running distance based checks on leu");
  heuristic_builder.ConnectAtomsOfResidue(ile);
  verify_connectivity(ile);
}

BOOST_AUTO_TEST_CASE(test_assign_torsions){
  EntityHandle e=CreateEntity();
  ChainHandle c=e.EditXCS().InsertChain("A");
  ResidueHandle l1=make_leu(c);
  ResidueHandle a2=make_arg(c);
  ResidueHandle l3=make_leu(c);
  l1.SetChemClass(ChemClass(ChemClass::LPeptideLinking));
  a2.SetChemClass(ChemClass(ChemClass::LPeptideLinking));  
  l3.SetChemClass(ChemClass(ChemClass::LPeptideLinking));
  HeuristicBuilder heuristic_builder;
  for (AtomHandleIter i=e.AtomsBegin(),x=e.AtomsEnd(); i!=x; ++i) {
    heuristic_builder.FillAtomProps(*i);
  }
  heuristic_builder.ConnectAtomsOfResidue(l1);
  heuristic_builder.ConnectAtomsOfResidue(a2);
  heuristic_builder.ConnectAtomsOfResidue(l3);
  XCSEditor edi=e.EditXCS();
  edi.Connect(l1.FindAtom("C"), a2.FindAtom("N"));
  edi.Connect(a2.FindAtom("C"), l3.FindAtom("N"));  
  heuristic_builder.AssignTorsions(c);

  BOOST_CHECK(a2.GetPhiTorsion().IsValid());
  BOOST_CHECK(l3.GetPhiTorsion().IsValid());

  BOOST_CHECK(l1.GetPsiTorsion().IsValid());
  BOOST_CHECK(a2.GetPsiTorsion().IsValid());
}

BOOST_AUTO_TEST_SUITE_END( )