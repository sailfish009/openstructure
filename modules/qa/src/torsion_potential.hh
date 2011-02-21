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
#ifndef OST_QA_TORSION_POTENTIAL_HH
#define OST_QA_TORSION_POTENTIAL_HH
/*
  This implementation of the torsion angle potential combines 3
  torsion angle pairs with 1 residues in analogy to the combined
  potential described by:
  Pascal Benkert, Silvio C. E. Tosatto, Dietmar Schomburg
  QMEAN: A comprehensive scoring function for model quality assessment
  Proteins, 2008

  Author: Pascal, Benkert, Marco Biasini
*/
#include <ost/qa/torsion_statistics.hh>
#include <boost/shared_ptr.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <vector>
#include <boost/scoped_ptr.hpp>

namespace ost { namespace qa {

typedef std::vector<AminoAcidSet> AminoAcidAlphabet;

/// \brief  torsion potential options
/// 
///  These options may be used to configure the different torsion potentials
/// based on a sliding window of 3 amino acids.
struct DLLEXPORT_OST_QA TorsionPotentialOpts {
  /// \brief initialize torsion potential with angular bucket size of 30 and
  ///        sigma of 0.02
  /// 
  /// The amino acid alphabets are empty.
  TorsionPotentialOpts();

  /// angular bucket size, 360 % angular_bucket_size must be zero
  int                 prev_angular_bucket_size;
  int                 central_angular_bucket_size;
  int                 next_angular_bucket_size;
  /// alphabet of the central. The amino acids in the set must be mutually excl.
  AminoAcidAlphabet   alphabet;
  /// weighting factor after (Sippl et al., 1990), usually 0.02
  Real              sigma;
  /// \internal
  template <typename DS>
  void Serialize(DS& ds);
};

class TorsionPotential;
typedef boost::shared_ptr<TorsionPotential> TorsionPotentialPtr;

/// \brief Torsion potential
/// 
/// The torsion potential class is parametrisable by TorsionPotentialOpts.
class DLLEXPORT_OST_QA TorsionPotential : 
   public boost::enable_shared_from_this<TorsionPotential> {
public:
  /// \brief  create new torsion potential with the given torsion statistics 
  ///         and options
  /// If the flag ist set the average energy per is calculated
  /// and shown on stdout. May be used to correct compositional bias in
  /// scoring functions (e.g. Pro and Gly have on average lower torsion energies).
  static TorsionPotentialPtr Create(TorsionStatisticsPtr statistics,
                                    const TorsionPotentialOpts& opts,
                                    bool calculate_average_energy_flag);
  /// \brief  Load potential from file.
  static TorsionPotentialPtr LoadFromFile(const String& path);

  /// \brief  Calculate energy for entity.
  Real GetTotalEnergy(mol::EntityHandle entity);
  
  /// \brief  Calculate energy for entity view. Only residues in the view are
  ///         considered.
  Real GetTotalEnergy(mol::EntityView view);

  /// \brief retrieve total number of energy local (i.e. valid residues)
  int GetEnergyCounts() const;
  
  
  
  Real GetTorsionEnergy(AminoAcid central_aa, Real prev_phi, Real prev_psi,
                        Real central_phi, Real central_psi,  
                        Real next_phi, Real next_psi) const {
    int icenter=this->GetAAIndex(central_aa);
    return energies_.Get(icenter, prev_phi, prev_psi,
                         central_phi, central_psi,
                         next_phi, next_psi);
  }
                        

  /// \brief save torsion potential
  /// 
  /// The output is in a non-portable binary form.
  void SaveToFile(const String& path);  
  
  /// \internal
  template <typename DS>
  void Serialize(DS& ds);
  

public:
  /// Meaning of parameters: AA before AA center AA after, 
  /// phi torsion angle, psi torsion angle
  typedef MultiClassifier<float, int, Real, Real,
                          Real, Real, Real, Real> TorsionEnergies;
private:
  
  int GetAAIndex(AminoAcid aa) const;
  
  void Fill(const TorsionStatisticsPtr& stat,
            bool calculate_average_energy_flag);

  TorsionPotentialOpts               options_;
  TorsionEnergies                    energies_;  
  int                                num_torsions_;
};


}}

#endif
