//------------------------------------------------------------------------------
// This file is part of the OpenStructure project <www.openstructure.org>
//
// Copyright (C) 2008-2010 by the OpenStructure authors
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
#ifndef OST_QA_ALL_ATOM_POTENTIAL_HH
#define OST_QA_ALL_ATOM_POTENTIAL_HH
/*
  Author: Marco Biasini
 */
 
#include <boost/shared_ptr.hpp> 
#include <ost/qa/module_config.hh>
#include <ost/qa/interaction_statistics.hh>


namespace ost { namespace qa {

class AllAtomPotential;
typedef boost::shared_ptr<AllAtomPotential> AllAtomPotentialPtr;

/// \brief interaction statistics options
struct DLLEXPORT_OST_QA AllAtomPotentialOpts {

  AllAtomPotentialOpts();

  AllAtomPotentialOpts(float lower_cutoff, float upper_cutoff, float distance_bucket_size, int sequence_sep, float sigma=0.02);

 public: 
  /// \brief weight factor 
  float sigma;
  /// \brief atoms that are closer than the lower cutoff are not considered
  float lower_cutoff;
  /// \brief atoms that are further apart than the upper cutoff are ignored
  float upper_cutoff;
  
  /// \brief distance bucket size
  float distance_bucket_size;
  /// \brief sequence separation
  int sequence_sep;
  
  template <typename DS>
  void Serialize(DS& ds);
};



class DLLEXPORT_OST_QA AllAtomPotential {
public:
  /// \brief calculate new statistical potential from the given statistics 
  ///        and options
  static AllAtomPotentialPtr Create(const InteractionStatisticsPtr& s,
                                    const AllAtomPotentialOpts& o);
                                        
  /// \brief load interaction potential from file                                      
  static AllAtomPotentialPtr LoadFromFile(const String& filename);
  
  /// \brief save interaction potential to file
  void SaveToFile(const String& filename);
  
  /// \brief calculate all-atom interaction score for whole entity
  float GetTotalEnergy(mol::EntityView view);

  /// \brief extract energy of a specific interaction
  /// (for plotting pseudo Lennard-Jones potential).
  float GetEnergy(atom::ChemType type_a, atom::ChemType type_b, 
                  float distance)
  {
    return energies_.Get(type_a, type_b, distance);
  }
  /// \brief calculate all-atom interaction between two entities.
  /// Two entities need to be provided:
  /// the atoms for which the energy should be derived and
  /// the atoms with respect to which the energy should be calculted.
  float GetTotalEnergy(mol::EntityView view, mol::EntityView target_view);

  /// \brief retrieve total number of interactions (for normalisation)
  int GetEnergyCounts() const { return interaction_counts_; }

  /// \brief set different seqeunce separation than used for training
  void SetSequenceSeparation(int seq_sep);

  const AllAtomPotentialOpts& GetOptions() const { return options_; }
  template <typename DS>
  void Serialize(DS& ds);
public:
  void Fill(const InteractionStatisticsPtr& stats);
  
  /// parameters: atom type one, atom type two, distance
  typedef MultiClassifier<float, int, int, float> AllAtomEnergies;
private:
  AllAtomPotentialOpts   options_;  
  AllAtomEnergies        energies_;
  mol::EntityView        target_view_;
  int                    interaction_counts_;
}; 

}}

#endif
