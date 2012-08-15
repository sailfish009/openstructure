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

typedef enum {
  PDB_DIALECT,
  CHARMM_DIALECT
} Dialect;

enum ConopAction {
  CONOP_WARN = 0,
  CONOP_SILENT,
  CONOP_REMOVE,
  CONOP_REMOVE_ATOM,
  CONOP_REMOVE_RESIDUE,
  CONOP_FATAL
};

class Processor;
typedef boost::shared_ptr<Processor> ProcessorPtr;
// the base class for all options
class DLLEXPORT_OST_CONOP Processor {
public:
  DiagnosticsPtr Process(mol::EntityHandle ent) const;
  virtual ProcessorPtr Copy() const = 0;
protected:
  virtual void DoProcess(DiagnosticsPtr diags, 
                         mol::EntityHandle ent) const = 0;
  virtual bool BeginProcessing(DiagnosticsPtr diags, 
                               mol::EntityHandle ent) const { return true; }  
  virtual bool EndProcessing(DiagnosticsPtr diags, 
                             mol::EntityHandle ent) const { return true; }
  void ProcessUnkResidue(DiagnosticsPtr diags,
                         mol::ResidueHandle res) const;
  void ProcessUnkAtoms(DiagnosticsPtr diags,
                       mol::AtomHandleList unks) const;
public:
  Processor(): strict_hydrogens_(false), check_bond_feasibility_(false),
    assign_torsions_(false), connect_(true), unk_atom_treatment_(CONOP_WARN),
    unk_res_treatment_(CONOP_WARN), zero_occ_treatment_(CONOP_SILENT) {}
  void SetConnect(bool connect) {
    connect_ = connect;
  }

  bool GetConnect() const {
    return connect_;
  }
  void SetAssignTorsions(bool flag) {
    assign_torsions_ = flag;
  }
  bool GetAssignTorsions() const {
    return assign_torsions_;
  }
  ConopAction GetUnkResidueTreatment() const {
    return unk_res_treatment_;
  }

  ConopAction GetUnkAtomTreatment() const {
    return unk_atom_treatment_;
  }

  bool GetCheckBondFeasibility() const {
    return check_bond_feasibility_;
  }

  bool GetStrictHydrogens() const {
    return strict_hydrogens_;
  }

  void SetStrictHydrogens(bool flag) {
    strict_hydrogens_ = flag;
  }

  void SetCheckBondFeasibility(bool flag) {
    check_bond_feasibility_ = flag;
  }

  void SetUnkResidueTreatment(ConopAction action) {
    unk_res_treatment_ = action;
  }

  void SetUnkAtomTreatment(ConopAction action) {
    unk_atom_treatment_ = action;
  }

  ConopAction GetZeroOccTreatment() const {
    return zero_occ_treatment_;
  }

  void SetZeroOccTreatment(ConopAction action) {
    zero_occ_treatment_ = action;
  }
private:
  bool strict_hydrogens_;
  bool check_bond_feasibility_;
  bool assign_torsions_;
  bool connect_;
  ConopAction unk_atom_treatment_;
  ConopAction unk_res_treatment_;
  ConopAction zero_occ_treatment_;
};

ConopAction DLLEXPORT_OST_CONOP ConopActionFromString(const String& name);


/// \brief assigns phi/psi/omega to all residues marked peptide-linking of 
///     the chain
/// 
/// Requires the atoms to be connected
void DLLEXPORT_OST_CONOP AssignBackboneTorsions(mol::ChainHandle chain);
void DLLEXPORT_OST_CONOP AssignBackboneTorsions(mol::ResidueHandleList residues);
void DLLEXPORT_OST_CONOP AssignBackboneTorsions(mol::ResidueHandle prev, 
                                                mol::ResidueHandle res, 
                                                mol::ResidueHandle next);

bool DLLEXPORT_OST_CONOP IsBondFeasible(const mol::AtomHandle&, 
                                        const mol::AtomHandle&);
}}

#endif
