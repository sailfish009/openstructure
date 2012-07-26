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

}}
