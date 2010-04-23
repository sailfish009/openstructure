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
#include "torsion_potential.hh"
#include "amino_acids.hh"

#include <ost/log.hh>
#include <ost/io/binary_data_source.hh>
#include <ost/io/binary_data_sink.hh>
#include <ost/io/container_serialization.hh>
#include <ost/integrity_error.hh>
#include <ost/mol/mol.hh>
#include <boost/filesystem/convenience.hpp>
#include <ost/message.hh>
#include <ost/io/io_exception.hh>

/*
  Authors: Marco Biasini, Pascal Benkert
 */

namespace ost { namespace qa {
  
namespace {

class TorsionEnergyCalc : public mol::EntityVisitor {
public:
  TorsionEnergyCalc(TorsionPotential::TorsionEnergies& energies, 
                    TorsionPotentialOpts opts):
    energies_(energies), energy_(0.0),
    opts_(opts), num_torsions_(0)
  {
  }
  
  virtual bool VisitResidue(const mol::ResidueHandle& rh)
  {
    mol::ResidueHandle prev=rh.GetPrev();
    mol::ResidueHandle next=rh.GetNext();
    if (!(next && prev && next.IsPeptideLinking() &&
          prev.IsPeptideLinking() && rh.IsPeptideLinking())) {
      return false;
    }

    AminoAcid ca=ResidueToAminoAcid(rh);
    if (ca==Xxx)
      return false;

    mol::TorsionHandle prev_phit=prev.GetPhiTorsion();
    mol::TorsionHandle prev_psit=prev.GetPsiTorsion();
    mol::TorsionHandle central_phit=rh.GetPhiTorsion();
    mol::TorsionHandle central_psit=rh.GetPsiTorsion();
    mol::TorsionHandle next_phit=next.GetPhiTorsion();
    mol::TorsionHandle next_psit=next.GetPsiTorsion();
    if (!prev_phit || !prev_psit || !central_phit || !central_psit ||
        !next_phit || !next_psit)
      return false;
    Real prev_phi=prev_phit.GetAngle()*180/M_PI - 0.001; if (prev_phi<-180) prev_phi=-180;
    Real prev_psi=prev_psit.GetAngle()*180/M_PI - 0.001; if (prev_psi<-180) prev_psi=-180;
    Real central_phi=central_phit.GetAngle()*180/M_PI - 0.001; if (central_phi<-180) central_phi=-180;
    Real central_psi=central_psit.GetAngle()*180/M_PI - 0.001; if (central_psi<-180) central_psi=-180;
    Real next_phi=next_phit.GetAngle()*180/M_PI - 0.001; if (next_phi<-180) next_phi=-180;
    Real next_psi=next_psit.GetAngle()*180/M_PI - 0.001; if (next_psi<-180) next_psi=-180;

    // calculate position of the amino acid in the alphabet
    int icenter=this->GetAAIndex(ca);

    if (icenter >-1) {
      energy_+=energies_.Get(icenter, prev_phi, prev_psi,
                             central_phi, central_psi,
                             next_phi, next_psi);
      num_torsions_++;
    } 
    else {
      LOGN_MESSAGE("Amino acid not found in alphabets...");
    }
    return false;
  }

  Real GetEnergy() const {
    return energy_;
    //return num_torsions_==0?0:energy_/num_torsions_;
  }

