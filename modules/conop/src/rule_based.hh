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

/// \brief returns all atoms not listed in  the specifictaion of compound
mol::AtomHandleList DLLEXPORT_OST_CONOP GetUnknownAtoms(mol::ResidueHandle res, 
                                                        CompoundPtr compound);

class RuleBasedProcessor;

typedef boost::shared_ptr<RuleBasedProcessor> RuleBasedProcessorPtr;

class DLLEXPORT_OST_CONOP RuleBasedProcessor  : public Processor {
public:
  RuleBasedProcessor(CompoundLibPtr compound_lib): 
    lib_(compound_lib)
  {
  }

  bool GetFixElement() const {
    return fix_element_;
  }
  void SetFixElement(bool flag) {
    fix_element_ = flag;
  }
  virtual ProcessorPtr Copy() const {
    return ProcessorPtr(new RuleBasedProcessor(*this));
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
  bool fix_element_;
};


mol::AtomHandleList DLLEXPORT_OST_CONOP 
GetUnknownAtomsOfResidue(mol::ResidueHandle residue, 
                         CompoundPtr compound,
                         bool strict_hydrogens=false);

}}
#endif

