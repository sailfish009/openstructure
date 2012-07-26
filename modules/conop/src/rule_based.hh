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
#ifndef OST_CONOP_RULE_BASED_HH
#define OST_CONOP_RULE_BASED_HH

#include <ost/mol/entity_handle.hh>
#include "compound_lib.hh"
#include "diag.hh"
#include "processor.hh"

namespace ost { namespace conop {

struct RuleBasedProcessorOptions : ProcessorOptions {
  RuleBasedProcessorOptions(): ProcessorOptions(), 
    strict_hydrogens(false), fix_element(true)
  {}
  bool strict_hydrogens;
  bool fix_element;
};

/// \brief returns all atoms not listed in  the specifictaion of compound
mol::AtomHandleList DLLEXPORT_OST_CONOP GetUnknownAtoms(mol::ResidueHandle res, 
                                                        CompoundPtr compound);

class DLLEXPORT_OST_CONOP RuleBasedProcessor  : 
  public ProcessorBase<RuleBasedProcessorOptions> {
public:
  RuleBasedProcessor(CompoundLibPtr compound_lib): 
    ProcessorBase<RuleBasedProcessorOptions>(), lib_(compound_lib)
  {
  }

  bool GetStrictHydrogens() const {
    return this->GetOptions().strict_hydrogens;
  }

  void SetStrictHydrogens(bool flag) {
    this->GetOptions().strict_hydrogens = flag;
  }

  bool GetFixElement() const {
    return this->GetOptions().fix_element;
  }
  void SetFixElement(bool flag) {
    this->GetOptions().fix_element = flag;
  }
protected:
  virtual void DoProcess(DiagnosticsPtr diags, 
                         mol::EntityHandle ent) const;
private:
  void ProcessUnkResidue(DiagnosticsPtr diags,
                         mol::ResidueHandle res) const;
  void ProcessUnkAtoms(DiagnosticsPtr diags,
                       mol::AtomHandleList unks) const;
  bool HasUnknownAtoms(mol::ResidueHandle residue, CompoundPtr compound) const;
  void ReorderAtoms(mol::ResidueHandle residue, CompoundPtr compound) const;
  void FillResidueProps(mol::ResidueHandle residue, CompoundPtr compound) const;
  void ConnectAtomsOfResidue(mol::ResidueHandle residue, CompoundPtr compound) const;
  void ConnectResidues(mol::ResidueHandle residue, mol::ResidueHandle next) const;
  bool IsBondFeasible(const mol::AtomHandle&, const mol::AtomHandle&) const;
  mol::AtomHandle LocateAtom(const mol::AtomHandleList&, int ordinal) const;
  CompoundLibPtr lib_;
};


mol::AtomHandleList DLLEXPORT_OST_CONOP 
GetUnknownAtomsOfResidue(mol::ResidueHandle residue, 
                         CompoundPtr compound,
                         bool strict_hydrogens=false);

}}
#endif

