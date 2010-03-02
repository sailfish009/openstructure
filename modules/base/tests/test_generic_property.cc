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
#define BOOST_TEST_DYN_LINK
#include <boost/test/unit_test.hpp>


using namespace ost;
using namespace ost::mol;

BOOST_AUTO_TEST_SUITE( base )

BOOST_AUTO_TEST_CASE( test_generic_property )
{
  EntityHandle eh=CreateEntity();
  XCSEditor editor=eh.RequestXCSEditor();  
  ChainHandle ch=editor.InsertChain("A");
  ResidueHandle res = editor.AppendResidue(ch, "X");
  AtomHandle atom = editor.InsertAtom(res, "Y",geom::Vec3());
  AtomHandle atom2 = editor.InsertAtom(res, "Y",geom::Vec3());
  BondHandle bond = editor.Connect(atom, atom2);

  eh.SetGenericStringProperty("a","123");
  ch.SetGenericFloatProperty("b",1.5);
  res.SetGenericIntProperty("c",123);
  atom.SetGenericBoolProperty("d",true);
  bond.SetGenericIntProperty("e", 12);

  BOOST_CHECK(eh.HasGenericProperty("x")==false);
  BOOST_CHECK(eh.HasGenericProperty("a")==true);

  BOOST_CHECK(eh.GetGenericStringProperty("a")=="123");
  BOOST_CHECK(ch.GetGenericFloatProperty("b")==1.5);
  BOOST_CHECK(res.GetGenericIntProperty("c")==123);
  BOOST_CHECK(atom.GetGenericBoolProperty("d")==true);
  BOOST_CHECK(bond.GetGenericIntProperty("e")==12);

  EntityView ev = eh.CreateFullView();
  ChainView chv = ev.FindChain("A");
  ResidueView resv = ev.FindResidue(res);
  AtomView atomv = ev.FindAtom(atom);

  BOOST_CHECK(ev.GetGenericStringProperty("a")=="123");
  BOOST_CHECK(chv.GetGenericFloatProperty("b")==1.5);
  BOOST_CHECK(resv.GetGenericIntProperty("c")==123);
  BOOST_CHECK(atomv.GetGenericBoolProperty("d")==true);
}

BOOST_AUTO_TEST_SUITE_END()
