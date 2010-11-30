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
#include <ost/log.hh>
#include <ost/mol/mol.hh>
#include "filter_clashes.hh"

namespace ost { namespace mol { namespace alg {

namespace {

Real GetThreshold(const String& ele1, const String& ele2) {
  if (ele1.length()!=1 || ele2.length()!=1) {
    return 1.5;
  }
  switch (ele1[0]) {
    case 'C' :
      switch (ele2[0]) {
        case 'C' : return 2.10;
        case 'N' : return 2.10;
        case 'S' : return 2.45;
        case 'O' : return 2.25;
        default: return 1.5;
      }
    case 'N':
      switch (ele2[0]) {
        case 'C' : return 2.10;
        case 'N' : return 2.05;
        case 'S' : return 2.55;
        case 'O' : return 2.10;
        default: return 1.5;
      }
    case 'O':
      switch (ele2[0]) {
        case 'C' : return 2.25;
        case 'N' : return 2.10;
        case 'S' : return 2.45;
        case 'O' : return 2.05;
        default: return 1.5;
      }
    case 'S':
      switch (ele2[0]) {
        case 'C' : return 2.45;
        case 'N' : return 2.55;
        case 'S' : return 1.80;
        case 'O' : return 2.45;
        default: return 1.5;
      }
    default:
      return 1.5;
  }
}


}

EntityView FilterClashes(const EntityView& ent, Real tolerance)
{
  EntityView filtered=ent.CreateEmptyView();
  ResidueViewList residues=ent.GetResidueList();
  for (ResidueViewList::iterator 
       i=residues.begin(), e=residues.end(); i!=e; ++i) {
    bool remove_sc=false, remove_bb=false;
    ResidueView res=*i;
    const AtomViewList& atoms=res.GetAtomList();
    for (AtomViewList::const_iterator 
         j=atoms.begin(), e2=atoms.end(); j!=e2; ++j) {
      AtomView atom=*j;
      String ele1=atom.GetElement();
      if (ele1=="H" || ele1=="D") {
        continue;
      }
      AtomViewList within=ent.FindWithin(atom.GetPos(), 2.5-tolerance);
      for (AtomViewList::iterator 
           k=within.begin(), e3=within.end(); k!=e3; ++k) {
        AtomView atom2=*k;
        if (atom2==atom) {
          continue;
        }
        String ele2=atom2.GetElement();
        if (ele2=="H" || ele2=="D") {
          continue;
        }
        // In theory, this should also trigger for disulfide bonds, but 
        // since we don't detect disulfides correctly, we can't count on that 
        // and we instead allow S-S distances down to 1.8.
        if (atom.GetHandle().FindBondToAtom(atom2.GetHandle()).IsValid()) {
          continue;
        }
        Real d=geom::Length2(atom.GetPos()-atom2.GetPos());
        Real threshold=GetThreshold(ele1, ele2)-tolerance;
        if (d<threshold*threshold) {
          remove_sc=true;
          String name=atom.GetName();
          if (name=="CA" || name=="N" || name=="O" || name=="C") {
            remove_bb=true;
          }
        }
      }
    }
    if (remove_bb) {
      LOG_VERBOSE("removing whole residue " << res);
      continue;
    }
    if (remove_sc) {
      LOG_VERBOSE("removing sidechain of residue " << res);
      for (AtomViewList::const_iterator 
           j=atoms.begin(), e2=atoms.end(); j!=e2; ++j) {
       AtomView atom=*j;
       String name=atom.GetName();
       if (name=="CA" || name=="N" || name=="O" || name=="C") {
         filtered.AddAtom(atom);
       }
      }
    }
    filtered.AddResidue(res, ViewAddFlag::INCLUDE_ATOMS);
  }
  return filtered;
}


EntityView FilterClashes(const EntityHandle& ent, Real tolerance)
{
  return FilterClashes(ent.CreateFullView(), tolerance);
}


}}}

