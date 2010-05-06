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
 *  Authors: Marco Biasini, Juergen Haas
 */
#include <ost/mol/mol.hh>

#include <ost/message.hh>
#define BOOST_TEST_DYN_LINK
#include <boost/test/unit_test.hpp>


using namespace ost;
using namespace ost::mol;

BOOST_AUTO_TEST_SUITE( mol_base )

BOOST_AUTO_TEST_CASE(test_in_sequence) 
{
  EntityHandle eh=CreateEntity();
  XCSEditor e=eh.RequestXCSEditor();
  ChainHandle ch1=e.InsertChain("A");
  ResidueHandle rA = e.AppendResidue(ch1, "A");
  ResidueHandle rB = e.AppendResidue(ch1, "B");
  ResidueHandle rC = e.AppendResidue(ch1, "C");  

  BOOST_CHECK(!InSequence(rA,rB));
  BOOST_CHECK(!InSequence(rB,rC));
  BOOST_CHECK(!InSequence(rA,rC));
}

BOOST_AUTO_TEST_CASE(test_res_index)
{
  EntityHandle eh=CreateEntity();
  XCSEditor e=eh.RequestXCSEditor();
  ChainHandle ch1=e.InsertChain("A");
  ResidueHandle rA = e.AppendResidue(ch1, "A");
  ResidueHandle rB = e.AppendResidue(ch1, "B");
  ResidueHandle rC = e.AppendResidue(ch1, "C");
  EntityView v(eh);
  ChainView cv1(v, ch1);
  ResidueView rvA = cv1.AddResidue(rA);
  ResidueView rvB = cv1.AddResidue(rB);
  ResidueView rvC = cv1.AddResidue(rC);
  BOOST_CHECK_EQUAL(rvA.GetIndex(),0);
  BOOST_CHECK_EQUAL(rvA.GetIndex(),cv1.GetResidueIndex(rvA.GetNumber()));
  BOOST_CHECK_EQUAL(rvB.GetIndex(),1);
  BOOST_CHECK_EQUAL(rvB.GetIndex(),cv1.GetResidueIndex(rvB.GetNumber()));
  BOOST_CHECK_EQUAL(rvC.GetIndex(),2);
  BOOST_CHECK_EQUAL(rvC.GetIndex(),cv1.GetResidueIndex(rvC.GetNumber()));
  cv1.RemoveResidue(rvB);
  BOOST_CHECK_EQUAL(rvA.GetIndex(),0);
  BOOST_CHECK_EQUAL(rvA.GetIndex(),cv1.GetResidueIndex(rvA.GetNumber()));
  BOOST_CHECK_EQUAL(rvC.GetIndex(),1);
  BOOST_CHECK_EQUAL(rvC.GetIndex(),cv1.GetResidueIndex(rvC.GetNumber()));
}

BOOST_AUTO_TEST_SUITE_END()
