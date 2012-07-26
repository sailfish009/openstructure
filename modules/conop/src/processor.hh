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
#ifndef OST_CONOP_PROCESSOR_HH
#define OST_CONOP_PROCESSOR_HH

#include <ost/mol/entity_handle.hh>
#include "module_config.hh"
#include "diag.hh"

namespace ost { namespace conop {

enum ConopAction {
  CONOP_WARN = 0,
  CONOP_SILENT,
  CONOP_REMOVE,
  CONOP_REMOVE_ATOM,
  CONOP_REMOVE_RESIDUE,
  CONOP_FATAL
};

struct ProcessorOptions {
  ProcessorOptions(): check_bond_feasibility(false), assign_torsions(false),
    connect(true), unk_atom_treatment(CONOP_WARN), unk_res_treatment(CONOP_WARN),
    zero_occ_treatment(CONOP_SILENT)
  { }
  bool check_bond_feasibility;
  bool assign_torsions;
  bool connect;
  ConopAction unk_atom_treatment;
  ConopAction unk_res_treatment;
  ConopAction zero_occ_treatment;
};

// the base class for all options
class DLLEXPORT_OST_CONOP Processor {
public:
  DiagnosticsPtr Process(mol::EntityHandle ent) const;


  virtual void PushFlags() = 0;
  virtual void PopFlags() = 0;
protected:
  virtual void DoProcess(DiagnosticsPtr diags, 
                         mol::EntityHandle ent) const = 0;
  virtual bool BeginProcessing(DiagnosticsPtr diags, 
                               mol::EntityHandle ent) const { return true; }  
  virtual bool EndProcessing(DiagnosticsPtr diags, 
                             mol::EntityHandle ent) const { return true; }
};

// Provides accessor methods for the basic Processor options. All 
// processors should derive from this class
template <typename O> 
class ProcessorBase  : public Processor {
public:
  typedef O option_type;
  ProcessorBase(): option_stack_(1, O())
  {}

  void SetConnect(bool connect) {
    this->GetOptions().connect = connect;
  }

  bool GetConnect() const {
    return this->GetOptions().connect;
  }
  void SetAssignTorsions(bool flag) {
    this->GetOptions().assign_torsions = flag;
  }
  bool GetAssignTorsions() const {
    return this->GetOptions().assign_torsions;
  }
  ConopAction GetUnkResidueTreatment() const {
    return this->GetOptions().unk_res_treatment;
  }

  ConopAction GetUnkAtomTreatment() const {
    return this->GetOptions().unk_atom_treatment;
  }

  bool GetCheckBondFeasibility() const {
    return this->GetOptions().check_bond_feasibility;
  }

  bool GetStrictHydrogens() const {
    return this->GetOptions().strict_hydrogens;
  }

  void SetStrictHydrogens(bool flag) {
    this->GetOptions().strict_hydrogens = flag;
  }

  void SetCheckBondFeasibility(bool flag) {
    this->GetOptions().check_bond_feasibility = flag;
  }

  void SetUnkResidueTreatment(ConopAction action) {
    this->GetOptions().unk_res_treatment = action;
  }

  void SetUnkAtomTreatment(ConopAction action) {
    this->GetOptions().unk_atom_treatment = action;
  }

  ConopAction GetZeroOccTreatment() const {
    return this->GetOptions().zero_occ_treatment;
  }

  void SetZeroOccTreatment(ConopAction action) {
    this->GetOptions().zero_occ_treatment = action;
  }

  virtual void PushFlags() {
    option_stack_.push_back(option_stack_.back());
  }

  virtual void PopFlags() {
    if (option_stack_.size() == 1) {
      throw std::runtime_error("Can't pop from stack with one item left");
    }
    option_stack_.pop_back();
  }
protected:
  const option_type& GetOptions() const { return option_stack_.back(); }
  option_type& GetOptions() { return option_stack_.back(); }
private:
  std::vector<option_type> option_stack_;
};


/// \brief assigns phi/psi/omega to all residues marked peptide-linking of 
///     the chain
/// 
/// Requires the atoms to be connected
void DLLEXPORT_OST_CONOP AssignBackboneTorsions(mol::ChainHandle chain);
void DLLEXPORT_OST_CONOP AssignBackboneTorsions(mol::ResidueHandleList residues);
void DLLEXPORT_OST_CONOP AssignBackboneTorsions(mol::ResidueHandle prev, 
                                                mol::ResidueHandle res, 
                                                mol::ResidueHandle next);

}}

#endif
