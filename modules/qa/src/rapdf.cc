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
#include "rapdf.hh"
#include <ost/mol/mol.hh>
#include <ost/geom/geom.hh>
namespace ost { namespace qa {


RAPDF::RAPDF(const String& file) {
  energies_.reset(new InteractionEnergies(file));
}

RAPDF::RAPDF(InteractionStatisticsPtr statistics, Real sigma) {
  typedef InteractionEnergies::IndexType Index;
  Real upper=statistics->GetUpperCutoff();        
  Real lower=statistics->GetLowerCutoff();
  Real bucket_size=statistics->GetDistanceBucketSize();                 
  uint32_t distance_buckets=uint32_t((upper-lower)/bucket_size);
  ContinuousClassifier distance_classifier(distance_buckets, lower, upper);
  energies_.reset(new InteractionEnergies(0.0,
                                          IntegralClassifier(atom::UNKNOWN, 0),
                                          IntegralClassifier(atom::UNKNOWN, 0),
                                          distance_classifier));
  // loop over all distance buckets and calculate energies for all atom pairs.
  for (uint32_t i=0; i<distance_buckets;++i) {
    Index start(0, 0, i), end(atom::UNKNOWN-1, atom::UNKNOWN-1, i);    
    Real radius=i*bucket_size+bucket_size/2;
    uint32_t total=statistics->GetCount(radius);
    for (uint32_t j=0; j<atom::UNKNOWN; ++j) {
      for (uint32_t k=0; k<atom::UNKNOWN;++k) {
        uint32_t lc=statistics->GetCount(atom::ChemType(j), 
                                       atom::ChemType(k), radius);
        Real frequency=lc/Real(total);
        Real energy=log((1+lc*sigma)/(1+lc*sigma*frequency));        
        energies_->Set(Index(j, k, i), energy);        
      }
    } 
  }
}

Real RAPDF::GetTotalEnergy(mol::EntityView entity) {
  tmp_energy_=0.0;
  entity.Apply(*this);
  return tmp_energy_;
}

RAPDF::RAPDF(const RAPDF& rhs) {
  energies_.reset(new InteractionEnergies(*rhs.energies_.get()));
}

bool RAPDF::VisitAtom(const mol::AtomHandle& atom) {
  mol::EntityHandle e=atom.GetResidue().GetChain().GetEntity();
  atom::ChemType atype=GetAtomTypeByName(amino_acid_, atom.GetName());
  //FIXME: do not hard-code the distance cutoff here.
  mol::AtomHandleList atoms=e.FindWithin(atom.GetPos(), 20.0);
  mol::AtomHandleList::iterator i;
  for(i=atoms.begin(); i!=atoms.end(); ++i) {
    mol::AtomHandle& a=*i;
    if (abs(a.GetResidue().GetNumber().GetNum()-rnumber_)<3)
      continue;
    AminoAcid b=ResidueToAminoAcid(a.GetResidue());
    if (b==Xxx)
      continue;
    atom::ChemType btype=GetAtomTypeByName(b, a.GetName());
    if (btype==atom::UNKNOWN)
      continue;
    Real d=Length(a.GetPos()-atom.GetPos());
    tmp_energy_+=energies_->Get(atype, btype, d);
  }
  return true; 
}

bool RAPDF::VisitResidue(const mol::ResidueHandle& residue) {
  amino_acid_=ResidueToAminoAcid(residue);
  rnumber_=residue.GetNumber().GetNum();  
  return amino_acid_!=Xxx;
}

}}