  int GetEnergyCounts() const {
    return num_torsions_;
  }

private:
  int GetAAIndex(AminoAcid aa)
  {
    return this->GetIndex(aa, opts_.alphabet);
  }

  
  int GetIndex(AminoAcid aa, AminoAcidAlphabet& alpha) {
    AminoAcidAlphabet::iterator i=alpha.begin();
    for (int index=0; i!=alpha.end(); ++i, ++index) {
      if ((*i).Contains(aa)) {
        return index;        
      }

    }
    return -1;
  }
private:
  TorsionPotential::TorsionEnergies& energies_;
  AminoAcid prev_;
  AminoAcid center_;
  mol::ResidueHandle cr_;
  Real energy_;  
  TorsionPotentialOpts opts_;  
  int    num_torsions_;  
};

}

template <typename DS>
void TorsionPotentialOpts::Serialize(DS& ds) 
{
  ds & prev_angular_bucket_size;
  ds & central_angular_bucket_size;
  ds & next_angular_bucket_size;
  ds & alphabet;
  ds & sigma;
}

template <typename DS>
void TorsionPotential::Serialize(DS& ds)
{
  ds & options_;
  ds & energies_;
}

TorsionPotentialOpts::TorsionPotentialOpts():
  prev_angular_bucket_size(90), central_angular_bucket_size(45),
  next_angular_bucket_size(90), sigma(0.0005)
{
  
}


TorsionPotentialPtr TorsionPotential::Create(TorsionStatisticsPtr statistics,
                                             const TorsionPotentialOpts& opts,
                                             bool calculate_average_energy_flag)
{

  typedef TorsionPotential::TorsionEnergies TorsionEnergies;
  int prev_bucket_size=statistics->GetTorsionBucketSizePrev();
  int central_bucket_size=statistics->GetTorsionBucketSizeCentral();
  int next_bucket_size=statistics->GetTorsionBucketSizeNext();

  //std::cout << prev_bucket_size << " " << central_bucket_size << " " << next_bucket_size << std::endl;

  if (prev_bucket_size == 0 || central_bucket_size == 0 || next_bucket_size == 0) {
    throw IntegrityError("Torsion angle statistics does not exist. Re-extract counts.");
  }

  if ((opts.prev_angular_bucket_size % prev_bucket_size)!=0) {
    throw IntegrityError("Previous angular bucket size for torsion potential is not an "
                          "integral multiple of the statistics bucket size");
  }
  if ((opts.central_angular_bucket_size % central_bucket_size)!=0) {
    throw IntegrityError("Central angular bucket size for torsion potential is not an "
                          "integral multiple of the statistics bucket size");
  }
  if ((opts.next_angular_bucket_size % next_bucket_size)!=0) {
    throw IntegrityError("Next  angular bucket size for torsion potential is not an "
                          "integral multiple of the statistics bucket size");
  }
  int prev_buckets=360/opts.prev_angular_bucket_size;
  int central_buckets=360/opts.central_angular_bucket_size;
  int next_buckets=360/opts.next_angular_bucket_size;
  TorsionPotentialPtr ptr(new TorsionPotential);

  ptr->options_.prev_angular_bucket_size = opts.prev_angular_bucket_size;
  ptr->options_.central_angular_bucket_size = opts.central_angular_bucket_size;
  ptr->options_.next_angular_bucket_size = opts.next_angular_bucket_size;

  ptr->options_.alphabet = opts.alphabet;
  uint64_t alphabet_size=ptr->options_.alphabet.size();

  ptr->energies_=TorsionEnergies(0.0, IntegralClassifier(alphabet_size, 0),
                                 ContinuousClassifier(prev_buckets,-180,180),
                                 ContinuousClassifier(prev_buckets,-180,180),
                                 ContinuousClassifier(central_buckets,-180,180),
                                 ContinuousClassifier(central_buckets,-180,180),
                                 ContinuousClassifier(next_buckets,-180,180),
                                 ContinuousClassifier(next_buckets,-180,180));
  ptr->Fill(statistics, calculate_average_energy_flag);
  return ptr;    
}                            

TorsionPotentialPtr TorsionPotential::LoadFromFile(const String& path) 
{
  if(!boost::filesystem::exists(path))
    throw io::IOException("Could not open torsion potential data file.\nFile does not exist at: "+path);

  std::ifstream stream(path.c_str(),  std::ios_base::binary);
  io::BinaryDataSource ds(stream);
  TorsionPotentialPtr ptr(new TorsionPotential);
  ds >> *ptr.get();
  return ptr;
}

void TorsionPotential::SaveToFile(const String& path)
{
  std::ofstream stream(path.c_str(),  std::ios_base::binary);
  io::BinaryDataSink ds(stream);
  ds << *this;
}

Real TorsionPotential::GetTotalEnergy(mol::EntityHandle entity) 
{
  TorsionEnergyCalc c(energies_, options_);  
  entity.Apply(c);  
  num_torsions_ = c.GetEnergyCounts();
  return c.GetEnergy();
}

Real TorsionPotential::GetTotalEnergy(mol::EntityView entity) 
{
  TorsionEnergyCalc c(energies_, options_);
  entity.Apply(c);
  num_torsions_ = c.GetEnergyCounts();
  return c.GetEnergy();
}

int TorsionPotential::GetEnergyCounts() const {
  return num_torsions_;
}

void TorsionPotential::Fill(const TorsionStatisticsPtr& stat, bool calculate_average_energy_flag)
{
  int prev_num_b=360/(options_.prev_angular_bucket_size);
  int central_num_b=360/(options_.central_angular_bucket_size);
  int next_num_b=360/(options_.next_angular_bucket_size);

  int prev_half_b=options_.prev_angular_bucket_size/2;
  int central_half_b=options_.central_angular_bucket_size/2;
  int next_half_b=options_.next_angular_bucket_size/2;

  uint64_t M=stat->GetCount();
  //std::cout << "total counts: " << M << std::endl;

  typedef AminoAcidAlphabet::const_iterator AAA;
  for (AAA i=options_.alphabet.begin(),
       e1=options_.alphabet.end(); i!=e1; ++i) {

    uint64_t Mi=stat->GetCount(*i);
    if (calculate_average_energy_flag==1) std::cout << *i << " " << float(Mi)/M  << " ";
    Real per_aa_energy = 0; // for average energy


    for (int prev_phi=0; prev_phi<prev_num_b; ++prev_phi) {
      for (int prev_psi=0; prev_psi<prev_num_b; ++prev_psi) {
        int prev_phi_a=-180+options_.prev_angular_bucket_size*prev_phi+prev_half_b;
        int prev_psi_a=-180+options_.prev_angular_bucket_size*prev_psi+prev_half_b;

          for (int central_phi=0; central_phi<central_num_b; ++central_phi) {
            for (int central_psi=0; central_psi<central_num_b; ++central_psi) {
              int central_phi_a=-180+options_.central_angular_bucket_size*central_phi+central_half_b;
              int central_psi_a=-180+options_.central_angular_bucket_size*central_psi+central_half_b;

              for (int next_phi=0; next_phi<next_num_b; ++next_phi) {
                for (int next_psi=0; next_psi<next_num_b; ++next_psi) {
                  int next_phi_a=-180+options_.next_angular_bucket_size*next_phi+next_half_b;
                  int next_psi_a=-180+options_.next_angular_bucket_size*next_psi+next_half_b;

                  uint64_t fxi=stat->GetCount(*i, prev_phi_a, prev_psi_a, central_phi_a, central_psi_a, next_phi_a, next_psi_a);
                  uint64_t fx=stat->GetCount(prev_phi_a, prev_psi_a, central_phi_a, central_psi_a, next_phi_a, next_psi_a);

                  // propensity = (fxi/Mi)/(fx/M)
                  Real propensity=0.0;
                  Real energy=0.0;
                  //avoid division by zero:
                  if (fx != 0 && Mi != 0 && M != 0) {
                    propensity=Real(Real(fxi)/Mi)/(Real(fx)/M);
                  }

                  energy=log(1+options_.sigma*Mi)-
                      log(1+options_.sigma*Mi*propensity);
		  
                  per_aa_energy=per_aa_energy+fxi*energy;  // for average energy


                  energies_.Add(energy,
                      i-options_.alphabet.begin(),
                      prev_phi_a, prev_psi_a, central_phi_a, central_psi_a, next_phi_a, next_psi_a);
                }
              }

            }
          }

      }
    }
    if (calculate_average_energy_flag==1) std::cout << per_aa_energy/Mi  << std::endl;
}
}
}}
