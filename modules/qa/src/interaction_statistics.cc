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
#include "interaction_statistics.hh"
#include <ost/mol/atom_view.hh>
#include <ost/mol/residue_view.hh>
#include <ost/io/binary_data_source.hh>
#include <ost/io/binary_data_sink.hh>
#include <ost/io/container_serialization.hh>

using namespace ost::mol::impl;

namespace ost { namespace qa {

InteractionStatistics::InteractionStatistics()
{

}

void InteractionStatistics::Extract(mol::EntityView view_a, mol::EntityView view_b) {
  view_a_=view_a;
  view_b_=view_b;
  view_a.Apply(*this);
}

bool InteractionStatistics::VisitResidue(const mol::ResidueHandle& r) {
  amino_acid_=ResidueToAminoAcid(r);
  return amino_acid_!=Xxx;
}

bool InteractionStatistics::VisitAtom(const mol::AtomHandle& atom) {
  atom::ChemType atype=GetAtomTypeByName(amino_acid_, atom.GetName());
  mol::ResidueHandle my_res=atom.GetResidue();
  if (atype!=atom::UNKNOWN) {
    mol::AtomViewList atoms_in_reach=view_b_.FindWithin(atom.GetPos(), 
                                                   upper_cutoff_);
    for (mol::AtomViewList::iterator i=atoms_in_reach.begin(),
         e=atoms_in_reach.end(); i!=e; ++i) {
      mol::AtomView other=*i;
      // avoid rounding problems: distances very close to the cut-off may
      // be rounded up the to cut-off and lead to an overflow in histogram_
      Real dist_sqr=geom::Length2(other.GetPos()-atom.GetPos())-0.0001;
      if (dist_sqr<lower_sqr_) {
        continue;
      }
      mol::ResidueView other_res=other.GetResidue();
      AminoAcid other_aa=ResidueToAminoAcid(other_res.GetHandle());
      if (other_aa==Xxx) {
        continue;
      }
      if (abs(other_res.GetNumber().GetNum()-my_res.GetNumber().GetNum())<sequence_sep_) {
        continue;
      }
      atom::ChemType btype=GetAtomTypeByName(other_aa, other.GetName());
      if (btype==atom::UNKNOWN) {
        continue;
      }
      histogram_.Add(1, atype, btype, sqrt(dist_sqr));
    }
  }
  return true;
}

InteractionStatistics::InteractionStatistics(Real lower_cutoff,
                                             Real upper_cutoff,
                                             Real bucket_size,
                                             int    sequence_sep)
  : lower_cutoff_(lower_cutoff), upper_cutoff_(upper_cutoff),
    bucket_size_(bucket_size),
    sequence_sep_(sequence_sep),
    histogram_(IntegralClassifier(atom::UNKNOWN, 0),
               IntegralClassifier(atom::UNKNOWN, 0),
               ContinuousClassifier(int((upper_cutoff_-lower_cutoff_)/bucket_size),
                                    lower_cutoff_, upper_cutoff_))
{
  upper_sqr_=upper_cutoff_*upper_cutoff_;
  lower_sqr_=lower_cutoff_*lower_cutoff_;
}

int InteractionStatistics::GetSequenceSeparation() const
{
  return sequence_sep_;
}

Real InteractionStatistics::GetUpperCutoff() const
{
  return upper_cutoff_;
}

Real InteractionStatistics::GetLowerCutoff() const
{
  return lower_cutoff_;
}

void InteractionStatistics::SaveToFile(const String& file_name) const
{
  std::ofstream stream(file_name.c_str(), std::ios_base::binary);
  io::BinaryDataSink ds(stream);
  ds << *this;
}

Real InteractionStatistics::GetDistanceBucketSize() const
{
  return bucket_size_;
}

uint64_t InteractionStatistics::GetCount(Real distance) const
{
  uint64_t total=0;
  for (uint64_t i=0; i<atom::UNKNOWN; ++i) {
    for (uint64_t j=0; j<atom::UNKNOWN; ++j) {
      total+=histogram_.Get(i, j, distance);
    }
  }
  return total;
}

uint64_t InteractionStatistics::GetCount(atom::ChemType a,
                                       atom::ChemType b,
                                       Real distance) const
{
  return histogram_.Get(a, b, distance);
}


uint64_t InteractionStatistics::GetCount(atom::ChemType a,
                                       atom::ChemType b,
                                       int distance_bin) const
{
  typedef InteractionHistogram::IndexType Index;

  return histogram_.Get(Index(a, b, distance_bin));
}



uint64_t InteractionStatistics::GetCount(atom::ChemType a,
                                       atom::ChemType b) const
{
  typedef InteractionHistogram::IndexType Index;
  uint64_t num=uint64_t((upper_cutoff_-lower_cutoff_)/bucket_size_);
  uint64_t total=0;
  for (uint64_t k=0; k<num; ++k)	{
    total+=histogram_.Get(Index(a, b, k));
  }
  return total;
}

void InteractionStatistics::Set(atom::ChemType a, atom::ChemType b,
                                int distance_bin, int counts) {
  typedef InteractionHistogram::IndexType Index;
  histogram_.Set(Index(a, b, distance_bin), counts);
}


template <typename DS>
void InteractionStatistics::Serialize(DS& ds)
{
  ds & lower_cutoff_;
  ds & upper_cutoff_;
  ds & bucket_size_;
  ds & sequence_sep_;
  if (ds.IsSource()) {
    upper_sqr_=upper_cutoff_*upper_cutoff_;
    lower_sqr_=lower_cutoff_*lower_cutoff_;
  }
  ds & histogram_;
}

InteractionStatisticsPtr InteractionStatistics::LoadFromFile(const String& fn)
{
  std::ifstream stream(fn.c_str(), std::ios_base::binary);
  io::BinaryDataSource ds(stream);
  InteractionStatisticsPtr p(new InteractionStatistics);
  ds >> *p.get();
  return p;
}

}}
