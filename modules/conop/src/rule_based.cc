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
#include <ost/log.hh>
#include <ost/profile.hh>
#include <ost/mol/xcs_editor.hh>
#include <ost/mol/bond_handle.hh>
#include <ost/mol/torsion_handle.hh>
#include <ost/mol/impl/residue_impl.hh>
#include <ost/mol/impl/atom_impl.hh>
#include <ost/mol/residue_handle.hh>
#include "rule_based.hh"

namespace ost { namespace conop {

struct OrdinalAtomComp {
  bool operator()(const mol::impl::AtomImplPtr& a,
                  const mol::impl::AtomImplPtr& b) const {
    return a->GetState()<b->GetState();
  }
};


void RuleBasedProcessor::DoProcess(DiagnosticsPtr diags, 
                                   mol::EntityHandle ent) const
{
  Profile prof("RuleBasedProcessor::Process");
  mol::ChainHandleList chains=ent.GetChainList();
  for (mol::ChainHandleList::iterator 
       i = chains.begin(), e = chains.end(); i!= e; ++i) {
    mol::ChainHandle& chain = *i;
    mol::ResidueHandleList residues = chain.GetResidueList();
    mol::ResidueHandle prev;
    for (mol::ResidueHandleList::iterator 
         j = residues.begin(), e2 = residues.end(); j != e2; ++j) {
      mol::ResidueHandle residue = *j;
      CompoundPtr compound = lib_->FindCompound(residue.GetName(), Compound::PDB);
      if (!compound) {
        // process unknown residue...
        this->ProcessUnkResidue(diags, residue);
        continue;
      }
      this->ReorderAtoms(residue, compound);  
      bool unks = this->HasUnknownAtoms(residue, compound);
      if (unks) {
        mol::AtomHandleList unk_atoms;
        unk_atoms = GetUnknownAtomsOfResidue(residue, compound, 
                                             this->GetStrictHydrogens());
        this->ProcessUnkAtoms(diags, unk_atoms);
        residue.SetChemClass(mol::ChemClass(mol::ChemClass::UNKNOWN));
        residue.SetChemType(mol::ChemType(mol::ChemType::UNKNOWN));
        residue.SetOneLetterCode('?');
        continue;
      }
      this->FillResidueProps(residue, compound);
      if (this->GetConnect()) {
        this->ConnectAtomsOfResidue(residue, compound);
        this->ConnectResidues(prev, residue);
      }
      prev = residue;
    }
    if (residues.empty() || !this->GetAssignTorsions()) {
      continue;
    }
    AssignBackboneTorsions(residues);
  }
}

void RuleBasedProcessor::ConnectResidues(mol::ResidueHandle rh,
                                         mol::ResidueHandle next) const
{
  if (!next.IsValid() || !rh.IsValid()) {
    return;
  }

  mol::XCSEditor e=rh.GetEntity().EditXCS(mol::BUFFERED_EDIT);

  // check if both of the residues are able to form a peptide bond.
  if (rh.IsPeptideLinking() && next.IsPeptideLinking()) {
    // If we have an OXT then there is no peptide bond connecting the two
    // residues.
    if (rh.FindAtom("OXT"))
      return;
    mol::AtomHandle c=rh.FindAtom("C");
    mol::AtomHandle n=next.FindAtom("N");
    // Give subclasses a chance to give us their opinions on the feasibility of
    // the peptide bond.
    if (c.IsValid() && n.IsValid() && IsBondFeasible(c, n)) {
      e.Connect(c, n, 1);
      rh.SetIsProtein(true);
      next.SetIsProtein(true);
    }
  } else if (rh.IsNucleotideLinking() && next.IsNucleotideLinking()) {
    mol::AtomHandle c=rh.FindAtom("O3'");
    mol::AtomHandle n=next.FindAtom("P");
    if (c.IsValid() && n.IsValid() && IsBondFeasible(c, n)) {
      e.Connect(c, n, 1);
    }
  }
}

void RuleBasedProcessor::FillResidueProps(mol::ResidueHandle residue,
                                      CompoundPtr compound) const
{
  residue.SetChemClass(compound->GetChemClass());
  residue.SetChemType(compound->GetChemType());
  residue.SetOneLetterCode(compound->GetOneLetterCode());
}

mol::AtomHandle RuleBasedProcessor::LocateAtom(const mol::AtomHandleList& ahl, 
                                               int ordinal) const
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


void RuleBasedProcessor::ConnectAtomsOfResidue(mol::ResidueHandle rh, 
                                               CompoundPtr compound) const
{

  //if (!compound) {
  //  dist_connect(this, rh.GetAtomList());
  //  return;
  //}
  //if (unknown_atoms_) {
  //  dist_connect(this, rh.GetAtomList());
  //  return;
  //}
  mol::XCSEditor e=rh.GetEntity().EditXCS(mol::BUFFERED_EDIT);
  BondSpecList::const_iterator j=compound->GetBondSpecs().begin();
  mol::AtomHandleList atoms=rh.GetAtomList();
  for(; j!=compound->GetBondSpecs().end(); ++j) {
      const BondSpec& bond=*j;
      mol::AtomHandle a1=this->LocateAtom(atoms, bond.atom_one);
      mol::AtomHandle a2=this->LocateAtom(atoms, bond.atom_two);
      if (a1.IsValid() && a2.IsValid()) { 
        if (!this->GetCheckBondFeasibility()) {
          if (this->GetStrictHydrogens() && (a1.GetElement()=="H" || 
                                             a2.GetElement()=="D")) {
            continue;
          }
          e.Connect(a1, a2, bond.order);
        } else { 
          if (IsBondFeasible(a1, a2)) {
            if (this->GetStrictHydrogens() && (a1.GetElement()=="H" || 
                                               a2.GetElement()=="D")) {
              continue;
            }
            e.Connect(a1, a2, bond.order);
          }
        }
      }
  }
}


void RuleBasedProcessor::ReorderAtoms(mol::ResidueHandle residue, 
                                      CompoundPtr compound) const
{
  mol::impl::ResidueImplPtr impl=residue.Impl();
  mol::impl::AtomImplList::iterator i=impl->GetAtomList().begin();
  for (; i!=impl->GetAtomList().end(); ++i) {
    mol::impl::AtomImplPtr atom=*i;
    atom->SetState(std::numeric_limits<unsigned int>::max());
    int index=compound->GetAtomSpecIndex(atom->GetName());
    if (index==-1) {
      atom->SetState(std::numeric_limits<unsigned int>::max());
      continue;
    }
    atom->SetState((compound->GetAtomSpecs())[index].ordinal);
    // override element
    if (this->GetFixElement()) {
      atom->SetElement((compound->GetAtomSpecs())[index].element);
    }
  }
  std::sort(impl->GetAtomList().begin(), impl->GetAtomList().end(),
            OrdinalAtomComp());
}

bool RuleBasedProcessor::HasUnknownAtoms(mol::ResidueHandle res, 
                                         CompoundPtr compound) const 
{
  AtomSpecList::const_iterator j=compound->GetAtomSpecs().begin();
  mol::AtomHandleList atoms=res.GetAtomList();
  mol::AtomHandleList::iterator i=atoms.begin();
  for (mol::AtomHandleList::iterator 
       i=atoms.begin(), e=atoms.end(); i!=e; ++i) {
    if ((*i).Impl()->GetState()==std::numeric_limits<unsigned int>::max()) {
      if (((*i).GetElement()=="H" || (*i).GetElement()=="D") && 
          !this->GetStrictHydrogens()) {
        continue;
      }
      return true;
    }
  }
  return false;
}

mol::AtomHandleList GetUnknownAtomsOfResidue(mol::ResidueHandle res, 
                                             CompoundPtr compound,
                                             bool strict_hydrogens)
{
  mol::AtomHandleList atoms=res.GetAtomList();
  mol::AtomHandleList unks;
  const AtomSpecList& atom_specs=compound->GetAtomSpecs();
  for (mol::AtomHandleList::const_iterator
        j=atoms.begin(), e2=atoms.end(); j!=e2; ++j) {
    bool found=false;
    for (AtomSpecList::const_iterator
          k=atom_specs.begin(), e3=atom_specs.end(); k!=e3; ++k) {
      if (k->name==j->GetName() || k->alt_name==j->GetName()) {
        found=true;
        break;
      }
    }
    if (!found) {
      unks.push_back(*j);
    }
  }
  return unks;
}

}}
