//------------------------------------------------------------------------------
// This file is part of the OpenStructure project <www.openstructure.org>
//
// Copyright (C) 2008-2015 by the OpenStructure authors
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
#ifndef OST_MOL_ALG_LOCAL_DIST_TEST_HH
#define OST_MOL_ALG_LOCAL_DIST_TEST_HH

#include <ost/mol/entity_view.hh>
#include <ost/mol/alg/module_config.hh>
#include <ost/seq/alignment_handle.hh>

namespace ost { namespace mol { namespace alg {
  
/// \brief Contains the infomation needed to uniquely identify an atom in a structure
///
/// Used by the the Local Distance Difference Test classes and functions
class DLLEXPORT_OST_MOL_ALG UniqueAtomIdentifier
{
  
public:
  /// \brief Constructor with all the relevant information  
  UniqueAtomIdentifier(const String& chain,const ResNum& residue,const String& residue_name, const String& atom): chain_(chain),residue_(residue),residue_name_(residue_name),atom_(atom) {}  

  // to make the compiler happy (boost python map suite)
  UniqueAtomIdentifier(): chain_(""),residue_(ResNum(1)),residue_name_(""),atom_("") {}  

  /// \brief Returns the name of the chain to which the atom belongs, as a String  
  String GetChainName() const { return chain_; } 

  /// \brief Returns the ResNum of the residue to which the atom belongs
  ResNum GetResNum() const { return residue_; }  

  /// \brief Returns the name of the residue to which the atom belongs, as a String
  String GetResidueName() const { return residue_name_; }

  /// \brief Returns the name of the atom, as a String
  String GetAtomName() const { return atom_; }

  // required because UniqueAtomIdentifier is used as a key for a std::map  
  bool operator==(const UniqueAtomIdentifier& rhs) const {
    return chain_==rhs.chain_ && residue_==rhs.residue_ && atom_==rhs.atom_;
  }

  // required because UniqueAtomIdentifier is used as a key for a std::map  
  bool operator<(const UniqueAtomIdentifier& rhs) const {
    int cc=chain_.compare(rhs.chain_);
    if (cc) {
      return cc<0;
    }
    if (residue_<rhs.residue_) {
      return true;
    } else if (residue_>rhs.residue_) {
     return false;
    }
    return atom_.compare(rhs.atom_)<0;
  }   
private:

