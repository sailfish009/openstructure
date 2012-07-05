#ifndef OST_CONOP_MODEL_CHECK_HH
#define OST_CONOP_MODEL_CHECK_HH

#include <ost/mol/entity_handle.hh>
#include <ost/conop/diag.hh>
#include <ost/conop/compound_lib.hh>
namespace ost { namespace conop {

class DLLEXPORT_OST_CONOP Checker {
public:
	Checker(CompoundLibPtr lib, const mol::EntityHandle& ent,
			    DiagEngine& diags): lib_(lib), ent_(ent), diags_(diags),
	                            checked_unk_res_(false)
  {}
  void CheckForUnknownAtoms();
  void CheckForCompleteness(bool require_hydrogens=false);
  void CheckForNonStandard();
  mol::AtomHandleList GetHydrogens();
  mol::AtomHandleList GetZeroOccupancy();
  
  const std::vector<Diag*>& GetDiags() const {  return diags_.GetDiags(); }
private:
	CompoundLibPtr      lib_;
	mol::EntityHandle   ent_;
	DiagEngine&         diags_;
	bool                checked_unk_res_;
};

}} /* ost::conop */
#endif
