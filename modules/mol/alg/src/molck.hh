#include <ost/mol/entity_handle.hh>
#include <ost/io/io_profile.hh>
#include <ost/conop/compound_lib.hh>

namespace ost { namespace mol{ namespace alg{ namespace molck {

struct MolckSettings;
ost::mol::EntityHandle load_x(const String& file, const ost::io::IOProfile& profile);
ost::conop::CompoundLibPtr load_compound_lib(const String& custom_path);
ost::mol::EntityHandle MapNonStandardResidues(ost::mol::EntityHandle& ent, ost::conop::CompoundLibPtr lib);
void RemoveAtoms(ost::mol::EntityHandle& ent,
                 ost::conop::CompoundLibPtr lib,
                 bool rm_unk_atoms,
                 bool rm_non_std,
                 bool rm_hyd_atoms,
                 bool rm_oxt_atoms,
                 bool rm_zero_occ_atoms,
                 bool colored=true);
void CleanUpElementColumn(ost::mol::EntityHandle& ent, ost::conop::CompoundLibPtr lib);

struct MolckSettings{
  
  String rm;
  String color;
  bool map_nonstd_res;
  bool assign_elem;

  MolckSettings():   rm("hyd"), // atoms to be removed
                     color("auto"), // whether the output should be colored
                     map_nonstd_res(true), // map non standard residues back to standard ones (e.g.: MSE->MET,SEP->SER,etc.)
                     assign_elem(true){} // clean up element column

};
}}}} // namespace
