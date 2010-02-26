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
#ifndef OST_QA_SOLIS_TORSION_POTENTIAL_HH
#define OST_QA_SOLIS_TORSION_POTENTIAL_HH
/*


  Author: Marco Biasini, Pascal Benkert
 */
#include <ost/qa/solis_torsion_statistics.hh>
#include <boost/shared_ptr.hpp>
#include <vector>
#include <boost/scoped_ptr.hpp>

namespace ost { namespace qa {

typedef std::vector<AminoAcidSet> AminoAcidAlphabet;

/// \brief  torsion potential options
/// 
///  These options may be used to configure the different torsion potentials
/// based on a sliding window of 3 amino acids.
struct DLLEXPORT_OST_QA TorsionPotentialOptsSolis {
  /// \brief initialize torsion potential with angular bucket size of 30 and
  ///        sigma of 0.02
  /// 
  /// The amino acid alphabets are empty.
  TorsionPotentialOptsSolis();

  /// angular bucket size, 360 % angular_bucket_size must be zero
  int                 angular_bucket_size;
  /// alphabet of residues before and after the central amino acids in the
  /// window. The amino acids in the set must be mutually exclusive
  AminoAcidAlphabet   outer_alphabet;
  /// alphabet of the central amino acid. The amino acid sets must be 
  /// mutually exclusive
  AminoAcidAlphabet   inner_alphabet;
  /// weighting factor after (Sippl et al., 1990), usually 0.02
  Real              sigma;
  /// \internal
  template <typename DS>
  void Serialize(DS& ds);
};

class TorsionPotentialSolis;
typedef boost::shared_ptr<TorsionPotentialSolis> TorsionPotentialSolisPtr;

/// \brief Torsion potential
/// 
/// The torsion potential class is parametrisable by TorsionPotentialOptsSolis.
/// 
/// This implementation of the torsion angle potential combines 1 torsion angle 
/// pair with up to 3 residues in analogy to the potential described by:
/// Armando D. Solis, S. Rackovsky
/// Optimally informative backbone structural propensities in proteins Proteins, 
/// 2002 The ordinary trosion angle potential (1 angle-pair, 1 residue) is 
/// covered as well.
class DLLEXPORT_OST_QA TorsionPotentialSolis {
public:
  /// \brief  create new torsion potential with the given torsion statistics 
  ///         and options
  static TorsionPotentialSolisPtr Create(TorsionStatisticsSolisPtr statistics,
                                    const TorsionPotentialOptsSolis& opts);
  /// \brief  Load potential from file.
  static TorsionPotentialSolisPtr LoadFromFile(const String& path);

  /// \brief  Calculate energy for entity.
  Real GetTotalEnergy(mol::EntityHandle entity);
  
  /// \brief  Calculate energy for entity view. Only residues in the view are
  ///         considered.
  Real GetTotalEnergy(mol::EntityView view);

    /// \brief retrieve total number of energy local (i.e. valid residues)
  int GetEnergyCounts() const;

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
  typedef MultiClassifier<float, int, int,
                          int, Real, Real> TorsionEnergies;
private:
  void FillAll(const TorsionStatisticsSolisPtr& stat);
  
  void FillPhiPsi(const TorsionStatisticsSolisPtr& stat, int phi, int psi, 
                  uint32_t total);
  TorsionPotentialOptsSolis               options_;
  TorsionEnergies                    energies_;  
  int                                num_torsions_;
};


}}

#endif
