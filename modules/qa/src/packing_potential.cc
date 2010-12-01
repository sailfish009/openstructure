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
#include "packing_potential.hh"
#include <ost/io/binary_data_source.hh>
#include <ost/io/binary_data_sink.hh>
#include <ost/io/container_serialization.hh>
#include <boost/filesystem/convenience.hpp>
#include <ost/message.hh>
#include <sstream>
#include <ost/io/io_exception.hh>
#include <ost/integrity_error.hh>

/*
  Authors: Marco Biasini, Pascal Benkert
 */

namespace ost { namespace qa {


PackingPotentialOpts::PackingPotentialOpts():
  sigma(0.02), cutoff(10.0), bucket_size(1), max_counts(25)
{

}


PackingPotentialOpts::PackingPotentialOpts(int b, int m, Real c, Real s):
  sigma(s), cutoff(c), bucket_size(b), max_counts(m)
{

}

template <typename DS>
void PackingPotentialOpts::Serialize(DS& ds)
{
  ds & sigma;
  ds & cutoff;
  ds & bucket_size;
  ds & max_counts;
}


PackingPotential::PackingPotential()
{

}

void PackingPotential::Fill(const PackingStatisticsPtr& stat, bool calculate_average_energy_flag)
{
  typedef PackingEnergies::IndexType Index;
  int buckets=(options_.max_counts/options_.bucket_size)+1; // +1 since zero counts are in the first bin
  uint64_t M=stat->GetTotalCount();
  for (int i=0; i < Xxx; ++i) {
    uint64_t Mi=stat->GetCount(AminoAcid(i));

    if (calculate_average_energy_flag==1) std::cout << AminoAcid(i) << " " << float(Mi)/M  << " ";
    Real per_aa_energy = 0; // for average energy

    for (int j=0; j<buckets; ++j) {
      uint64_t fxi=stat->GetCount(AminoAcid(i), j*options_.bucket_size);
      // could be moved out of the loop and cached for more speed. at the
      // moment I just don't care...
      uint64_t fx=stat->GetCount(j*options_.bucket_size);

      // propensity = (fxi/Mi)/(fx/M)
      Real propensity=0.0;
      //avoid division by zero:
      if (fx != 0 && Mi != 0 && M != 0) {
        propensity=Real(Real(fxi)/Mi)/(Real(fx)/M);
      }

      Real e=log(1+options_.sigma*Mi)-
               log(1+options_.sigma*Mi*propensity);

      per_aa_energy=per_aa_energy+fxi*e; // for average energy

      energies_.Set(Index(i, j), e);
    }
    if (calculate_average_energy_flag==1) std::cout << per_aa_energy/Mi  << std::endl;
  }
}

PackingPotentialPtr PackingPotential::Create(const PackingStatisticsPtr& stat,
                                             const PackingPotentialOpts& opts,
                                             bool calculate_average_energy_flag)
{
//  stat->bucket_size_=opts.bucket_size;
//  stat->max_counts_=opts.max_counts;
//  stat->cutoff_=opts.cutoff;

  if (stat->GetBucketSize() != opts.bucket_size) {
    std::stringstream error_message;
    error_message << "BucketSize limit specified in opts does not agree with those of the statistics file: " << opts.bucket_size << " vs " << stat->GetBucketSize();
    throw IntegrityError(error_message.str());
  }

  if (stat->GetMaxCounts() != opts.max_counts) {
    std::stringstream error_message;
    error_message << "MaxCounts limit specified in opts does not agree with those of the statistics file: " << opts.max_counts << " vs " << stat->GetMaxCounts();
    throw IntegrityError(error_message.str());
  }

  if (stat->GetCutoffDistance() != opts.cutoff) {
    std::stringstream error_message;
    error_message << "CutoffDistance limit specified in opts does not agree with those of the statistics file: " << opts.cutoff << " vs " << stat->GetCutoffDistance();
    throw IntegrityError(error_message.str());
  }

  PackingPotentialPtr p(new PackingPotential);
  p->options_=opts;

  p->options_.cutoff=opts.cutoff;
  p->options_.bucket_size=opts.bucket_size;
  p->options_.max_counts=opts.max_counts;
  IntegralClassifier counts((opts.max_counts/opts.bucket_size)+1, 0); // +1 since zero counts are in the first bin
  p->energies_=PackingEnergies(0.0, IntegralClassifier(Xxx, 0), counts);
  p->Fill(stat, calculate_average_energy_flag);
  return p;
}

PackingPotentialPtr PackingPotential::LoadFromFile(const String& filename)
{
  if(!boost::filesystem::exists(filename))
  throw io::IOException("Could not open packing potential data file.\nFile does not exist at: "+filename);

  PackingPotentialPtr p(new PackingPotential);
  std::ifstream stream(filename.c_str(), std::ios_base::binary);
  io::BinaryDataSource ds(stream);
  ds >> *p.get();
  return p;
}

void PackingPotential::SaveToFile(const String& filename)
{
  std::ofstream stream(filename.c_str(), std::ios_base::binary);
  io::BinaryDataSink ds(stream);
  ds << *this;
}

template <typename DS>
void PackingPotential::Serialize(DS& ds)
{
  ds & options_;
  ds & energies_;
}


Real PackingPotential::GetTotalEnergy(const mol::EntityView& view,
                                        const mol::EntityViewList& views)
{
  view_=view;
  views_=views;
  energy_=0.0;
  energy_counts_=0;
  view_.Apply(*this);
  return energy_;
}

int PackingPotential::GetEnergyCounts()
{
  return energy_counts_;
}

bool PackingPotential::VisitAtom(const mol::AtomHandle& atom)
{
  AminoAcid aa=ResidueToAminoAcid(atom.GetResidue());
  if (aa==Xxx)
    return false;
  int count=0;
  for (mol::EntityViewList::iterator i=views_.begin(),
       e=views_.end(); i!=e; ++i) {
    count+=i->FindWithin(atom.GetPos(), options_.cutoff).size();
  }
  energy_+=this->GetPackingEnergy(aa, count);
  energy_counts_++;
  return false;
}

}}
