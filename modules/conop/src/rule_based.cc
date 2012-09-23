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
#include <limits>
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
      this->ReorderAtoms(residue, compound, this->GetFixElement());  
      bool unks = this->HasUnknownAtoms(residue, compound, 
                                        this->GetStrictHydrogens());
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
        this->ConnectAtomsOfResidue(residue, compound, 
                                    this->GetStrictHydrogens());
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

void RuleBasedProcessor::ProcessUnkResidue(DiagnosticsPtr diags,
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

void RuleBasedProcessor::ProcessUnkAtoms(DiagnosticsPtr diags,
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




}}
