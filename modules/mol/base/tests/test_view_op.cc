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
/*
  Authors: Marco Biasini, Juergen Haas
 */
 #define BOOST_TEST_DYN_LINK
#include <boost/test/unit_test.hpp>
#include <ost/mol/mol.hh>
#include <ost/mol/view_op.hh>

using namespace ost;
using namespace ost::mol;

EntityHandle mk_test_ent()
{
  EntityHandle ent=CreateEntity();
  XCSEditor edi=ent.RequestXCSEditor();
  ChainHandle chain_a=edi.InsertChain("A");
  ResidueHandle res_a=edi.AppendResidue(chain_a, "A");
  AtomHandle atom_a=edi.InsertAtom(res_a, "A", geom::Vec3());
  AtomHandle atom_b=edi.InsertAtom(res_a, "B", geom::Vec3());  
  ResidueHandle res_b=edi.AppendResidue(chain_a, "B");  
  AtomHandle atom_c=edi.InsertAtom(res_b, "C", geom::Vec3());
  AtomHandle atom_d=edi.InsertAtom(res_b, "D", geom::Vec3());
  
  ChainHandle chain_b=edi.InsertChain("B");  
  ResidueHandle res_c=edi.AppendResidue(chain_b, "C");
  AtomHandle atom_e=edi.InsertAtom(res_c, "E", geom::Vec3());
  AtomHandle atom_f=edi.InsertAtom(res_c, "F", geom::Vec3());  
  ResidueHandle res_d=edi.AppendResidue(chain_b, "D");
  AtomHandle atom_g=edi.InsertAtom(res_d, "G", geom::Vec3());
  AtomHandle atom_h=edi.InsertAtom(res_d, "H", geom::Vec3());  
  edi.Connect(atom_a, atom_b);
  edi.Connect(atom_b, atom_c);
  edi.Connect(atom_c, atom_d);
  edi.Connect(atom_d, atom_e);
  edi.Connect(atom_e, atom_f);
  edi.Connect(atom_f, atom_g);
  edi.Connect(atom_g, atom_h);
  return ent;
}

bool find_bond(AtomHandle a, AtomHandle b, const BondHandleList& bonds)
{
  return std::find(bonds.begin(), bonds.end(), a.FindBondToAtom(b))!=bonds.end();
}

BOOST_AUTO_TEST_SUITE( mol_base )

BOOST_AUTO_TEST_CASE(test_difference)
{

  EntityHandle ent=mk_test_ent();
  EntityView full=ent.CreateFullView();
  EntityView v1=ent.Select("aname=A,B,C");
  EntityView diff_view=mol::Difference(full, v1);

  BOOST_CHECK_EQUAL(diff_view.GetBondCount(), 5);
  BOOST_CHECK_EQUAL(diff_view.GetAtomCount(), 5);
  BOOST_CHECK_EQUAL(diff_view.GetResidueCount(), 3);
  BOOST_CHECK_EQUAL(diff_view.GetChainCount(), 2);
  // check chains
  BOOST_CHECK(diff_view.FindChain("A"));
  BOOST_CHECK(diff_view.FindChain("B"));

  // check residues
  BOOST_CHECK(!diff_view.FindResidue(ent.FindResidue("A", mol::ResNum(1))));
  BOOST_CHECK(diff_view.FindResidue(ent.FindResidue("A", mol::ResNum(2))));
  BOOST_CHECK(diff_view.FindResidue(ent.FindResidue("B", mol::ResNum(1))));
  BOOST_CHECK(diff_view.FindResidue(ent.FindResidue("B", mol::ResNum(2))));

  // check atoms
  BOOST_CHECK(!diff_view.FindAtom(ent.FindAtom("A", mol::ResNum(1), "A")));
  BOOST_CHECK(!diff_view.FindAtom(ent.FindAtom("A", mol::ResNum(1), "B")));
  BOOST_CHECK(!diff_view.FindAtom(ent.FindAtom("A", mol::ResNum(2), "C")));
  BOOST_CHECK(diff_view.FindAtom(ent.FindAtom("A", mol::ResNum(2), "D")));
  BOOST_CHECK(diff_view.FindAtom(ent.FindAtom("B", mol::ResNum(1), "E")));
  BOOST_CHECK(diff_view.FindAtom(ent.FindAtom("B", mol::ResNum(1), "F")));
  BOOST_CHECK(diff_view.FindAtom(ent.FindAtom("B", mol::ResNum(2), "G")));
  BOOST_CHECK(diff_view.FindAtom(ent.FindAtom("B", mol::ResNum(2), "H")));

  // check bonds
  BondHandleList bonds=diff_view.GetBondList();
  BOOST_CHECK(!find_bond(ent.FindAtom("A", mol::ResNum(1), "A"),
                         ent.FindAtom("A", mol::ResNum(1), "B"),
                         bonds));
  BOOST_CHECK(!find_bond(ent.FindAtom("A", mol::ResNum(1), "B"),
                         ent.FindAtom("A", mol::ResNum(2), "C"),
                         bonds));
  BOOST_CHECK(find_bond(ent.FindAtom("A", mol::ResNum(2), "C"),
                         ent.FindAtom("A", mol::ResNum(2), "D"),
                         bonds));
  BOOST_CHECK(find_bond(ent.FindAtom("B", mol::ResNum(1), "E"),
                        ent.FindAtom("B", mol::ResNum(1), "F"),
                        bonds));
  BOOST_CHECK(find_bond(ent.FindAtom("B", mol::ResNum(1), "F"),
                        ent.FindAtom("B", mol::ResNum(2), "G"),
                        bonds));
  BOOST_CHECK(find_bond(ent.FindAtom("B", mol::ResNum(2), "G"),
                        ent.FindAtom("B", mol::ResNum(2), "H"),
                        bonds));
}

