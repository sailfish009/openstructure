//------------------------------------------------------------------------------
// This file is part of the OpenStructure project <www.openstructure.org>
//
// Copyright (C) 2008-2011 by the OpenStructure authors
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
#include <ost/mol/entity_view.hh>
#include <ost/mol/atom_view.hh>
#include <ost/mol/iterator.hh>
#include "clash_score.hh"

namespace ost { namespace qa {

namespace {
  
template <typename T, typename I>
Real do_clash_score(const T& ent_a, const mol::EntityView& ent_b)
{
  Real energy=0.0;
  for (I i=ent_a.AtomsBegin(), e=ent_a.AtomsEnd(); i!=e; ++i) {
    
    mol::AtomViewList clashees=ent_b.FindWithin((*i).GetPos(), 
                                                (*i).GetRadius()+1.7);

    for (mol::AtomViewList::iterator j=clashees.begin(), 
         e2=clashees.end(); j!=e2; ++j) {
         energy+=StericEnergy((*j).GetPos(), (*j).GetRadius()-0.25,
                              (*i).GetPos(), (*i).GetRadius()-0.25);           
    }
  }
  return energy;  
}

}

Real StericEnergy(const geom::Vec3& pos1, Real r1,
                  const geom::Vec3& pos2, Real r2)
{
  geom::Vec3 d=pos1-pos2;
  // delay calculation of square root as long as possible. It is only needed
  // in the rare case when 0.8254*rr <= d <= rr
  Real distance_sqr=geom::Length2(d); 
  Real rr=r1+r2;
  if (distance_sqr>rr*rr) {
    return 0.0;
  }
  if (distance_sqr<0.8254*0.8254*rr*rr) {
    return 10.0;
  }
  return 57.273*(1.0-sqrt(distance_sqr)/rr);
}

Real ClashScore(const mol::EntityView& ent_a, const mol::EntityView& ent_b)
{
  return do_clash_score<mol::EntityView, mol::AtomViewIter>(ent_a, ent_b);
}

Real ClashScore(const mol::EntityHandle& ent_a, const mol::EntityView& ent_b)
{
  return do_clash_score<mol::EntityHandle, mol::AtomHandleIter>(ent_a, ent_b);  
}

Real ClashScore(const mol::AtomHandle& atom, const mol::EntityView& ent_b)
{
  Real energy=0.0;
  mol::AtomViewList clashees=ent_b.FindWithin(atom.GetPos(), 
                                              atom.GetRadius()+2.0);
  for (mol::AtomViewList::iterator j=clashees.begin(), 
       e2=clashees.end(); j!=e2; ++j) {
       energy+=StericEnergy((*j).GetPos(), (*j).GetRadius(),
                            atom.GetPos(), atom.GetRadius());           
  }
  return energy;                                 
}

}}
