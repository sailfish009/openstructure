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
#ifndef OST_QA_REDUCED_STATISTICS_HH
#define OST_QA_REDUCED_STATISTICS_HH

#include <ost/qa/histogram.hh>
#include <ost/qa/amino_acids.hh>
namespace ost { 

namespace mol {

class EntityHandle;

}  
namespace qa {

/// \brief aggregates the option for the reduced model statistics
struct DLLEXPORT_OST_QA ReducedStatOptions {
  ReducedStatOptions(): lower_cutoff(0), upper_cutoff(0), 
                        num_angular_bins(0), num_dist_bins(0), 
                        sequence_sep(0)
  { }
  
  ReducedStatOptions(Real l_cutoff, Real u_cutoff, uint n_ang_bins, 
                     uint n_dist_bins, uint ssep):
    lower_cutoff(l_cutoff), upper_cutoff(u_cutoff), 
    num_angular_bins(n_ang_bins), num_dist_bins(n_dist_bins),
    sequence_sep(ssep)
  { }
  Real lower_cutoff;
  Real upper_cutoff;
  uint num_angular_bins;
  uint num_dist_bins;
  uint sequence_sep;
  
  
  template <typename DS>
  void Serialize(DS& ds)
  {
    ds & lower_cutoff;
    ds & upper_cutoff;
    ds & num_angular_bins;
    ds & num_dist_bins;
    ds & sequence_sep;
  }
};


class ReducedStatistics;
typedef boost::shared_ptr<ReducedStatistics> ReducedStatisticsPtr;


// parametrized as first amino acid, second amino acid, 
// calpha-calpha distance, angle
typedef Histogram<int, int, float, float>  ReducedHistogram;

class DLLEXPORT_OST_QA ReducedStatistics {
public:
  ReducedStatistics(Real l_cutoff, Real u_cutoff, uint num_ang_bins, 
                    uint num_dist_bins, uint ssep):
    opts_(l_cutoff, u_cutoff, num_ang_bins, num_dist_bins, ssep),
    histo_(IntegralClassifier(20, 0), IntegralClassifier(20, 0),
           ContinuousClassifier(num_dist_bins, l_cutoff, u_cutoff),
           ContinuousClassifier(num_ang_bins, 0.0, M_PI))
  { }
  
  const ReducedStatOptions& GetOptions() const { return opts_; }
  
  /// \brief extract the statistics from the given entity (handle)
  void Extract(mol::EntityHandle ent);
  
  /// \brief extract statistics from given entity (view)
  void Extract(mol::EntityView ent);
  
  void Save(const String& filename);
  
  
  static ReducedStatisticsPtr Load(const String& filename);
  /// \internal
  template <typename DS>
  void Serialize(DS& ds);
  
  uint64_t GetTotalCount() const;
  
  uint64_t GetCount(AminoAcid aa_one, AminoAcid aa_two) const;
  
  
  uint64_t GetCount(AminoAcid aa_one, AminoAcid aa_two, int dist_bin, 
                    int ang_bin)
  {
    return histo_.Get(ReducedHistogram::IndexType(aa_one, aa_two, dist_bin, 
                                                  ang_bin));
  }
  
  uint64_t GetCount(int dist_bin, int ang_bin) const;
private:
  ReducedStatistics(): opts_(), histo_() {}
  ReducedStatOptions opts_;
  ReducedHistogram  histo_;
};



}}


#endif
