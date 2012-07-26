#ifndef OST_CONOP_RULE_BASED_HH
#define OST_CONOP_RULE_BASED_HH

#include <ost/mol/entity_handle.hh>
#include "compound_lib.hh"
#include "diag.hh"
namespace ost { namespace conop {


class DLLEXPORT_OST_CONOP RuleBasedConop {
public:
  RuleBasedConop(CompoundLibPtr compound_lib): lib_(compound_lib), 
    strict_hydrogens_(false), check_bond_feasibility_(false) {}
  
  void Process(mol::EntityHandle ent);

private:
  bool HasUnknownAtoms(mol::ResidueHandle residue, CompoundPtr compound);
  void ReorderAtoms(mol::ResidueHandle residue, CompoundPtr compound);
  void FillResidueProps(mol::ResidueHandle residue, CompoundPtr compound);
  void ConnectAtomsOfResidue(mol::ResidueHandle residue, CompoundPtr compound);
  void ConnectResidues(mol::ResidueHandle residue, mol::ResidueHandle next);
  bool IsBondFeasible(const mol::AtomHandle&, const mol::AtomHandle&);
  mol::AtomHandle LocateAtom(const mol::AtomHandleList&, int ordinal);
  CompoundLibPtr lib_;
  bool           strict_hydrogens_;
  bool           check_bond_feasibility_;
};

}}
#endif

