#include <ost/mol/entity_handle.hh>
#include <ost/conop/compound_lib.hh>

namespace ost { namespace mol{ namespace alg {

struct MolckSettings;

struct MolckSettings{
  
  bool rm_unk_atoms;
  bool rm_non_std;
  bool rm_hyd_atoms;
  bool rm_oxt_atoms;
  bool rm_zero_occ_atoms;
  bool colored;
  bool map_nonstd_res;
  bool assign_elem;

  MolckSettings():   rm_unk_atoms(false), // Remove unknown and atoms not following the nomenclature
                     rm_non_std(false), // Remove all residues not one of the 20 standard amino acids
                     rm_hyd_atoms(true), // Remove hydrogen atoms
                     rm_oxt_atoms(false), // Remove terminal oxygens
                     rm_zero_occ_atoms(false), // Remove atoms with zero occupancy
                     colored(false), // Whether the output should be colored
                     map_nonstd_res(true), // Map non standard residues back to standard ones (e.g.: MSE->MET,SEP->SER,etc.)
                     assign_elem(true){} // Clean up element column

};

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

void Molck(ost::mol::EntityHandle& ent, ost::conop::CompoundLibPtr lib, const MolckSettings& settings);


}}} // namespace
