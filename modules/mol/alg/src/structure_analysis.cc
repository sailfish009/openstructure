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

/*
 * Author Niklaus Johner
 */

#include <ost/base.hh>
#include <ost/mol/mol.hh>
#include "structure_analysis.hh"

namespace ost { namespace mol { namespace alg {

geom::Vec3List GetPosListFromView(const EntityView& view){
  CheckHandleValidity(view);
  geom::Vec3List vl;
  AtomViewList atoms=view.GetAtomList();
  vl.reserve(atoms.size());
  for (AtomViewList::const_iterator i=atoms.begin(), 
       e=atoms.end(); i!=e; ++i) {
    vl.push_back(i->GetPos());
  }
  return vl;
}    
  
Real CalculateAgreementWithDensityMap(const geom::Vec3List& vl, img::MapHandle& density_map){
  Real sum,v;
  sum=0;
  CheckHandleValidity(density_map);
  for (geom::Vec3List::const_iterator v1=vl.begin(),e=vl.end(); v1!=e; ++v1) {
    img::Point p(density_map.CoordToIndex(*v1));
    v=density_map.GetReal(p);
    sum=sum+v;
  }
  return sum;
}
  
}}} //ns