  String chain_;
  ResNum residue_;
  String residue_name_;
  String atom_;    
};

// typedef used to make the code cleaner
typedef std::pair<UniqueAtomIdentifier,UniqueAtomIdentifier> UAtomIdentifiers;

/// \brief Residue distance list. 
///
/// Container for all the interatomic distances that are checked in a Local Distance Difference Test 
/// and are originating from a single specific residue 
typedef std::map<std::pair<UniqueAtomIdentifier,UniqueAtomIdentifier>,std::pair<Real,Real> > ResidueRDMap;

/// \brief Global distance list. 
///
/// Container for all the residue-based interatomic distance lists that are checked in a Local Distance Difference Test
/// and  belong to the same structure
typedef std::map<ost::mol::ResNum,ResidueRDMap> GlobalRDMap;

// used by the multi-reference distance-list generator function
typedef std::map<UniqueAtomIdentifier,int> ExistenceMap;

/// \brief Calculates number of distances conserved in a model, given a list of distances to check and a model
///
/// Calculates the two values needed to determine the Local Distance Difference Test for a given model, i.e.
/// the number of conserved distances in the model and the number of total distances in the reference structure. 
/// The function requires a list of distances to check, a model on which the distances are checked, and a 
/// list of tolerance thresholds that are used to determine if the distances are conserved. 
/// 
/// The function only processes standard residues in the first chains of the model and of the reference
/// For residues with symmetric sidechains (GLU, ASP, ARG, VAL, PHE, TYR), the 
/// naming of the atoms is ambigous. For these residues, the function computes the Local Distance Difference 
/// Test score that each naming convention would generate when considering all non-ambigous surrounding atoms.
/// The solution that gives higher score is then picked to compute the final Local Difference
/// Distance Score for the whole model.
///
/// A sequence separation parameter can be passed to the function. If this happens, only distances between residues
/// whose separation is higher than the provided parameter are considered when computing the score. 
///
/// If a string is provided as an argument to the function, residue-per-residue statistics are stored as 
/// residue properties. Specifically, the local residue-based lddt score is stored in a float property named
/// as the provided string, while the residue-based number of conserved and total distances are saved in two 
/// int properties named <string>_conserved and <string>_total.
std::pair<long int,long int> DLLEXPORT_OST_MOL_ALG LocalDistDiffTest(const EntityView& mdl,
                                         const GlobalRDMap& dist_list,
                                         std::vector<Real> cutoff_list,
                                         int sequence_separation = 0, 
                                         const String& local_ldt_property_string="");

/// \brief Calculates the Local Distance Difference Score for a given model with respect to a given target
///
/// Calculates the Local Distance Difference Test score for a given model with respect to a given reference structure. Requires
/// a model, a reference structure, a list of thresholds that are used to determine if distances are conserved, and an inclusion
/// radius value used to determine which distances are checked.
/// 
/// The function only processes standard residues in the first chains of the model and of the reference
/// For residues with symmetric sidechains (GLU, ASP, ARG, VAL, PHE, TYR), the 
/// naming of the atoms is ambigous. For these residues, the function computes the Local Distance Difference 
/// Test score that each naming convention would generate when considering all non-ambigous surrounding atoms.
/// The solution that gives higher score is then picked to compute the final Local Difference
/// Distance Score for the whole model.
///
/// If a string is provided as an argument to the function, residue-per-residue statistics are stored as 
/// residue properties. Specifically, the local residue-based lddt score is stored in a float property named
/// as the provided string, while the residue-based number of conserved and total distances are saved in two 
/// int properties named <string>_conserved and <string>_total.
Real DLLEXPORT_OST_MOL_ALG LocalDistDiffTest(const EntityView& mdl,
                                         const EntityView& target,
                                         Real cutoff, 
                                         Real max_dist,
                                         const String& local_ldt_property_string="");
/// \brief Calculates the Local Distance Difference Test score for a given model starting from an alignment between a reference structure and the model. 
///
/// Calculates the Local Distance Difference Test score given an alignment between a model and a taget structure.
/// Requires a threshold on which to calculate the score and an inclusion radius to determine the interatiomic 
/// distances to check. Obviously, the strucvtures of the model and the reference must be attached to the alignment.
/// By default the first structure in the alignment is considered the reference and the second is considered the
/// model, but this can be changed by passing to the function the indexes of the two structures in the ref_index
/// and mdl_index parameters.
/// BEWARE: This algorithm uses the old version of the Local Distance Difference Test 
/// (multiple cycles, single threshold, etc. ) and will give a slightly different result than the other functions
Real DLLEXPORT_OST_MOL_ALG LocalDistDiffTest(const ost::seq::AlignmentHandle& aln,
                                         Real cutoff, Real max_dist,
                                         int ref_index=0, int mdl_index=1);

/// \brief Computes the Local Distance Difference High-Accuracy Test given a list of distances to check
///
/// Computes the Local Distance Difference High-Accuracy Test (with threshold 0.5,1,2 and 4 Angstrom)
/// Requires a list of distances to check and a model for which the score is computed
///
/// A sequence separation parameter can be passed to the function. If this happens, only distances between residues
/// whose separation is higher than the provided parameter are considered when computing the score. 
Real DLLEXPORT_OST_MOL_ALG LDDTHA(EntityView& v, const GlobalRDMap& global_dist_list, int sequence_separation=0);

/// \brief Creates a list of distances to check during a Local Difference Distance Test
///
/// Requires a reference structure and an inclusion radius (max_dist)
GlobalRDMap DLLEXPORT_OST_MOL_ALG CreateDistanceList(const EntityView& ref,Real max_dist);

/// \brief Creates a list of distances to check during a Local Difference Distance Test starting from multiple reference structures
///
/// Requires a list of reference structure and an inclusion radius (max_dist).
/// 
/// The structures in the list have to be properly prepared before being passed 
/// to the function. Corresponding residues in the structures must have the same residue number, the same chain name,
/// etc. Gaps are allowed and automatically dealt with: if information about a distance is present in at least one of
/// the structures, it will be considered.  
///
///
/// If a distance between two atoms is shorter than the inclusion radius in all structures in which the two atoms are 
/// present, it is included in the list. However, if the distance is longer than the inclusion radius in at least one 
/// of the structures, it is not be considered a local interaction and is exluded from the list
///
/// The function takes care of residues with ambigous symmetric sidechains. To decide which naming convention to use, the functions
/// computes a local distance score of each reference structure with the first reference structure in the list, using only non ambigously-named atoms. 
/// It picks then the naming convention that gives the highest score, guaranteeing that all references are processed with the correct atom names.
///
/// The cutoff list that will later be used to compute the Local Distance Difference Test score and the  sequence separation parameter 
/// must be passed to the function. These parameters do not influence the output distance list, which always includes all distances
/// within the provided max_dist (to make it consistent with the single-reference corresponding function). However, the parameters are used when
/// dealing with the naming convention of residues with ambiguous nomenclature. 
GlobalRDMap DLLEXPORT_OST_MOL_ALG CreateDistanceListFromMultipleReferences(const std::vector<EntityView>& ref_list,std::vector<Real>& cutoff_list, int sequence_separation, Real max_dist);

/// \brief Prints all distances in a global distance list to standard output
void DLLEXPORT_OST_MOL_ALG PrintGlobalRDMap(const GlobalRDMap& glob_dist_list);

/// \brief Prints all distances in a residue distance list to standard output
void DLLEXPORT_OST_MOL_ALG PrintResidueRDMap(const ResidueRDMap& res_dist_list);

// required by some helper function. Cannot reuse similar functions in other modules without creating
// circular dependencies
bool DLLEXPORT_OST_MOL_ALG IsStandardResidue(String rn);

}}}

#endif


