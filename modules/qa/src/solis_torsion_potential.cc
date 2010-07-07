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
#include "solis_torsion_potential.hh"
#include "amino_acids.hh"

#include <ost/mol/mol.hh>
#include <ost/log.hh>
#include <ost/io/binary_data_source.hh>
#include <ost/io/binary_data_sink.hh>
#include <ost/io/container_serialization.hh>
#include <ost/integrity_error.hh>
#include <boost/filesystem/convenience.hpp>
#include <ost/message.hh>
#include <ost/io/io_exception.hh>

namespace ost { namespace qa {
  
namespace {

class SolisTorsionEnergyCalc : public mol::EntityVisitor {
public:
  SolisTorsionEnergyCalc(TorsionPotentialSolis::TorsionEnergies& energies,
                    TorsionPotentialOptsSolis opts):
    energies_(energies), prev_(Xxx), center_(Xxx), energy_(0.0),
    opts_(opts), num_torsions_(0)
  {
  }
  
  virtual bool VisitResidue(const mol::ResidueHandle& rh)
  {
    AminoAcid c=ResidueToAminoAcid(rh);

    if (prev_==Xxx || center_==Xxx || c==Xxx) {
      prev_=center_;
      center_=c;
      cr_=rh;      
      return false;
    }
    mol::TorsionHandle phit=cr_.GetPhiTorsion();
    mol::TorsionHandle psit=cr_.GetPsiTorsion();
    if (!phit || !psit) {
      prev_=center_;
      center_=c;
      cr_=rh;      
      return false;      
    }
    Real phi=phit.GetAngle()*180/M_PI - 0.000000001;
    Real psi=psit.GetAngle()*180/M_PI - 0.000000001;
    int ibefore=this->GetOuterAAIndex(prev_);
    int iafter=this->GetOuterAAIndex(c);   
    int icenter=this->GetInnerAAIndex(center_);


    if (iafter>-1 && icenter >-1 && ibefore>-1) {
      energy_+=energies_.Get(ibefore, icenter, iafter, phi, psi);
      num_torsions_++;
    } 
    else {
      LOGN_MESSAGE("Amino acid not found in alphabets...");
    }
    prev_=center_;
    center_=c;
    cr_=rh;        
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
  int GetOuterAAIndex(AminoAcid aa)
  {
    return this->GetIndex(aa, opts_.outer_alphabet);
  }
  
  int GetInnerAAIndex(AminoAcid aa)
  {
    return this->GetIndex(aa, opts_.inner_alphabet);
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
  TorsionPotentialSolis::TorsionEnergies& energies_;
  AminoAcid prev_;
  AminoAcid center_;
  mol::ResidueHandle cr_;
  Real energy_;  
  TorsionPotentialOptsSolis opts_;
  int    num_torsions_;  
};

}

template <typename DS>
void TorsionPotentialOptsSolis::Serialize(DS& ds)
{
  ds & angular_bucket_size;
  ds & outer_alphabet;
  ds & inner_alphabet;
  ds & sigma;
}

template <typename DS>
void TorsionPotentialSolis::Serialize(DS& ds)
{
  ds & options_;
  ds & energies_;
}

TorsionPotentialOptsSolis::TorsionPotentialOptsSolis():
  angular_bucket_size(30), sigma(0.02) 
{
  
}


TorsionPotentialSolisPtr TorsionPotentialSolis::Create(
                                              TorsionStatisticsSolisPtr statistics,
                                             const TorsionPotentialOptsSolis& opts)
{
  typedef TorsionPotentialSolis::TorsionEnergies TorsionEnergies;
  int bucket_size=statistics->GetTorsionBucketSize();

  if ((opts.angular_bucket_size % bucket_size)!=0) {
    throw IntegrityError("Angular bucket size for torsion potential is not an "
                          "integral multiple of the statistics bucket size");
  }
  int buckets=360/opts.angular_bucket_size;
  TorsionPotentialSolisPtr ptr(new TorsionPotentialSolis);

  ptr->options_.angular_bucket_size = opts.angular_bucket_size;
  ptr->options_.outer_alphabet = opts.outer_alphabet;
  ptr->options_.inner_alphabet = opts.inner_alphabet;

  uint32_t outer_s=ptr->options_.outer_alphabet.size();
  uint32_t inner_s=ptr->options_.inner_alphabet.size();
  ptr->energies_=TorsionEnergies(0.0, IntegralClassifier(outer_s, 0),
                                 IntegralClassifier(inner_s, 0),
                                 IntegralClassifier(outer_s, 0),                            
                                 ContinuousClassifier(buckets,-180,180),
                                 ContinuousClassifier(buckets,-180,180));
  ptr->FillAll(statistics);
  return ptr;    
}                            

TorsionPotentialSolisPtr TorsionPotentialSolis::LoadFromFile(const String& path)
{
  if(!boost::filesystem::exists(path))
  throw io::IOException("Could not open torsion potential data file.\nFile does not exist at: "+path);

  std::ifstream stream(path.c_str(),  std::ios_base::binary);
  io::BinaryDataSource ds(stream);
  TorsionPotentialSolisPtr ptr(new TorsionPotentialSolis);
  ds >> *ptr.get();
  return ptr;
}

void TorsionPotentialSolis::SaveToFile(const String& path)
{
  std::ofstream stream(path.c_str(),  std::ios_base::binary);
  io::BinaryDataSink ds(stream);
  ds << *this;
}

Real TorsionPotentialSolis::GetTotalEnergy(mol::EntityHandle entity)
{
  SolisTorsionEnergyCalc c(energies_, options_);  
  entity.Apply(c);
  num_torsions_ = c.GetEnergyCounts();
  return c.GetEnergy();
}

Real TorsionPotentialSolis::GetTotalEnergy(mol::EntityView entity)
{
  SolisTorsionEnergyCalc c(energies_, options_);
  entity.Apply(c);
  num_torsions_ = c.GetEnergyCounts();
  return c.GetEnergy();
}

int TorsionPotentialSolis::GetEnergyCounts() const {
  return num_torsions_;
}


void TorsionPotentialSolis::FillAll(const TorsionStatisticsSolisPtr& stat)
{
  uint32_t total=stat->GetCount();
  int num_b=360/(options_.angular_bucket_size);
  int half_b=options_.angular_bucket_size/2;
  for (int phi=0; phi<num_b; ++phi) {
    for (int psi=0; psi<num_b; ++psi) {
      int phi_a=-180+options_.angular_bucket_size*phi+half_b;
      int psi_a=-180+options_.angular_bucket_size*psi+half_b;
      this->FillPhiPsi(stat, phi_a, psi_a, total);
    }
  }
}

void TorsionPotentialSolis::FillPhiPsi(const TorsionStatisticsSolisPtr& stat,
                                  int phi, int psi, uint32_t M)
{
  // collect total counts for all amino acids first
  uint32_t fx=stat->GetCount(phi, psi);
  typedef AminoAcidAlphabet::const_iterator AAA;
  for (AAA i=options_.outer_alphabet.begin(), 
       e1=options_.outer_alphabet.end(); i!=e1; ++i) {
    for (AAA j=options_.inner_alphabet.begin(), 
         e2=options_.inner_alphabet.end(); j!=e2; ++j) {
      for (AAA k=options_.outer_alphabet.begin(), 
           e3=options_.outer_alphabet.end(); k!=e3; ++k) {

        uint32_t fxi=stat->GetCount(*i, *j, *k, phi, psi);
        uint32_t Mi=stat->GetCount(*i, *j, *k);

        // propensity = (fxi/Mi)/(fx/M)
        Real propensity=0.0;
        //avoid division by zero:
        if (fx != 0 && Mi != 0 && M != 0) {
          propensity=Real(Real(fxi)/Mi)/(Real(fx)/M);
        }

        Real energy=log(1+options_.sigma*Mi)-
                      log(1+options_.sigma*Mi*propensity);

        energies_.Add(energy,
                      i-options_.outer_alphabet.begin(),
                      j-options_.inner_alphabet.begin(),
                      k-options_.outer_alphabet.begin(), phi, psi);
      }
    }
  }  
}



}}
