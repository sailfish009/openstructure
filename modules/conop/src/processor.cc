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
#include <ost/mol/xcs_editor.hh>
#include <ost/mol/bond_handle.hh>
#include <ost/mol/torsion_handle.hh>
#include "processor.hh"

namespace ost { namespace conop {

DiagnosticsPtr Processor::Process(mol::EntityHandle ent) const
{
  DiagnosticsPtr diags(new Diagnostics);
  if (!this->BeginProcessing(diags, ent)) {
    return diags;
  }
  this->DoProcess(diags, ent);
  
  this->EndProcessing(diags, ent);
  return diags;
}

void AssignBackboneTorsions(mol::ChainHandle chain)
{
  AssignBackboneTorsions(chain.GetResidueList());
}

void AssignBackboneTorsions(mol::ResidueHandleList residues)
{
  if (residues.empty()) { return; }
  mol::ResidueHandle r1;
  mol::ResidueHandle r2;
  mol::ResidueHandle r3 = residues.front();
  for (mol::ResidueHandleList::iterator 
        j = residues.begin()+1, e2 = residues.end(); j != e2; ++j) {
    r1 = r2;
    r2 = r3;
    r3 = *j;
    AssignBackboneTorsions(r1, r2, r3); 
  }
  AssignBackboneTorsions(r2, r3, mol::ResidueHandle());
}


void AssignBackboneTorsions(mol::ResidueHandle prev, 
                            mol::ResidueHandle res, 
                            mol::ResidueHandle next)
{

  mol::XCSEditor e=res.GetEntity().EditXCS(mol::BUFFERED_EDIT);
  //psi
  if (next.IsValid() && next.IsPeptideLinking()){
    mol::AtomHandle ca_this=res.FindAtom("CA");
    mol::AtomHandle n_this=res.FindAtom("N");
    mol::AtomHandle c_this=res.FindAtom("C");
    mol::AtomHandle n_next=next.FindAtom("N");
    if ((ca_this && n_this && c_this && n_next &&  BondExists(c_this, n_next))
        && !res.GetPsiTorsion()) {
      e.AddTorsion("PSI", n_this, ca_this, c_this, n_next);
    }
  };
  //phi
  if (prev.IsValid() && prev.IsPeptideLinking()) {
    mol::AtomHandle c_prev=prev.FindAtom("C");
    mol::AtomHandle n_this=res.FindAtom("N");
    mol::AtomHandle ca_this=res.FindAtom("CA");
    mol::AtomHandle c_this=res.FindAtom("C");
    if ((c_prev && n_this && ca_this && c_this && BondExists(c_prev, n_this))
        && !res.GetPhiTorsion()) {
      e.AddTorsion("PHI", c_prev, n_this, ca_this, c_this);
    }
  }
  //omega
  if (prev.IsValid() && prev.IsPeptideLinking()) {
    mol::AtomHandle ca_prev=prev.FindAtom("CA");
    mol::AtomHandle c_prev=prev.FindAtom("C");
    mol::AtomHandle n=res.FindAtom("N");
    mol::AtomHandle ca=res.FindAtom("CA");
    if ((ca_prev && c_prev && n && ca && BondExists(c_prev, n))
        && !res.GetOmegaTorsion()) {
      e.AddTorsion("OMEGA",ca_prev , c_prev, n, ca);
    }
  }
} 

void Processor::ProcessUnkResidue(DiagnosticsPtr diags,
                                  mol::ResidueHandle res) const
{
  // Don't do anything if treatment is set to SILENT
  if (this->GetUnkResidueTreatment() == CONOP_SILENT) {
    return;
  }
  switch (this->GetUnkResidueTreatment()) {
    case CONOP_WARN:
      diags->AddDiag(DIAG_UNK_RESIDUE, "unknown residue %0")
            .AddResidue(res);
      break;
    case CONOP_FATAL:
      // FIXME: Implement a ConopError based on Diag...
      break;
    case CONOP_REMOVE_RESIDUE:
    case CONOP_REMOVE:
      diags->AddDiag(DIAG_UNK_RESIDUE, "removed unknown residue %0")
           .AddResidue(res);
      res.GetEntity().EditXCS().DeleteResidue(res);
      return;
    default:
      assert(0 && "unhandled switch");
  }
}

void Processor::ProcessUnkAtoms(DiagnosticsPtr diags,
                                mol::AtomHandleList unks) const
{
  // Don't do anything if treatment is set to SILENT
  if (this->GetUnkAtomTreatment() == CONOP_SILENT) {
    return;
  }
 
  assert(!unks.empty() && "empty unk list");
  mol::XCSEditor edi = unks.front().GetEntity().EditXCS();
  for (mol::AtomHandleList::iterator 
       i = unks.begin(), e = unks.end(); i != e; ++i) {
    switch (this->GetUnkAtomTreatment()) {
      case CONOP_REMOVE_ATOM:
      case CONOP_REMOVE:
        edi.DeleteAtom(*i);
        diags->AddDiag(DIAG_UNK_ATOM, "removed unknown atom %0 from residue %1")
             .AddString(i->GetName()).AddResidue(i->GetResidue());
        break;
      case CONOP_WARN:
        diags->AddDiag(DIAG_UNK_ATOM, "residue %0 contains unknown atom %1")
             .AddResidue(i->GetResidue()).AddString(i->GetName());
        break;
      case CONOP_FATAL:
        // FIXME: Implement a ConopError based on Diag...
        break;
      case CONOP_REMOVE_RESIDUE:
        diags->AddDiag(DIAG_UNK_ATOM, "removed residue %0 with unknown atom %1")
             .AddString(i->GetResidue().GetQualifiedName())
             .AddString(i->GetName());
        edi.DeleteResidue(i->GetResidue());
        return;
      default:
        assert(0 && "unhandled switch");
    }
  }
}

bool IsBondFeasible(const mol::AtomHandle& atom_a,
                    const mol::AtomHandle& atom_b)
{
  Real radii=0.0;
  if (atom_a.GetRadius()>0.0) {
    radii=atom_a.GetRadius();
  } else {
    return false;
  }
  if (atom_b.GetRadius()>0.0) {
    radii+=atom_b.GetRadius();
  } else {
    return false;
  } 
  Real len=geom::Length2(atom_a.GetPos()-atom_b.GetPos());
  Real lower_bound=radii*radii*0.0625;
  Real upper_bound=lower_bound*6.0;
  return (len<=upper_bound && len>=lower_bound);
}

}}