BOOST_AUTO_TEST_CASE(test_union_a)
{
  EntityHandle ent=mk_test_ent();
  EntityView v1=ent.Select("aname=B,C");
  EntityView v2=ent.Select("aname=F,G");
  EntityView un=mol::Union(v1, v2);
  BOOST_CHECK_EQUAL(un.GetAtomCount(), 4);
  BOOST_CHECK_EQUAL(un.GetResidueCount(), 4); 
  BOOST_CHECK_EQUAL(un.GetChainCount(), 2);  
  // test chains
  BOOST_CHECK(un.FindChain("A"));
  BOOST_CHECK(un.FindChain("B"));
  
  // test residues
  BOOST_CHECK(un.FindResidue(ent.FindResidue("A", mol::ResNum(1))));
  BOOST_CHECK(un.FindResidue(ent.FindResidue("A", mol::ResNum(2))));  
  BOOST_CHECK(un.FindResidue(ent.FindResidue("B", mol::ResNum(1))));
  BOOST_CHECK(un.FindResidue(ent.FindResidue("B", mol::ResNum(2))));  
  
  // test atoms
  BOOST_CHECK(!un.FindAtom(ent.FindAtom("A", mol::ResNum(1), "A")));
  BOOST_CHECK(un.FindAtom(ent.FindAtom("A", mol::ResNum(1), "B")));
  BOOST_CHECK(un.FindAtom(ent.FindAtom("A", mol::ResNum(2), "C")));
  BOOST_CHECK(!un.FindAtom(ent.FindAtom("A", mol::ResNum(2), "D")));
  
  BOOST_CHECK(!un.FindAtom(ent.FindAtom("B", mol::ResNum(1), "E")));
  BOOST_CHECK(un.FindAtom(ent.FindAtom("B", mol::ResNum(1), "F")));  
  BOOST_CHECK(un.FindAtom(ent.FindAtom("B", mol::ResNum(2), "G")));
  BOOST_CHECK(!un.FindAtom(ent.FindAtom("B", mol::ResNum(2), "H")));
  
  // bonds
   BondHandleList bonds=un.GetBondList();
   BOOST_CHECK(!find_bond(ent.FindAtom("A", mol::ResNum(1), "A"),
                          ent.FindAtom("A", mol::ResNum(1), "B"),
                          bonds));
   BOOST_CHECK(find_bond(ent.FindAtom("A", mol::ResNum(1), "B"),
                         ent.FindAtom("A", mol::ResNum(2), "C"),
                         bonds));
   BOOST_CHECK(!find_bond(ent.FindAtom("A", mol::ResNum(2), "C"),
                          ent.FindAtom("A", mol::ResNum(2), "D"),
                          bonds));                       
   BOOST_CHECK(!find_bond(ent.FindAtom("B", mol::ResNum(1), "E"),
                          ent.FindAtom("B", mol::ResNum(1), "F"),
                          bonds));                       
   BOOST_CHECK(find_bond(ent.FindAtom("B", mol::ResNum(1), "F"),
                         ent.FindAtom("B", mol::ResNum(2), "G"),
                         bonds));
   BOOST_CHECK(!find_bond(ent.FindAtom("B", mol::ResNum(2), "G"),
                          ent.FindAtom("B", mol::ResNum(2), "H"),
                          bonds));
}

