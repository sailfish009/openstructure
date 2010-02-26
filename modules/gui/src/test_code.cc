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
#include <cstdlib>

#include "test_code.hh"

#include <ost/log.hh>
#include <ost/entity.hh>
#include <ost/gfx/scene.hh>
#include <ost/gfx/entity_connect.hh>

using namespace ost;
using namespace ost::gfx;

namespace {

mol::EntityHandle build_entity()
{
  mol::EntityHandle e = CreateEntity();

  mol::ChainHandle chain = e.InsertChain("");

  mol::ResidueHandle res = chain.AppendResidue("");

  mol::AtomHandle a0 = res.InsertAtom("X",geom::Vec3(0.0,0.0,0.0));
  mol::AtomHandle a1 = res.InsertAtom("X",geom::Vec3(0.0,0.0,10.0));
  mol::AtomHandle a2 = res.InsertAtom("X",geom::Vec3(10.0,0.0,10.0));
  mol::AtomHandle a3 = res.InsertAtom("X",geom::Vec3(10.0,-10.0,10.0));
  mol::AtomHandle a4 = res.InsertAtom("X",geom::Vec3(10.0,0.0,10.0));
  mol::AtomHandle a5 = res.InsertAtom("X",geom::Vec3(10.0,0.0,20.0));
  //mol::AtomHandle a6 = res.InsertAtom(geom::Vec3(10.0,-10.0,20.0));
  //mol::AtomHandle a7 = res.InsertAtom(geom::Vec3(20.0,0.0,20.0));
  e.Connect(a0,a1);
  e.Connect(a1,a2);
  e.Connect(a2,a3);
  e.Connect(a1,a4);
  e.Connect(a2,a5);
  //e.Connect(a5,a6);
  //e.Connect(a5,a7);

  e.RebuildICS();

  return e;
}

} // anon ns

void test_code()
{
  mol::EntityHandle eh = build_entity();

  GfxObjP go (new EntityConnect("test",eh));
  Scene::Instance().Add(go);
  Scene::Instance().SetCenter(go->GetCenter());
}
