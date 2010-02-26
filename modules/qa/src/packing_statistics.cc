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
#include "packing_statistics.hh"

#include <ost/log.hh>
#include <ost/geom/geom.hh>
#include <ost/mol/mol.hh>
#include <ost/io/binary_data_source.hh>
#include <ost/io/binary_data_sink.hh>
#include <ost/io/container_serialization.hh>

namespace ost { namespace qa {

PackingStatistics::PackingStatistics(Real cutoff,
                                     int max_counts,
                                     int bucket_size)
 : cutoff_(cutoff),
   max_counts_(max_counts),
   bucket_size_(bucket_size),
   histogram_(IntegralClassifier(Xxx, 0),       // Xxx is the last object in enum and therefore corresponds to the total number of atom types
              IntegralClassifier((max_counts/bucket_size)+1, 0)) {      // since zero counts is possible, we have n+1 bins
   if (max_counts%bucket_size!=0) {
     LOGN_MESSAGE("initialising packing statistics with max_count="
                  << max_counts << "which is not divisible by bucket_size="
                  << bucket_size << "without remainder. Will continue anyway.");
   }
}


PackingStatistics::PackingStatistics():
 cutoff_(0.0),
 max_counts_(0),
 bucket_size_(0),
 histogram_()
{
}


void PackingStatistics::Extract(mol::EntityView view, const mol::EntityViewList& views)
{
  view_=view;
  views_=views;
  view.Apply(*this);
}

bool PackingStatistics::VisitAtom(const mol::AtomHandle& atom) {
  AminoAcid aa=ResidueToAminoAcid(atom.GetResidue());
  if (aa==Xxx)
    return false;
  int count=0;
  for (mol::EntityViewList::iterator i=views_.begin(),
       e=views_.end(); i!=e; ++i) {
    count+=i->FindWithin(atom.GetPos(), cutoff_).size();
  }
  count=count>=max_counts_ ? max_counts_ : count;       // if more counts observed than maximum expected -> set to maximum
  histogram_.Add(1, aa, count/bucket_size_);
  return false;
}

int PackingStatistics::GetBucketSize() const {
  return bucket_size_;
}

int PackingStatistics::GetMaxCounts() const {
  return max_counts_;
}

//! \brief Get cutoff radius of sphere around CB.
Real PackingStatistics::GetCutoffDistance() const {
  return cutoff_;
}

uint64_t PackingStatistics::GetCount(AminoAcid aa, int counts) const {
  return histogram_.Get(aa, counts/bucket_size_);
}

uint64_t PackingStatistics::GetCount(AminoAcid aa) const {
  uint64_t b=(max_counts_/bucket_size_)+1;
  uint64_t total=0;
  for (uint64_t i=0; i<b;++i) {
    total+=histogram_.Get(aa, i);
  }
  return total;
}

PackingStatisticsPtr PackingStatistics::LoadFromFile(const String& filename)
{
  std::ifstream stream(filename.c_str(),  std::ios_base::binary);
  io::BinaryDataSource ds(stream);
  PackingStatisticsPtr ptr(new PackingStatistics);
  ds >> *ptr.get();
  return ptr;
}

void PackingStatistics::SaveToFile(const String& filename)
{
  std::ofstream stream(filename.c_str(),  std::ios_base::binary);
  io::BinaryDataSink ds(stream);
  ds << *this;
}

uint64_t PackingStatistics::GetTotalCount() const
{
  uint64_t b=(max_counts_/bucket_size_)+1;
  uint64_t total=0;
  for (uint64_t i=0; i<Xxx; ++i) {
    for (uint64_t j=0; j<b;++j) {
      total+=histogram_.Get(i, j);
    }
  }
  return total;
}

uint64_t PackingStatistics::GetCount(int count) const
{
  uint64_t total=0;
  for (uint64_t i=0; i<Xxx; ++i) {
    total+=histogram_.Get(i, count/bucket_size_);
  }
  return total;
}



}}