BOOST_AUTO_TEST_CASE(test_union_b)
{
  EntityHandle ent=mk_test_ent();
  EntityView v1=ent.Select("aname=B,C");
  EntityView v2=ent.Select("aname=A,B,C,F,G");
  EntityView un=mol::Union(v1, v2);
  BOOST_CHECK_EQUAL(un.GetAtomCount(), 5);
  BOOST_CHECK_EQUAL(un.GetResidueCount(), 4); 
  BOOST_CHECK_EQUAL(un.GetChainCount(), 2);
  

  // test chains
  BOOST_CHECK(un.FindChain("A"));
  BOOST_CHECK(un.FindChain("B"));
  
  // test residues
  BOOST_CHECK(un.FindResidue(ent.FindResidue("A", mol::ResNum(1))));
  BOOST_CHECK(un.FindResidue(ent.FindResidue("A", mol::ResNum(2))));  
  BOOST_CHECK(un.FindResidue(ent.FindResidue("B", mol::ResNum(1))));
  BOOST_CHECK(un.FindResidue(ent.FindResidue("B", mol::ResNum(2))));  
  
  // test atoms
  BOOST_CHECK(un.FindAtom(ent.FindAtom("A", mol::ResNum(1), "A")));
  BOOST_CHECK(un.FindAtom(ent.FindAtom("A", mol::ResNum(1), "B")));
  BOOST_CHECK(un.FindAtom(ent.FindAtom("A", mol::ResNum(2), "C")));
  BOOST_CHECK(!un.FindAtom(ent.FindAtom("A", mol::ResNum(2), "D")));
  
  BOOST_CHECK(!un.FindAtom(ent.FindAtom("B", mol::ResNum(1), "E")));
  BOOST_CHECK(un.FindAtom(ent.FindAtom("B", mol::ResNum(1), "F")));  
  BOOST_CHECK(un.FindAtom(ent.FindAtom("B", mol::ResNum(2), "G")));
  BOOST_CHECK(!un.FindAtom(ent.FindAtom("B", mol::ResNum(2), "H")));
  
  // bonds
   BondHandleList bonds=un.GetBondList();
   BOOST_CHECK(find_bond(ent.FindAtom("A", mol::ResNum(1), "A"),
                         ent.FindAtom("A", mol::ResNum(1), "B"),
                         bonds));
   BOOST_CHECK(find_bond(ent.FindAtom("A", mol::ResNum(1), "B"),
                         ent.FindAtom("A", mol::ResNum(2), "C"),
                         bonds));
   BOOST_CHECK(!find_bond(ent.FindAtom("A", mol::ResNum(2), "C"),
                          ent.FindAtom("A", mol::ResNum(2), "D"),
                          bonds));                       
   BOOST_CHECK(!find_bond(ent.FindAtom("B", mol::ResNum(1), "E"),
                          ent.FindAtom("B", mol::ResNum(1), "F"),
                          bonds));                       
   BOOST_CHECK(find_bond(ent.FindAtom("B", mol::ResNum(1), "F"),
                         ent.FindAtom("B", mol::ResNum(2), "G"),
                         bonds));
   BOOST_CHECK(!find_bond(ent.FindAtom("B", mol::ResNum(2), "G"),
                          ent.FindAtom("B", mol::ResNum(2), "H"),
                          bonds));
}

BOOST_AUTO_TEST_CASE(test_intersection_a)
{
  EntityHandle ent=mk_test_ent();
  EntityView v1=ent.Select("aname=B,C,F,G");
  EntityView v2=ent.Select("aname=C,F,G");
  EntityView is=mol::Intersection(v1, v2);
  
  BOOST_CHECK_EQUAL(is.GetBondCount(), 1);
  BOOST_CHECK_EQUAL(is.GetAtomCount(), 3);
  BOOST_CHECK_EQUAL(is.GetResidueCount(), 3);
  BOOST_CHECK_EQUAL(is.GetChainCount(), 2);
  // test chains
  BOOST_CHECK(is.FindChain("A"));
  BOOST_CHECK(is.FindChain("B"));
  
  // test residues
  BOOST_CHECK(!is.FindResidue(ent.FindResidue("A", mol::ResNum(1))));
  BOOST_CHECK(is.FindResidue(ent.FindResidue("A", mol::ResNum(2))));  
  BOOST_CHECK(is.FindResidue(ent.FindResidue("B", mol::ResNum(1))));
  BOOST_CHECK(is.FindResidue(ent.FindResidue("B", mol::ResNum(2))));  
  
  // test atoms
  BOOST_CHECK(!is.FindAtom(ent.FindAtom("A", mol::ResNum(1), "A")));
  BOOST_CHECK(!is.FindAtom(ent.FindAtom("A", mol::ResNum(1), "B")));
  BOOST_CHECK(is.FindAtom(ent.FindAtom("A", mol::ResNum(2), "C")));
  BOOST_CHECK(!is.FindAtom(ent.FindAtom("A", mol::ResNum(2), "D")));
  
  BOOST_CHECK(!is.FindAtom(ent.FindAtom("B", mol::ResNum(1), "E")));
  BOOST_CHECK(is.FindAtom(ent.FindAtom("B", mol::ResNum(1), "F")));  
  
  BOOST_CHECK(is.FindAtom(ent.FindAtom("B", mol::ResNum(2), "G")));
  BOOST_CHECK(!is.FindAtom(ent.FindAtom("B", mol::ResNum(2), "H")));

  // bonds
  BondHandleList bonds=is.GetBondList();
  BOOST_CHECK(!find_bond(ent.FindAtom("A", mol::ResNum(1), "A"),
                         ent.FindAtom("A", mol::ResNum(1), "B"),
                         bonds));
  BOOST_CHECK(!find_bond(ent.FindAtom("A", mol::ResNum(1), "B"),
                        ent.FindAtom("A", mol::ResNum(2), "C"),
                         bonds));
  BOOST_CHECK(!find_bond(ent.FindAtom("A", mol::ResNum(2), "C"),
                         ent.FindAtom("A", mol::ResNum(2), "D"),
                         bonds));
  BOOST_CHECK(!find_bond(ent.FindAtom("B", mol::ResNum(1), "E"),
                         ent.FindAtom("B", mol::ResNum(1), "F"),
                         bonds));
  BOOST_CHECK(find_bond(ent.FindAtom("B", mol::ResNum(1), "F"),
                        ent.FindAtom("B", mol::ResNum(2), "G"),
                        bonds));
  BOOST_CHECK(!find_bond(ent.FindAtom("B", mol::ResNum(2), "G"),
                         ent.FindAtom("B", mol::ResNum(2), "H"),
                         bonds));
}

