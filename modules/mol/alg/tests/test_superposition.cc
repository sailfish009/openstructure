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
 * Author Juergen Haas
 */
#include <ost/mol/alg/svd_superpose.hh>
#define BOOST_TEST_DYN_LINK
#include <boost/test/unit_test.hpp>
#include <ost/mol/entity_handle.hh>


using namespace ost;
using namespace ost::mol;
using namespace ost::mol::alg;

struct Fixture {
  Fixture() {

    AtomProp prop;
    prop.element="C";

    e=CreateEntity();
    XCSEditor ed=e.RequestXCSEditor();
    c=ed.InsertChain("A");
    r=ed.AppendResidue(c, "XXX");
    ab=ed.InsertAtom(r, "A", geom::Vec3(-5,-5,-5), prop);
    ac=ed.InsertAtom(r, "B", geom::Vec3(-5, 5,-5), prop);
    ad=ed.InsertAtom(r, "C", geom::Vec3(-5, 5, 5), prop);
    ae=ed.InsertAtom(r, "D", geom::Vec3(-5,-5, 5), prop);
    af=ed.InsertAtom(r, "E", geom::Vec3(5,-5, 5), prop);
    ag=ed.InsertAtom(r, "F", geom::Vec3(5,-5,-5), prop);
    ah=ed.InsertAtom(r, "G", geom::Vec3(5, 5,-5), prop);
    ai=ed.InsertAtom(r, "H", geom::Vec3(5, 5, 5), prop);


    BondHandle b1=ed.Connect(ab, ad);
    BondHandle b2=ed.Connect(ad, ah);
    BondHandle b3=ed.Connect(ah, ae);
    BondHandle b4=ed.Connect(ae, ab);

    BondHandle b5=ed.Connect(ac, ag);
    BondHandle b6=ed.Connect(ag, ai);
    BondHandle b7=ed.Connect(ai, af);
    BondHandle b8=ed.Connect(af, ac);
    BondHandle b9=ed.Connect(ab, ac);
    BondHandle b10=ed.Connect(ah, ai);
    BondHandle b11=ed.Connect(ae, ag);
    BondHandle b12=ed.Connect(ad, af);



  }
  EntityHandle e;
  ChainHandle c;
  ResidueHandle r;
  AtomHandle ab;
  AtomHandle ac;
  AtomHandle ad;
  AtomHandle ae;
  AtomHandle af;
  AtomHandle ag;
  AtomHandle ah;
  AtomHandle ai;

};

BOOST_AUTO_TEST_SUITE( mol_alg )

BOOST_AUTO_TEST_CASE(superposition_svd) 
{
  Fixture f1, f2;
  EntityView ev1 = f1.e.CreateFullView();
  XCSEditor ed=f1.e.RequestXCSEditor();
  EntityView ev2 = f2.e.CreateFullView();
  ChainHandle ch1=f1.e.GetChainList()[0];
  ResidueHandleIter rit=ch1.ResiduesBegin();
  for (;rit!=ch1.ResiduesEnd();++rit){
    ResidueHandle resh=*rit;
    AtomHandleIter at=resh.AtomsBegin();
    for (;at!=resh.AtomsEnd();++at){
      AtomHandle atom=*at;
      ed.SetAtomPos(atom, geom::EulerTransformation(0.1, 0, 0)*atom.GetPos()
                  +geom::Vec3(0, 5, 0));
    }
  }
  SuperpositionResult res;
  res=SuperposeSVD(ev1, ev2, true);
  BOOST_CHECK(abs(res.rmsd)<0.0001);
}

BOOST_AUTO_TEST_SUITE_END()