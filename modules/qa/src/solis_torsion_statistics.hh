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
#ifndef OST_QA_SOLIS_TORSION_STATISTICS_HH
#define OST_QA_SOLIS_TORSION_STATISTICS_HH
/*
  This implementation of the torsion angle potential combines 1
  torsion angle pair with up to 3 residues in analogy to the
  potential described by:
  Armando D. Solis, S. Rackovsky
  Optimally informative backbone structural propensities in proteins
  Protei
  The ordinary trosion angle potential (1 angle-pair, 1 residue) is covered
  as well.

  Author: Marco Biasini, Pascal Benkert
 */
#include <ost/mol/entity_handle.hh>
#include <ost/qa/module_config.hh>
#include <ost/qa/histogram.hh>
#include <ost/qa/amino_acids.hh>
#include <boost/shared_ptr.hpp>

namespace ost { namespace qa {

class TorsionStatisticsSolis;
typedef boost::shared_ptr<TorsionStatisticsSolis> TorsionStatisticsSolisPtr;

/// \brief sequence dependent torsion statistics
class DLLEXPORT_OST_QA TorsionStatisticsSolis {
public:
  /// construct new torsion statistics. In order to get something useful
  /// you need to either extract statistics from a set of PDBs or the
  /// statistics from file.
  /// 
  /// \param   torsion_bucket_size is the histogram bucket size for torsion
  ///          angles.
  TorsionStatisticsSolis(int torsion_bucket_size);
  TorsionStatisticsSolis();
  /// \brief extract torsion counts from whole entity.
  void Extract(mol::EntityHandle entity);
  
  /// \brief extract torsion counts from (filtered) entity
  void Extract(mol::EntityView view);
  
  /// \brief load torsion statistics from file
  static TorsionStatisticsSolisPtr LoadFromFile(const String& file_name);

  void SaveToFile(const String& file_name) const;
  
  /// \brief  Get count of phi/psi torsions with the specified torsion
  ///         bucket size away conditional on the central, previous
  ///         and next amino acid. 
  uint32_t GetCount(const AminoAcidSet& prev_aa, 
                  const AminoAcidSet& central_aa,
                  const AminoAcidSet& next_aa,
                  Real phi_angle, Real psi_angle) const;
                  
  uint32_t GetCount(const AminoAcidSet& prev_aa, 
                  const AminoAcidSet& central_aa,
                  const AminoAcidSet& next_aa) const;                  

  /// \brief Get count of amino acids with the given phi/psi angle.
  uint32_t GetCount(Real phi_angle, Real psi_angle) const;

  /// \brief  Get count of amino acids with the given phi/psi angle
  ///         conditional on the central residue.
  uint32_t GetCount(AminoAcid central_aa, 
                  Real phi_angle, Real psi_angle) const;
                
  /// \brief  Get frequency of phi/psi angle in bucket phi_angle/psi_angle when
  ///         the amino acid is aa.
  /// \return frequency in the range of [0, 1]
  Real GetFrequency(AminoAcid aa, Real phi_angle, Real psi_angle) const;
  
  /// \brief  Get frequency of phi/psi angle in bucket phi_angle/psi_angle
  ///         regardless of the amino acid type.
  /// \return frequency in the range of [0, 1]
  Real GetFrequency(Real phi_angle, Real psi_angle) const;  
  
  /// \brief  get the number of torsion buckets for every of the torsion
  ///         angles.
  uint32_t GetTorsionBucketCount() const;
  int GetTorsionBucketSize() const;
  
  int GetCount() const;
  /// \internal
  template <typename DS>
  void Serialize(DS& ds)
  {
    ds & torsion_bucket_size_;
    ds & histogram_;
  }
public:
  typedef MultiClassifier<int, int, int, int, Real, Real> TorsionHistogram;
  typedef TorsionHistogram::IndexType               IndexType;
private:
  /// \brief  Convenience function to add counts in the inclusive range
  ///         [start, end]
  uint32_t Accumulate(const IndexType& start, const IndexType& end) const;
  
  uint32_t IndexForAngle(Real angle) const;
  

  TorsionHistogram  histogram_;
  int               torsion_bucket_size_;
};
}}

#endif


