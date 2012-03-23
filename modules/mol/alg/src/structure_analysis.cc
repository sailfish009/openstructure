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
 
void DLLEXPORT_OST_MOL_ALG WrapEntityInPeriodicCell(EntityHandle eh, const geom::Vec3 cell_center, const geom::Vec3 basis_vec){
  mol::XCSEditor edi=eh.EditXCS(mol::BUFFERED_EDIT);
  geom::Vec3 cm,wrapped_cm,shift;
  edi=eh.EditXCS();
  ResidueHandleList residues=eh.GetResidueList();
  unsigned int n_residues=eh.GetResidueCount();
  for (unsigned int i=0; i<n_residues; ++i) {
    ResidueHandle r=residues[i];
    cm=r.GetCenterOfMass();
    wrapped_cm=geom::WrapVec3(cm,cell_center,basis_vec);
    if (wrapped_cm==cm) continue;
    AtomHandleList atoms=r.GetAtomList();
    unsigned int n_atoms=r.GetAtomCount();
    shift=wrapped_cm-cm;
    for (unsigned int j=0; j<n_atoms; ++j) {
      edi.SetAtomPos(atoms[j],atoms[j].GetPos()+shift);
    }
  }
}

}}} //ns
