//------------------------------------------------------------------------------
// This file is part of the OpenStructure project <www.openstructure.org>
//
// Copyright (C) 2008-2020 by the OpenStructure authors
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
 *  Authors: Marco Biasini
 */

#include <ost/mol/builder.hh>
#include <ost/mol/transfer_connectivity.hh>

#define BOOST_TEST_DYN_LINK
#include <boost/test/unit_test.hpp>

using namespace ost;
using namespace ost::mol;

BOOST_AUTO_TEST_SUITE( mol_transfer_conn );

BOOST_AUTO_TEST_CASE(test_transfer_conn)
{
  EntityHandle ent = Builder()
                       .Chain("A")
                         .Gly()
                         .Gly()
                         .ConnectToPrev()
                         .Gly()
                         .ConnectToPrev()
                   ;

  ent.EditXCS().Connect(ent.FindAtom("A", 1, "O"), ent.FindAtom("A", 3, "O"));

  EntityHandle ent2 = Builder()
                       .Chain("A")
                         .Gly(false)
                         .Gly(false)
                         .Gly(false)
                   ;
  std::map<ResidueHandle, ResidueHandle> to_from;
  ResidueHandleList r1 = ent.GetResidueList();
  ResidueHandleList r2 = ent2.GetResidueList();
  for (size_t i = 0;i<r1.size(); ++i) {
    to_from[r2[i]] = r1[i];
  }
  TransferConnectivity(ent2, to_from);
  BOOST_CHECK(BondExists(ent2.FindAtom("A", 1, "N"), ent2.FindAtom("A", 1, "CA")));
  BOOST_CHECK(BondExists(ent2.FindAtom("A", 1, "CA"), ent2.FindAtom("A", 1, "C")));
  BOOST_CHECK(BondExists(ent2.FindAtom("A", 1, "C"), ent2.FindAtom("A", 1, "O")));
  BOOST_CHECK(BondExists(ent2.FindAtom("A", 1, "C"), ent2.FindAtom("A", 2, "N")));
  BOOST_CHECK(BondExists(ent2.FindAtom("A", 2, "N"), ent2.FindAtom("A", 2, "CA")));
  BOOST_CHECK(BondExists(ent2.FindAtom("A", 2, "CA"), ent2.FindAtom("A", 2, "C")));
  BOOST_CHECK(BondExists(ent2.FindAtom("A", 2, "C"), ent2.FindAtom("A", 2, "O")));
  BOOST_CHECK(BondExists(ent2.FindAtom("A", 2, "C"), ent2.FindAtom("A", 3, "N")));
  BOOST_CHECK(BondExists(ent2.FindAtom("A", 3, "N"), ent2.FindAtom("A", 3, "CA")));
  BOOST_CHECK(BondExists(ent2.FindAtom("A", 3, "CA"), ent2.FindAtom("A", 3, "C")));
  BOOST_CHECK(BondExists(ent2.FindAtom("A", 3, "C"), ent2.FindAtom("A", 3, "O")));
  BOOST_CHECK(BondExists(ent2.FindAtom("A", 1, "O"), ent2.FindAtom("A", 3, "O")));
  BOOST_CHECK_EQUAL(12, ent2.GetBondCount());
}



BOOST_AUTO_TEST_SUITE_END();
