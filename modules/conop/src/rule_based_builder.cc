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
   Author: Marco Biasini
 */
#include <ost/log.hh>
#include <ost/geom/geom.hh>
#include <ost/mol/impl/atom_impl.hh>
#include <ost/mol/impl/residue_impl.hh>
#include <ost/conop/conop.hh>
#include <limits>
#include "rule_based_builder.hh"


namespace ost { namespace conop {


void RuleBasedBuilder::CompleteAtoms(mol::ResidueHandle rh) {}

void RuleBasedBuilder::CheckResidueCompleteness(const mol::ResidueHandle& rh) 
{
  this->LookupCompound(rh);
  if (!last_compound_) {
    mol::AtomHandleList atoms=rh.GetAtomList();    
    for (mol::AtomHandleList::const_iterator i=atoms.begin(), 
         e=atoms.end(); i!=e; ++i) {
      this->OnUnknownAtom(*i);
    }
    return;
  }
  this->ReorderAtoms(rh , last_compound_);
  AtomSpecList::const_iterator j=last_compound_->GetAtomSpecs().begin();
  mol::AtomHandleList atoms=rh.GetAtomList();
  mol::AtomHandleList::iterator i=atoms.begin();
  for (; j!=last_compound_->GetAtomSpecs().end() && i!=atoms.end(); ++j) {
    if ((*j).is_leaving || (*j).element=="H" || (*j).element=="D")
      continue;
    if ((*j).ordinal!=static_cast<int>((*i).Impl()->GetState())) {
      this->OnMissingAtom(rh, (*j).name);
    } else {
      ++i;
    }
  }
}

bool RuleBasedBuilder::HasUnknownAtoms(mol::ResidueHandle res)
{
  this->LookupCompound(res);
  if (!last_compound_) {
    return true;
  }
  this->ReorderAtoms(res, last_compound_);
  AtomSpecList::const_iterator j=last_compound_->GetAtomSpecs().begin();
  mol::AtomHandleList atoms=res.GetAtomList();
  mol::AtomHandleList::iterator i=atoms.begin();
  for (mol::AtomHandleList::iterator 
       i=atoms.begin(), e=atoms.end(); i!=e; ++i) {
    if ((*i).Impl()->GetState()==std::numeric_limits<unsigned int>::max()) {
      if (((*i).GetElement()=="H" || (*i).GetElement()=="D") && 
          this->GetStrictHydrogenMode()==false) {
        continue;
      }
      return true;
    }
  }
  return false;
}

void RuleBasedBuilder::FillAtomProps(mol::AtomHandle atom, const AtomSpec& spec) 
{
  Conopology& conop_inst=Conopology::Instance();
  if (!conop_inst.IsValidElement(atom.GetElement())) {
    atom.SetElement(spec.element);
  } 
}

void RuleBasedBuilder::FillResidueProps(mol::ResidueHandle residue) 
{
  this->LookupCompound(residue);
  if (!last_compound_)
    return;
  residue.SetChemClass(last_compound_->GetChemClass());
  residue.SetOneLetterCode(last_compound_->GetOneLetterCode());
};

struct OrdinalComp {
  bool operator()(const mol::impl::AtomImplPtr& a,
                  const mol::impl::AtomImplPtr& b) const {
    return a->GetState()<b->GetState();
  }
};


void RuleBasedBuilder::LookupCompound(const mol::ResidueHandle& rh) 
{
  Compound::Dialect dialect=this->GetDialect()==PDB_DIALECT ? Compound::PDB : Compound::CHARMM;  
  if ((last_compound_) && (rh.GetName()==last_compound_->GetID())) {
   return;
  }
  last_compound_=compound_lib_->FindCompound(rh.GetName(), dialect);
  if (!last_compound_ && this->GetDialect()!=PDB_DIALECT) {
    last_compound_=compound_lib_->FindCompound(rh.GetName(), Compound::PDB);
  }
}

void RuleBasedBuilder::ReorderAtoms(mol::ResidueHandle residue,
                                    CompoundPtr compound) 
{
  if (last_residue_==residue) {
    return;
  }
  mol::impl::ResidueImplPtr impl=residue.Impl();
  mol::impl::AtomImplList::iterator i=impl->GetAtomList().begin();
  for (; i!=impl->GetAtomList().end(); ++i) {
    mol::impl::AtomImplPtr atom=*i;
    int index=compound->GetAtomSpecIndex(atom->GetName());
    if (index==-1) {
      if (!this->OnUnknownAtom(mol::AtomHandle(atom))) {
        atom->SetState(std::numeric_limits<unsigned int>::max());
      }
      continue;
    }
    atom->SetState((compound->GetAtomSpecs())[index].ordinal);
  }
  std::sort(impl->GetAtomList().begin(), impl->GetAtomList().end(),
            OrdinalComp());
  last_residue_=residue;
  unknown_atoms_=this->HasUnknownAtoms(residue);
  if (unknown_atoms_) {
    LOG_WARNING("residue " << residue << " doesn't look like a standard " 
                << residue.GetKey() << " (" << compound->GetFormula() << ")");
    residue.SetChemClass(mol::ChemClass(mol::ChemClass::UNKNOWN));
    residue.SetOneLetterCode('?');
  }
}



mol::ResidueKey RuleBasedBuilder::IdentifyResidue(const mol::ResidueHandle& rh) 
{
  LookupCompound(rh);
  if (last_compound_) {
    return last_compound_->GetID();
  } else {
    return "UNK";
  }
}

mol::AtomHandle RuleBasedBuilder::LocateAtom(const mol::AtomHandleList& ahl, 
                                             int ordinal) 
{
  if (ahl.empty())
    return mol::AtomHandle();
  const mol::AtomHandle* r_it=&ahl.back();
  if (static_cast<int>(ahl.size())>ordinal) {
    r_it=&ahl.front()+ordinal;
  }
  while ((r_it>=&ahl.front()) &&
         (static_cast<int>(r_it->Impl()->GetState())>ordinal)) {
    --r_it;
  }
  bool not_found=(r_it<&ahl.front() ||
                 static_cast<int>(r_it->Impl()->GetState())!=ordinal);
  return  not_found ? mol::AtomHandle() : *r_it;
}


inline void dist_connect(RuleBasedBuilder* b, mol::AtomHandleList atoms)
{
  for (mol::AtomHandleList::const_iterator i=atoms.begin(), 
       e=atoms.end(); i!=e; ++i) {
    b->DistanceBasedConnect(*i);
  }  
}
void RuleBasedBuilder::ConnectAtomsOfResidue(mol::ResidueHandle rh) 
{

  LookupCompound(rh);
    
  if (!last_compound_) {
    dist_connect(this, rh.GetAtomList());
    return;
  }
  this->ReorderAtoms(rh, last_compound_);
  if (unknown_atoms_) {
    dist_connect(this, rh.GetAtomList());
    return;
  }
  mol::XCSEditor e=rh.GetEntity().EditXCS(mol::BUFFERED_EDIT);
  BondSpecList::const_iterator j=last_compound_->GetBondSpecs().begin();
  mol::AtomHandleList atoms=rh.GetAtomList();
  for(; j!=last_compound_->GetBondSpecs().end(); ++j) {
      const BondSpec& bond=*j;
      mol::AtomHandle a1=this->LocateAtom(atoms, bond.atom_one);
      mol::AtomHandle a2=this->LocateAtom(atoms, bond.atom_two);
      if (a1.IsValid() && a2.IsValid() && this->IsBondFeasible(a1, a2)) {
        if (this->GetStrictHydrogenMode() && 
            (a1.GetElement()=="H" || a2.GetElement()=="D")) {
          continue;
        }
        e.Connect(a1, a2, bond.order);
      }
  }
  for (mol::AtomHandleList::iterator i=atoms.begin(), e=atoms.end(); i!=e; ++i) {
    if (((*i).GetElement()=="H" || (*i).GetElement()=="D") && 
        (*i).GetBondCount()==0) {
      this->DistanceBasedConnect(*i);
    }
  }
}

void RuleBasedBuilder::ConnectResidueToNext(mol::ResidueHandle rh,
                                            mol::ResidueHandle next) 
{
  if (!next.IsValid()) {
    return;
  }
  Compound::Dialect dialect=this->GetDialect()==PDB_DIALECT ? Compound::PDB : Compound::CHARMM;
  mol::XCSEditor e=rh.GetEntity().EditXCS(mol::BUFFERED_EDIT);
  CompoundPtr mc=compound_lib_->FindCompound(rh.GetName(), dialect);
  CompoundPtr nc=compound_lib_->FindCompound(next.GetName(), dialect);
  if (!(mc && nc))
    return;
  // check if both of the residues are able to form a peptide bond.
  if (!mc->IsPeptideLinking())
    return;
  if (!nc->IsPeptideLinking())
    return;
  // If we have an OXT then there is no peptide bond connecting the two
  // residues.
  if (rh.FindAtom("OXT"))
    return;
  mol::AtomHandle c=rh.FindAtom("C");
  mol::AtomHandle n=next.FindAtom("N");
  // Give subclasses a chance to give us their opinions on the feasibility of
  // the peptide bond.
  if (c.IsValid() && n.IsValid() && this->DoesPeptideBondExist(c, n)) {
    e.Connect(c, n, 1);
    rh.SetIsProtein(true);
    next.SetIsProtein(true);
  }
}


void RuleBasedBuilder::AssignTorsions(mol::ChainHandle chain) 
{
  if (chain.GetResidueCount()==0)
    return;
  std::vector<mol::ResidueHandle> rlist = chain.GetResidueList();
  mol::AtomHandleList atom_list = rlist[0].GetAtomList();
  for(unsigned int ri=0;ri<rlist.size();++ri) {
    mol::ResidueHandle res=rlist[ri];
    this->AssignTorsionsToResidue(res);
  }
}

void RuleBasedBuilder::AssignTorsionsToResidue(mol::ResidueHandle residue) 
{
  /// The only components having named torsions are the standard set of amino
  /// acids, plus some of compounds derived from them such as selenium
  /// methionine. Things are simplified a lot by only storing the torsions
  /// of the side chains in the database. PHI, PSI and OMEGA torsions are    
  /// checked without a lookup in the database.
  LookupCompound(residue);
  if (!last_compound_)
    return;  
  if (!last_compound_->IsPeptideLinking()) {
    return;
  }
  Builder::AssignBackBoneTorsionsToResidue(residue);
}

bool RuleBasedBuilder::IsResidueComplete(const mol::ResidueHandle& residue)
{
  LookupCompound(residue);
  if (!last_compound_)
    return false;
  mol::AtomHandleList atoms=residue.GetAtomList();
  mol::AtomHandleList::iterator i=atoms.begin();
  for (AtomSpecList::const_iterator j=last_compound_->GetAtomSpecs().begin(),
       e=last_compound_->GetAtomSpecs().end(); j!=e; ++j) {
    if ((*j).is_leaving || (*j).element=="H" || (*j).element=="D") {
      continue;      
    }
    if (!(residue.FindAtom(j->name) || residue.FindAtom(j->alt_name))) {
      return false;
    }
  }  
  return true;
}


void RuleBasedBuilder::FillAtomProps(mol::AtomHandle atom)
{
  LookupCompound(atom.GetResidue());
  if (!last_compound_) {
    this->OnUnknownAtom(atom);
    return;
  }
  int index=last_compound_->GetAtomSpecIndex(atom.GetName());
  if (index==-1) {
    this->OnUnknownAtom(atom);
    return;
  }
  const AtomSpec& atom_spec=last_compound_->GetAtomSpecs()[index];
  this->FillAtomProps(atom, atom_spec);
}

bool RuleBasedBuilder::OnUnknownAtom(mol::AtomHandle atom)
{
  Conopology& conop_inst=Conopology::Instance();
  if (!conop_inst.IsValidElement(atom.GetElement())) {
    atom.SetElement(Builder::GuessAtomElement(atom.GetName(), atom.IsHetAtom()));
  }
  return false;
}

}}
