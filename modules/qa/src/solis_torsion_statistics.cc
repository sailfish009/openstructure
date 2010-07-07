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
#include "solis_torsion_statistics.hh"
#include "amino_acids.hh"
#include <ost/log.hh>
#include <ost/mol/mol.hh>
#include <fstream>
#include <ost/message.hh>
#include <ost/io/binary_data_source.hh>
#include <ost/io/binary_data_sink.hh>
#include <ost/io/container_serialization.hh>

/*
  Author: Marco Biasini, Pascal Benkert
 */
namespace ost { namespace qa {


namespace {

class TorsionStatSolis : public mol::EntityVisitor {
public:
  TorsionStatSolis(TorsionStatisticsSolis::TorsionHistogram& histo):
    histo_(histo)
  {}
  
  virtual bool VisitResidue(const mol::ResidueHandle& residue) {

    mol::ResidueHandle prev=residue.GetPrev();
    mol::ResidueHandle next=residue.GetNext();
    if (!(next && prev && next.IsPeptideLinking() && 
          prev.IsPeptideLinking() && residue.IsPeptideLinking())) {
      return false;
    }

    AminoAcid pa=ResidueToAminoAcid(prev);
    AminoAcid na=ResidueToAminoAcid(next);  
    AminoAcid ca=ResidueToAminoAcid(residue);

    if (pa==Xxx || na==Xxx || ca==Xxx)
      return false;
    mol::TorsionHandle phit=residue.GetPhiTorsion();
    mol::TorsionHandle psit=residue.GetPsiTorsion();
    if (!phit || !psit)
      return false;
    Real phi=phit.GetAngle()*180/M_PI - 0.000000001;
    Real psi=psit.GetAngle()*180/M_PI - 0.000000001;
    histo_.Add(1, pa, ca, na, phi, psi);
    return false;
  }
private:
  TorsionStatisticsSolis::TorsionHistogram& histo_;
};

}

// initialise with 0 or 1 (1 better for combined potentials)
TorsionStatisticsSolis::TorsionStatisticsSolis(int torsion_bucket_size)
  : histogram_(1,IntegralClassifier(20, 0),
               IntegralClassifier(20, 0),
               IntegralClassifier(20, 0),
               ContinuousClassifier(int(360/torsion_bucket_size), -180, 180),
               ContinuousClassifier(int(360/torsion_bucket_size), -180, 180)),
    torsion_bucket_size_(torsion_bucket_size) 
{
  
}
    
uint32_t TorsionStatisticsSolis::GetTorsionBucketCount() const
{
  return uint32_t(360/torsion_bucket_size_);
}

int TorsionStatisticsSolis::GetTorsionBucketSize() const
{
  return torsion_bucket_size_;
}

void TorsionStatisticsSolis::Extract(mol::EntityView view)
{
  TorsionStatSolis stat(histogram_);
  view.Apply(stat);
}

void TorsionStatisticsSolis::Extract(mol::EntityHandle entity)
{
  TorsionStatSolis stat(histogram_);
  entity.Apply(stat);
}

TorsionStatisticsSolisPtr TorsionStatisticsSolis::LoadFromFile(const String& file_name)
{  
  std::ifstream stream(file_name.c_str(), std::ios_base::binary);
  TorsionStatisticsSolisPtr ptr(new TorsionStatisticsSolis);
  io::BinaryDataSource ds(stream);
  ds >> *ptr.get();
  return ptr;
}

void TorsionStatisticsSolis::SaveToFile(const String& file_name) const
{
  std::ofstream stream(file_name.c_str(), std::ios_base::binary);
  io::BinaryDataSink ds(stream);
  ds << *this;
}

uint32_t TorsionStatisticsSolis::GetCount(const AminoAcidSet& prev_aa,
                                   const AminoAcidSet& central_aa,
                                   const AminoAcidSet& next_aa,
                                   Real phi_angle, Real psi_angle) const 
{
  uint32_t count=0;
  uint32_t phi_bucket=this->IndexForAngle(phi_angle);
  uint32_t psi_bucket=this->IndexForAngle(psi_angle);
  for (AminoAcidSet::Iterator i=prev_aa.Begin(), e1=prev_aa.End(); i!=e1; ++i) {
    for (AminoAcidSet::Iterator j=central_aa.Begin(), 
         e2=central_aa.End(); j!=e2; ++j) {
      for (AminoAcidSet::Iterator k=next_aa.Begin(), 
           e3=next_aa.End(); k!=e3; ++k) {
        TorsionHistogram::IndexType start(*i, *j, *k, phi_bucket, psi_bucket), 
                                    end(*i, *j, *k, phi_bucket, psi_bucket);
        count+=this->Accumulate(start, end);
      }
    }
  }
  return count;        
}

uint32_t TorsionStatisticsSolis::GetCount(const AminoAcidSet& prev_aa,
                                   const AminoAcidSet& central_aa,
                                   const AminoAcidSet& next_aa) const 
{
  uint32_t count=0;
  uint32_t torsion_buckets=uint32_t(360.0/torsion_bucket_size_);    
  for (AminoAcidSet::Iterator i=prev_aa.Begin(), e1=prev_aa.End(); i!=e1; ++i) {
    for (AminoAcidSet::Iterator j=central_aa.Begin(), 
         e2=central_aa.End(); j!=e2; ++j) {
      for (AminoAcidSet::Iterator k=next_aa.Begin(), 
           e3=next_aa.End(); k!=e3; ++k) {
        TorsionHistogram::IndexType start(*i, *j, *k, 0, 0), 
                                    end(*i, *j, *k, torsion_buckets-1, 
                                        torsion_buckets-1);
        count+=this->Accumulate(start, end);
      }
    }
  }
  return count;        
}
    

uint32_t TorsionStatisticsSolis::GetCount(Real phi_angle, Real psi_angle) const
{
  uint32_t phi_bucket=this->IndexForAngle(phi_angle);
  uint32_t psi_bucket=this->IndexForAngle(psi_angle);
  TorsionHistogram::IndexType start(0,0,0, phi_bucket, psi_bucket), 
    end(Xxx-1, Xxx-1, Xxx-1, phi_bucket, psi_bucket);
  return this->Accumulate(start, end);
}

uint32_t TorsionStatisticsSolis::Accumulate(const IndexType& start,
                                     const IndexType& end) const
{
  uint32_t total=0;
  for (TorsionHistogram::Iterator i(start, end); !i.AtEnd(); ++i) {
    total+=histogram_.Get(*i);
  }
  return total;                                         
}


uint32_t TorsionStatisticsSolis::IndexForAngle(Real angle) const
{
  return (histogram_.FindBucket(0,0,0,angle,0))[3];
}
    
uint32_t TorsionStatisticsSolis::GetCount(AminoAcid central_aa,
                                   Real phi_angle, Real psi_angle) const 
{
  uint32_t phi_bucket=this->IndexForAngle(phi_angle);
  uint32_t psi_bucket=this->IndexForAngle(psi_angle);
  TorsionHistogram::IndexType start(0,central_aa,0, phi_bucket, psi_bucket), 
                              end(Xxx-1, central_aa, Xxx-1, 
                                  phi_bucket, psi_bucket);
  return this->Accumulate(start, end);                 
}


Real TorsionStatisticsSolis::GetFrequency(AminoAcid aa, Real phi_angle,
                                       Real psi_angle) const 
{
  uint32_t phi_bucket=this->IndexForAngle(phi_angle);
  uint32_t psi_bucket=this->IndexForAngle(psi_angle);  
  uint32_t local_count=0, total_count=0;
  uint32_t torsion_buckets=uint32_t(360.0/torsion_bucket_size_);   
  for (uint32_t phi=0; phi<torsion_buckets; ++phi) {
    for (uint32_t psi=0; psi<torsion_buckets; ++psi) {
      TorsionHistogram::IndexType start(0, aa, 0, phi, psi), 
        end(Xxx-1, aa, Xxx-1, phi, psi);
      uint32_t phi_psi_count=this->Accumulate(start, end);                                
      if (phi==phi_bucket && psi==psi_bucket) {
        local_count+=phi_psi_count;
      }
      total_count+=phi_psi_count;
    }
  }
  return Real(local_count)/Real(total_count);
}

Real TorsionStatisticsSolis::GetFrequency(Real phi_angle,
                                       Real psi_angle) const 
{
  uint32_t phi_bucket=this->IndexForAngle(phi_angle);
  uint32_t psi_bucket=this->IndexForAngle(psi_angle);  
  uint32_t local_count=0, total_count=0;
  uint32_t torsion_buckets=uint32_t(360.0/torsion_bucket_size_);  
  for (uint32_t phi=0; phi<torsion_buckets; ++phi) {
    for (uint32_t psi=0; psi<torsion_buckets; ++psi) {
      TorsionHistogram::IndexType start(0, 0, 0, phi, psi), 
                                  end(Xxx-1, Xxx-1, Xxx-1, phi, psi);
      uint32_t phi_psi_count=this->Accumulate(start, end);                                
      if (phi==phi_bucket && psi==psi_bucket) {
        local_count+=phi_psi_count;
      }
      total_count+=phi_psi_count;
    }
  }
  return Real(local_count)/Real(total_count);
}

int TorsionStatisticsSolis::GetCount() const
{
  uint32_t torsion_buckets=uint32_t(360/torsion_bucket_size_);    
  TorsionHistogram::IndexType start(0, 0, 0, 0, 0), 
                              end(Xxx-1, Xxx-1, Xxx-1, torsion_buckets-1,
                                  torsion_buckets-1);
  return this->Accumulate(start, end);
}

  
TorsionStatisticsSolis::TorsionStatisticsSolis():
  torsion_bucket_size_(0)
{
}

}}