BOOST_AUTO_TEST_CASE(test_intersection_b)
{
  EntityHandle ent=mk_test_ent();
  EntityView v1=ent.Select("aname=B,C,F,G");
  EntityView v2=ent.Select("aname=F,G");
  EntityView is=mol::Intersection(v1, v2);
  
  BOOST_CHECK_EQUAL(is.GetBondCount(), 1);
  BOOST_CHECK_EQUAL(is.GetAtomCount(), 2);
  BOOST_CHECK_EQUAL(is.GetResidueCount(), 2);
  BOOST_CHECK_EQUAL(is.GetChainCount(), 1);
  // test chains
  BOOST_CHECK(!is.FindChain("A"));
  BOOST_CHECK(is.FindChain("B"));
  
  // test residues
  BOOST_CHECK(!is.FindResidue(ent.FindResidue("A", mol::ResNum(1))));
  BOOST_CHECK(!is.FindResidue(ent.FindResidue("A", mol::ResNum(2))));  
  BOOST_CHECK(is.FindResidue(ent.FindResidue("B", mol::ResNum(1))));
  BOOST_CHECK(is.FindResidue(ent.FindResidue("B", mol::ResNum(2))));  
  
  // test atoms
  BOOST_CHECK(!is.FindAtom(ent.FindAtom("A", mol::ResNum(1), "A")));
  BOOST_CHECK(!is.FindAtom(ent.FindAtom("A", mol::ResNum(1), "B")));
  BOOST_CHECK(!is.FindAtom(ent.FindAtom("A", mol::ResNum(2), "C")));
  BOOST_CHECK(!is.FindAtom(ent.FindAtom("A", mol::ResNum(2), "D")));
  
  BOOST_CHECK(!is.FindAtom(ent.FindAtom("B", mol::ResNum(1), "E")));
  BOOST_CHECK(is.FindAtom(ent.FindAtom("B", mol::ResNum(1), "F")));  
  
  BOOST_CHECK(is.FindAtom(ent.FindAtom("B", mol::ResNum(2), "G")));
  BOOST_CHECK(!is.FindAtom(ent.FindAtom("B", mol::ResNum(2), "H")));
  
  // bonds
  BondHandleList bonds=is.GetBondList();
  BOOST_CHECK(!find_bond(ent.FindAtom("A", mol::ResNum(1), "A"),
                         ent.FindAtom("A", mol::ResNum(1), "B"),
                         bonds));
  BOOST_CHECK(!find_bond(ent.FindAtom("A", mol::ResNum(1), "B"),
                         ent.FindAtom("A", mol::ResNum(2), "C"),
                         bonds));
  BOOST_CHECK(!find_bond(ent.FindAtom("A", mol::ResNum(2), "C"),
                         ent.FindAtom("A", mol::ResNum(2), "D"),
                         bonds));
  BOOST_CHECK(!find_bond(ent.FindAtom("B", mol::ResNum(1), "E"),
                         ent.FindAtom("B", mol::ResNum(1), "F"),
                         bonds));
  BOOST_CHECK(find_bond(ent.FindAtom("B", mol::ResNum(1), "F"),
                        ent.FindAtom("B", mol::ResNum(2), "G"),
                        bonds));
  BOOST_CHECK(!find_bond(ent.FindAtom("B", mol::ResNum(2), "G"),
                         ent.FindAtom("B", mol::ResNum(2), "H"),
                         bonds));
}

BOOST_AUTO_TEST_SUITE_END()

