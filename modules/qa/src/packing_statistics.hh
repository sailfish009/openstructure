//------------------------------------------------------------------------------
// This file is part of the OpenStructure project <www.openstructure.org>
//
// Copyright (C) 2008-2011 by the OpenStructure authors
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
#ifndef OST_QA_PACKING_STATISTICS_HH
#define OST_QA_PACKING_STATISTICS_HH

#include <ost/mol/entity_visitor.hh>
#include <ost/qa/histogram.hh>
#include <ost/qa/amino_acids.hh>
#include <ost/mol/entity_view.hh>

namespace ost { namespace qa {

/// \brief two dimensional histogram
/// the meaning of the histogram axes is: amino acid type, number of
/// atoms inside of cutoff radius.
typedef Histogram<int, int>  PackingHistogram;
class PackingStatistics;
typedef boost::shared_ptr<PackingStatistics> PackingStatisticsPtr;
class DLLEXPORT_OST_QA PackingStatistics : public mol::EntityVisitor {
public:
  PackingStatistics(Real cutoff, int max_counts, int bucket_size);
  PackingStatistics();
  
  /// \brief Get bucket size for counts, i.e. 2 indicates that a residue with
  ///         6 and 7 seven CB within the cutoff radius have the same packing.
  int GetBucketSize() const;
  
  /// \brief Get maximal count of CB atoms inside cutoff distance. larger
  ///         values are truncated.
  int GetMaxCounts() const;
  
  /// \brief Get cutoff radius of sphere around CB.
  Real GetCutoffDistance() const;
  
  /// \brief extract packing statistics of entity view.
  /// 
  /// For each peptide residue in view, the total number of CB (CA for glycine)
  /// in views are counted.
  void Extract(mol::EntityView view, const mol::EntityViewList& views);
  
  static PackingStatisticsPtr LoadFromFile(const String& filename);
  
  void SaveToFile(const String& filename);
  
  virtual bool VisitAtom(const mol::AtomHandle& atom);
  
  uint64_t GetCount(AminoAcid aa) const;
  uint64_t GetCount(AminoAcid aa, int count) const;
  
  uint64_t GetTotalCount() const;
  uint64_t GetCount(int count) const;

  template <typename DS>
  void Serialize(DS& ds)
  {
    ds & cutoff_;
    ds & max_counts_;
    ds & bucket_size_;
    ds & histogram_;
  }

  Real            cutoff_;
  int               max_counts_;
  int               bucket_size_;
  
private:
  mol::EntityView        view_;
  mol::EntityViewList    views_;
  PackingHistogram  histogram_;
};

}}

#endif
