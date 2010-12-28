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

#include <ost/io/binary_data_source.hh>
#include <ost/io/binary_data_sink.hh>
#include <ost/io/container_serialization.hh>
#include "reduced_potential.hh"
#include "impl/reduced_impl.hh"
namespace ost { namespace qa {


namespace {

class ReducedEnergiesCalc : public impl::ReducedPotentialImpl {
public:

  ReducedEnergiesCalc(const ReducedStatOptions& opts, ReducedEnergies& energies,
                      mol::EntityHandle ent, bool norm): 
    impl::ReducedPotentialImpl(opts, ent),
    energies_(energies), energy_(0.0), norm_(norm), count_(0)
  { }

  virtual void OnInteraction(AminoAcid aa_one, AminoAcid aa_two, 
                             Real dist, Real angle)
  {
    //std::cout << aa_one << " " << aa_two << " " << dist  << " " << angle << std::endl;
    energy_+=energies_.Get(aa_one, aa_two, dist, angle);
    count_+=1;
  }
  
  Real GetEnergy() const 
  {
    if (norm_) {
      return count_ > 0 ? energy_ /count_ : 0.0;
    }
    return energy_;
  }
private:
  ReducedEnergies&   energies_;
  Real energy_;
  bool norm_;
  uint64_t count_;
};


}

ReducedPotentialPtr ReducedPotential::Load(const String& filename)
{
  std::ifstream stream(filename.c_str(), std::ios_base::binary);
  io::BinaryDataSource ds(stream);
  ReducedPotentialPtr p(new ReducedPotential);
  ds >> *p.get();
  return p;
}


ReducedPotentialPtr ReducedPotential::Create(const ReducedStatisticsPtr& stat)
{
  ReducedPotentialPtr pot(new ReducedPotential);
  pot->opts_=stat->GetOptions();
  pot->energies_=ReducedEnergies(0.0, IntegralClassifier(20, 0), 
                                 IntegralClassifier(20, 0),
                                 ContinuousClassifier(pot->opts_.num_dist_bins, 
                                                      pot->opts_.lower_cutoff, 
                                                      pot->opts_.upper_cutoff),
                                 ContinuousClassifier(pot->opts_.num_angular_bins, 
                                                      0.0, M_PI));
  pot->InitFromStats(stat);
  return pot;
}

void ReducedPotential::InitFromStats(const ReducedStatisticsPtr& stats)
{
    typedef ReducedEnergies::IndexType Index;
    uint64_t total_count=stats->GetTotalCount();
    for (size_t i=0; i<Xxx; ++i) {
      for (size_t j=0; j<Xxx; ++j) {
        uint64_t t2=stats->GetCount(AminoAcid(i), AminoAcid(j));
        for (size_t k=0; k<opts_.num_dist_bins; ++k) {
          for (size_t l=0; l<opts_.num_angular_bins; ++l) {
            uint64_t t3=stats->GetCount(AminoAcid(i), AminoAcid(j), k, l);
            // could be moved outside of the loop and cached...
            uint64_t t4=stats->GetCount(k, l);
            float f1=t2 > 0 ?(float(t3)/float(t2)) : 0.0;
            float f2=total_count > 0 ? (float(t4)/float(total_count)) : 0.0;
            float d = f2>1e-10 ? f1/f2: 0.0;
            float e=0.582*(log(1+0.02*t2)-log(1+0.02*t2*d));
            energies_.Set(Index(i, j, k, l), e);
          }
        }
      }
    }
}

void ReducedPotential::Save(const String& filename)
{  
  std::ofstream stream(filename.c_str(), std::ios_base::binary);
  io::BinaryDataSink ds(stream);
  ds << *this;
}


template <typename DS>
void ReducedPotential::Serialize(DS& ds)
{
  ds & opts_;
  ds & energies_;
}

Real ReducedPotential::GetTotalEnergy(ost::mol::EntityHandle ent, bool norm)
{
  ReducedEnergiesCalc calc(opts_, energies_, ent, norm);
  ent.Apply(calc);
  return calc.GetEnergy();
}


}}
