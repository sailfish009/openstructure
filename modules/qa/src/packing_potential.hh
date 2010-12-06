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
#ifndef OST_QA_PACKING_POTENTIAL_HH
#define OST_QA_PACKING_POTENTIAL_HH

/*
  Author: Marco Biasini
 */


#include <boost/shared_ptr.hpp>

#include <ost/mol/entity_visitor.hh>

#include "packing_statistics.hh"
#include "multi_classifier.hh"

namespace ost { namespace qa {

struct DLLEXPORT_OST_QA PackingPotentialOpts {
  /// \brief initialise with sigma 0.02 and bucket_size 1 distance cutoff 
  ///        2.0 and max_counts 100.
  PackingPotentialOpts();
  
  /// \brief initialise with specified bucket size, max count and sigma
  PackingPotentialOpts(int bucket_size, int max_count, Real cutoff,
                       Real sigma=0.02);
  /// \brief  sigma for smoothing, after Sippl, 1990. Usually set to 0.02
  Real sigma;
  /// \brief  upper distance cutoff
  Real cutoff;
  /// \brief  bin size for counts.
  int    bucket_size;
  /// \brief maximal number of atoms
  int    max_counts;
  
  template <typename DS>
  void Serialize(DS& ds);
};

class PackingPotential;
typedef boost::shared_ptr<PackingPotential> PackingPotentialPtr;


/// \brief packing (solvent accessibility) potential
/// 
/// Not thread safe
class DLLEXPORT_OST_QA PackingPotential : public mol::EntityVisitor {
public:
  typedef MultiClassifier<float, int, int> PackingEnergies;
public:
  PackingPotential();

  /// \brief  create new solvation potential with the given solvation statistics
  ///         and options
  /// If the flag ist set the average energy per is calculated
  /// and shown on stdout. May be used to correct compositional bias in
  /// scoring functions (e.g. Pro and Gly have on average lower torsion energies).
  static PackingPotentialPtr Create(const PackingStatisticsPtr& stat,
                                    const PackingPotentialOpts& opts,
                                    bool calculate_average_energy_flag=0);
                                    
  static PackingPotentialPtr LoadFromFile(const String& filename);
  
  void SaveToFile(const String& filename);
  
  /// \brief extract packing statistics of entity view.
  /// 
  /// See documentation for PackingStatistics::Extract()
  Real GetTotalEnergy(const mol::EntityView& view, 
                      const mol::EntityViewList& views);

  int GetEnergyCounts();
  
  
  Real GetPackingEnergy(AminoAcid aa, int count) const
  {
    count=count>options_.max_counts ? options_.max_counts : count;
    return energies_.Get(aa, count/options_.bucket_size);
  }
  const PackingPotentialOpts& GetOptions() const { return options_; }
  template <typename DS>
  void Serialize(DS& ds);
  
public:
  /// \internal
  virtual bool VisitAtom(const mol::AtomHandle& atom);
private:
  /// \brief potenials are calculated based on the derived statistics
  ///
  /// If the flag ist set the average energy per is calculated
  /// and shown on stdout. May be used to correct compositional bias in
  /// scoring functions (e.g. Pro and Gly have on average lower torsion energies).
  void Fill(const PackingStatisticsPtr& stat,
            bool calculate_average_energy_flag=0);
  
  PackingPotentialOpts options_;
  PackingEnergies      energies_;  
  
  // used to calculate total energy...
  mol::EntityView        view_;
  mol::EntityViewList    views_;  
  Real                   energy_;
  int                    energy_counts_;
};

}}

#endif

