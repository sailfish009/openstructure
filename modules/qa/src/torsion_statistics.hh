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
#ifndef OST_QA_TORSION_STATISTICS_HH
#define OST_QA_TORSION_STATISTICS_HH
/*
  Author: Pascal, Benkert, Marco Biasini
*/
#include <ost/mol/entity_handle.hh>
#include <ost/qa/histogram.hh>
#include <ost/qa/amino_acids.hh>
#include <boost/shared_ptr.hpp>

namespace ost { namespace qa {

class TorsionStatistics;
typedef boost::shared_ptr<TorsionStatistics> TorsionStatisticsPtr;

/// \brief sequence dependent torsion statistics
/// 
/// This implementation of the torsion angle potential combines 3 torsion angle 
/// pairs with 1 residues in analogy to the combined potential described by:
/// Pascal Benkert, Silvio C. E. Tosatto, Dietmar Schomburg QMEAN: A 
/// comprehensive scoring function for model quality assessment Proteins, 2008
class DLLEXPORT_OST_QA TorsionStatistics {
public:
  /// construct new torsion statistics. In order to get something useful
  /// you need to either extract statistics from a set of PDBs or the
  /// statistics from file.
  /// 
  /// \param prev_torsion_bucket_size is torsion bucket size for the first 
  ///    torsion
  /// \param central_torsion_bucket_size is the bucket size for the second 
  ///    torsion
  /// \param next_torsion_bucket_size is the bucket size for the third torsion
  TorsionStatistics(int prev_torsion_bucket_size, int central_torsion_bucket_size, 
                    int next_torsion_bucket_size);
  TorsionStatistics();
  /// \brief extract torsion counts from whole entity.
  void Extract(mol::EntityHandle entity);
  
  /// \brief extract torsion counts from (filtered) entity
  void Extract(mol::EntityView view);
  
  /// \brief load torsion statistics from file
  static TorsionStatisticsPtr LoadFromFile(const String& file_name);

  void SaveToFile(const String& file_name) const;
  
  /// \brief  Get count of phi/psi torsions with the specified torsion
  ///         bucket size away conditional on the central amino acid.
  uint64_t GetCount(const AminoAcidSet& central_aa,
                  Real prev_phi_angle, Real prev_psi_angle,
                  Real central_phi_angle, Real central_psi_angle,
                  Real next_phi_angle, Real next_psi_angle) const;
                  
  uint64_t GetCount(Real prev_phi_angle, Real prev_psi_angle,
                  Real central_phi_angle, Real central_psi_angle,
                  Real next_phi_angle, Real next_psi_angle) const;

  uint64_t GetCount(const AminoAcidSet& central_aa) const;

  uint64_t GetCount() const;

//  /// \brief  Get frequency of the amino acid aa.
//  /// \return frequency in the range of [0, 1]
//  Real GetFrequency(AminoAcid aa) const;
  

  /// \brief  get the number of torsion buckets for every of the torsion
  ///         angles.
  uint32_t GetTorsionBucketCountPrev() const;
  int GetTorsionBucketSizePrev() const;
  uint32_t GetTorsionBucketCountCentral() const;
  int GetTorsionBucketSizeCentral() const;
  uint32_t GetTorsionBucketCountNext() const;
  int GetTorsionBucketSizeNext() const;


  /// \internal
  template <typename DS>
  void Serialize(DS& ds)
  {
    ds & prev_torsion_bucket_size_;
    ds & central_torsion_bucket_size_;
    ds & next_torsion_bucket_size_;
    ds & histogram_;
  }
public:

  typedef MultiClassifier<uint32_t, int, Real, Real,
                          Real, Real, Real, Real> TorsionHistogram;
  typedef TorsionHistogram::IndexType               IndexType;


private:
  /// \brief  Convenience function to add counts in the inclusive range
  ///         [start, end]
  uint64_t Accumulate(const IndexType& start, const IndexType& end) const;
  
  uint32_t IndexForAnglePrev(Real angle) const;
  uint32_t IndexForAngleCentral(Real angle) const;
  uint32_t IndexForAngleNext(Real angle) const;

  TorsionHistogram  histogram_;
  int               prev_torsion_bucket_size_;
  int               central_torsion_bucket_size_;
  int               next_torsion_bucket_size_;
};
}}

#endif


