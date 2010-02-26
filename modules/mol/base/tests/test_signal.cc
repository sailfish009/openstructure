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
#include <ost/log.hh>
#define BOOST_TEST_DYN_LINK
#include <boost/test/unit_test.hpp>
#include <ost/message.hh>
#include <ost/geom/geom.hh>

using namespace ost;
using namespace ost::mol;

int num_of_atoms_deleted=0;
void on_delete_atom(const AtomHandle& atom) {
  num_of_atoms_deleted++;
}


struct Fixture {
  Fixture() {
    e=CreateEntity();
    c=e.InsertChain("A");
    r=c.AppendResidue("XXX");
    aa=r.InsertAtom("A", geom::Vec3());
    ab=r.InsertAtom("B", geom::Vec3());
    ac=r.InsertAtom("C", geom::Vec3());
    ad=r.InsertAtom("D", geom::Vec3());

    BondHandle b1=e.Connect(aa, ab);
    BondHandle b2=e.Connect(ab, ac);
    BondHandle b3=e.Connect(ac, ad);

    TorsionHandle t=e.AddTorsion("TORSION", aa, ab, ac, ad);
  }
  EntityHandle e;
  ChainHandle  c;
  ResidueHandle r;
  AtomHandle aa;
  AtomHandle ab;
  AtomHandle ac;
  AtomHandle ad;
};

void test_signal() {
  Fixture f1;
  SignalConnection c=f1.e.OnAtomDeleted().AttachObserver(on_delete_atom);
  f1.r.DeleteAtom(f1.aa);
  BOOST_CHECK_EQUAL(num_of_atoms_deleted, 1);
  f1.e.OnAtomDeleted().DetachObserver(c);
  f1.r.DeleteAtom(f1.ab);
  BOOST_CHECK_EQUAL(num_of_atoms_deleted, 1);
}


BOOST_AUTO_TEST_SUITE( mol_base )

BOOST_AUTO_TEST_CASE(test_signal) 
{
  test_signal();
}

BOOST_AUTO_TEST_SUITE_END()
