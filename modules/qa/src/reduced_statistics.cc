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

#include <ost/log.hh>
#include <ost/mol/mol.hh>
#include "amino_acids.hh"
#include "reduced_statistics.hh"
#include "impl/reduced_impl.hh"

#include <ost/io/binary_data_source.hh>
#include <ost/io/binary_data_sink.hh>
#include <ost/io/container_serialization.hh>

namespace ost { namespace qa {


namespace {

class ReducedStatExtractor : public impl::ReducedPotentialImpl {
public:
  
  ReducedStatExtractor(const ReducedStatOptions& opts, ReducedHistogram& histo,
                       mol::EntityHandle ent): 
    impl::ReducedPotentialImpl(opts, ent),
    histo_(histo)
  { }
  
  ReducedStatExtractor(const ReducedStatOptions& opts, ReducedHistogram& histo,
                       mol::EntityView ent): 
    impl::ReducedPotentialImpl(opts, ent),
    histo_(histo)
  { }
  
  virtual void OnInteraction(AminoAcid aa_one, AminoAcid aa_two, 
                             Real dist, Real angle)
  {
    histo_.Add(1, aa_one, aa_two, dist, angle);
  }
private:
  ReducedHistogram&   histo_;
};


}

uint64_t ReducedStatistics::GetTotalCount() const
{
  typedef ReducedHistogram::IndexType Index;
  uint64_t count=0;
  for (size_t i=0; i<Xxx; ++i) {
    for (size_t j=0; j<Xxx; ++j) {
      for (size_t k=0; k<opts_.num_dist_bins; ++k) {
        for (size_t l=0; l<opts_.num_angular_bins; ++l) {
          count+=histo_.Get(Index(i, j, k, l));
        }
      }
    }
  }
  return count;
}

uint64_t ReducedStatistics::GetCount(AminoAcid aa_one, AminoAcid aa_two) const
{
  typedef ReducedHistogram::IndexType Index;
  uint64_t count=0;
  for (size_t k=0; k<opts_.num_dist_bins; ++k) {
    for (size_t l=0; l<opts_.num_angular_bins; ++l) {
      count+=histo_.Get(Index(aa_one, aa_two, k, l));
    }
  }
  return count;
}

uint64_t ReducedStatistics::GetCount(int dist_bin, int ang_bin) const
{
  typedef ReducedHistogram::IndexType Index;
  uint64_t count=0;
  for (size_t i=0; i<Xxx; ++i) {
    for (size_t j=0; j<Xxx; ++j) {
      count+=histo_.Get(Index(i, j, dist_bin, ang_bin));
    }
  }
  return count;
}

void ReducedStatistics::Save(const String& filename)
{
  std::ofstream stream(filename.c_str(), std::ios_base::binary);
  io::BinaryDataSink ds(stream);
  ds << *this;
}


ReducedStatisticsPtr ReducedStatistics::Load(const String& filename)
{
  std::ifstream stream(filename.c_str(), std::ios_base::binary);
  io::BinaryDataSource ds(stream);
  ReducedStatisticsPtr p(new ReducedStatistics);
  ds >> *p.get();
  return p;
}

template <typename DS>
void ReducedStatistics::Serialize(DS& ds)
{
  ds & opts_;
  ds & histo_;
}


void ReducedStatistics::Extract(mol::EntityHandle ent)
{
  if (ent.GetChainCount()!=1) {
    std::stringstream ss;
    ss << "Expected exactly one chain, but entity has " 
       <<  ent.GetChainCount() << " chains";
    throw std::runtime_error(ss.str());
  }
  ReducedStatExtractor extractor(opts_, histo_, ent);
  ent.Apply(extractor);
}

void ReducedStatistics::Extract(mol::EntityView ent)
{
  if (ent.GetChainCount()!=1) {
    std::stringstream ss;
    ss << "Expected exactly one chain, but entity has " 
       <<  ent.GetChainCount() << " chains";
    throw std::runtime_error(ss.str());
  }
  ReducedStatExtractor extractor(opts_, histo_, ent.GetHandle());
  ent.Apply(extractor);
}


}}