
#include <string>
#include <ost/mol/entity_handle.hh>
#include <ost/conop/compound_lib.hh>

namespace {
  inline std::string BoolToString(bool b)
    {
      return b ? "True" : "False";
    }
}

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

  MolckSettings(bool init_rm_unk_atoms,
                bool init_rm_non_std,
                bool init_rm_hyd_atoms,
                bool init_rm_oxt_atoms,
                bool init_rm_zero_occ_atoms,
                bool init_colored,
                bool init_map_nonstd_res,
                bool init_assign_elem){
     rm_unk_atoms = init_rm_unk_atoms;
     rm_non_std = init_rm_non_std;
     rm_hyd_atoms = init_rm_hyd_atoms;
     rm_oxt_atoms = init_rm_oxt_atoms;
     rm_zero_occ_atoms = init_rm_zero_occ_atoms;
     colored = init_colored;
     map_nonstd_res = init_map_nonstd_res;
     assign_elem = init_assign_elem;
  }

  public:
    std::string ToString(){
      std::string rep = "MolckSettings(rm_unk_atoms=" + BoolToString(rm_unk_atoms) +
        ", rm_unk_atoms=" + BoolToString(rm_unk_atoms) +
        ", rm_non_std=" + BoolToString(rm_non_std) +
        ", rm_hyd_atoms=" + BoolToString(rm_hyd_atoms) +
        ", rm_oxt_atoms=" + BoolToString(rm_oxt_atoms) +
        ", rm_zero_occ_atoms=" + BoolToString(rm_zero_occ_atoms) +
        ", colored=" + BoolToString(colored) +
        ", map_nonstd_res=" + BoolToString(map_nonstd_res) +
        ", assign_elem=" + BoolToString(assign_elem) +
        ")";
      return rep;
    }

};

ost::mol::EntityHandle MapNonStandardResidues(ost::mol::EntityHandle& ent,
                                              ost::conop::CompoundLibPtr lib);

void RemoveAtoms(ost::mol::EntityHandle& ent,
                 ost::conop::CompoundLibPtr lib,
                 bool rm_unk_atoms,
                 bool rm_non_std,
                 bool rm_hyd_atoms,
                 bool rm_oxt_atoms,
                 bool rm_zero_occ_atoms,
                 bool colored=true);

void CleanUpElementColumn(ost::mol::EntityHandle& ent,
                          ost::conop::CompoundLibPtr lib);

void Molck(ost::mol::EntityHandle& ent,
           ost::conop::CompoundLibPtr lib,
           const MolckSettings& settings);


}}} // namespace
