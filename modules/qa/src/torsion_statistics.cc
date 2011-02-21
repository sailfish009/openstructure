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
#include "torsion_statistics.hh"
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

class TorsionStat : public mol::EntityVisitor {
public:
  TorsionStat(TorsionStatistics::TorsionHistogram& histo):
    histo_(histo)
  {}
  
  virtual bool VisitResidue(const mol::ResidueHandle& residue) {

    mol::ResidueHandle prev=residue.GetPrev();
    mol::ResidueHandle next=residue.GetNext();
    if (!(next && prev && next.IsPeptideLinking() && 
          prev.IsPeptideLinking() && residue.IsPeptideLinking())) {
      return false;
    }

    AminoAcid ca=ResidueToAminoAcid(residue);
    if (ca==Xxx)
      return false;

    mol::TorsionHandle prev_phit=prev.GetPhiTorsion();
    mol::TorsionHandle prev_psit=prev.GetPsiTorsion();
    mol::TorsionHandle central_phit=residue.GetPhiTorsion();
    mol::TorsionHandle central_psit=residue.GetPsiTorsion();
    mol::TorsionHandle next_phit=next.GetPhiTorsion();
    mol::TorsionHandle next_psit=next.GetPsiTorsion();
    if (!prev_phit || !prev_psit || !central_phit || !central_psit ||
        !next_phit || !next_psit)
      return false;
    Real prev_phi=prev_phit.GetAngle()*180/M_PI - 0.001;
    Real prev_psi=prev_psit.GetAngle()*180/M_PI - 0.001;
    Real central_phi=central_phit.GetAngle()*180/M_PI - 0.001;
    Real central_psi=central_psit.GetAngle()*180/M_PI - 0.001;
    Real next_phi=next_phit.GetAngle()*180/M_PI - 0.001;
    Real next_psi=next_psit.GetAngle()*180/M_PI - 0.001;

    histo_.Add(1, ca, prev_phi, prev_psi, central_phi, central_psi, next_phi, next_psi);
    return false;
  }
private:
  TorsionStatistics::TorsionHistogram& histo_;
};

}

TorsionStatistics::TorsionStatistics(int prev_torsion_bucket_size,
                                     int central_torsion_bucket_size,
                                     int next_torsion_bucket_size)
  : histogram_(1, IntegralClassifier(20, 0),
               ContinuousClassifier(int(360/prev_torsion_bucket_size), -180, 180),
               ContinuousClassifier(int(360/prev_torsion_bucket_size), -180, 180),
               ContinuousClassifier(int(360/central_torsion_bucket_size), -180, 180),
               ContinuousClassifier(int(360/central_torsion_bucket_size), -180, 180),
               ContinuousClassifier(int(360/next_torsion_bucket_size), -180, 180),
               ContinuousClassifier(int(360/next_torsion_bucket_size), -180, 180)),
    prev_torsion_bucket_size_(prev_torsion_bucket_size),
    central_torsion_bucket_size_(central_torsion_bucket_size),
    next_torsion_bucket_size_(next_torsion_bucket_size)
{
  
}
    
uint32_t TorsionStatistics::GetTorsionBucketCountPrev() const
{
  return uint32_t(360/prev_torsion_bucket_size_);
}

uint32_t TorsionStatistics::GetTorsionBucketCountCentral() const
{
  return uint32_t(360/central_torsion_bucket_size_);
}

uint32_t TorsionStatistics::GetTorsionBucketCountNext() const
{
  return uint32_t(360/next_torsion_bucket_size_);
}


int TorsionStatistics::GetTorsionBucketSizePrev() const
{
  return prev_torsion_bucket_size_;
}

int TorsionStatistics::GetTorsionBucketSizeCentral() const
{
  return central_torsion_bucket_size_;
}

int TorsionStatistics::GetTorsionBucketSizeNext() const
{
  return next_torsion_bucket_size_;
}


void TorsionStatistics::Extract(mol::EntityView view) 
{
  TorsionStat stat(histogram_);
  view.Apply(stat);
}

void TorsionStatistics::Extract(mol::EntityHandle entity) 
{
  TorsionStat stat(histogram_);
  entity.Apply(stat);
}

TorsionStatisticsPtr TorsionStatistics::LoadFromFile(const String& file_name) 
{  
  std::ifstream stream(file_name.c_str(), std::ios_base::binary);
  TorsionStatisticsPtr ptr(new TorsionStatistics);  
  io::BinaryDataSource ds(stream);
  ds >> *ptr.get();
  return ptr;
}

void TorsionStatistics::SaveToFile(const String& file_name) const 
{
  std::ofstream stream(file_name.c_str(), std::ios_base::binary);
  io::BinaryDataSink ds(stream);
  ds << *this;
}


uint64_t TorsionStatistics::GetCount(const AminoAcidSet& central_aa,
                  Real prev_phi_angle, Real prev_psi_angle,
                  Real central_phi_angle, Real central_psi_angle,
                  Real next_phi_angle, Real next_psi_angle) const
{
  uint64_t count=0;
  uint32_t prev_phi_bucket=this->IndexForAnglePrev(prev_phi_angle);
  uint32_t prev_psi_bucket=this->IndexForAnglePrev(prev_psi_angle);
  uint32_t central_phi_bucket=this->IndexForAngleCentral(central_phi_angle);
  uint32_t central_psi_bucket=this->IndexForAngleCentral(central_psi_angle);
  uint32_t next_phi_bucket=this->IndexForAngleNext(next_phi_angle);
  uint32_t next_psi_bucket=this->IndexForAngleNext(next_psi_angle);
  for (AminoAcidSet::Iterator i=central_aa.Begin(), e1=central_aa.End(); i!=e1; ++i) {
        TorsionHistogram::IndexType start(*i, prev_phi_bucket, prev_psi_bucket,
                                          central_phi_bucket, central_psi_bucket,
                                          next_phi_bucket, next_psi_bucket),
                                      end(*i, prev_phi_bucket, prev_psi_bucket,
                                          central_phi_bucket, central_psi_bucket,
                                          next_phi_bucket, next_psi_bucket);
        count+=this->Accumulate(start, end);
  }
  return count;        
}


uint64_t TorsionStatistics::GetCount(Real prev_phi_angle, Real prev_psi_angle,
                  Real central_phi_angle, Real central_psi_angle,
                  Real next_phi_angle, Real next_psi_angle) const
{
  uint32_t prev_phi_bucket=this->IndexForAnglePrev(prev_phi_angle);
  uint32_t prev_psi_bucket=this->IndexForAnglePrev(prev_psi_angle);
  uint32_t central_phi_bucket=this->IndexForAngleCentral(central_phi_angle);
  uint32_t central_psi_bucket=this->IndexForAngleCentral(central_psi_angle);
  uint32_t next_phi_bucket=this->IndexForAngleNext(next_phi_angle);
  uint32_t next_psi_bucket=this->IndexForAngleNext(next_psi_angle);
  TorsionHistogram::IndexType start(0, prev_phi_bucket, prev_psi_bucket,
                                    central_phi_bucket, central_psi_bucket,
                                    next_phi_bucket, next_psi_bucket),
                              end(Xxx-1, prev_phi_bucket, prev_psi_bucket,
                                    central_phi_bucket, central_psi_bucket,
                                    next_phi_bucket, next_psi_bucket);
  return this->Accumulate(start, end);
}


uint64_t TorsionStatistics::GetCount(const AminoAcidSet& central_aa) const
{
  uint64_t count=0;
  uint32_t prev_buckets=uint32_t(360.0/prev_torsion_bucket_size_);
  uint32_t central_buckets=uint32_t(360.0/central_torsion_bucket_size_);
  uint32_t next_buckets=uint32_t(360.0/next_torsion_bucket_size_);

  for (AminoAcidSet::Iterator i=central_aa.Begin(), e1=central_aa.End(); i!=e1; ++i) {
        TorsionHistogram::IndexType start(*i, 0, 0, 0, 0, 0, 0),
                                    end(*i, prev_buckets-1, prev_buckets-1,
                                        central_buckets-1, central_buckets-1,
                                        next_buckets-1, next_buckets-1);
        count+=this->Accumulate(start, end);
  }
  return count;
}


uint64_t TorsionStatistics::GetCount() const
{
  uint32_t prev_buckets=uint32_t(360.0/prev_torsion_bucket_size_);
  uint32_t central_buckets=uint32_t(360.0/central_torsion_bucket_size_);
  uint32_t next_buckets=uint32_t(360.0/next_torsion_bucket_size_);

  TorsionHistogram::IndexType start(0, 0, 0, 0, 0, 0, 0),
                              end(Xxx-1, prev_buckets-1, prev_buckets-1,
                                  central_buckets-1, central_buckets-1,
                                  next_buckets-1, next_buckets-1);
  return this->Accumulate(start, end);
}


uint64_t TorsionStatistics::Accumulate(const IndexType& start, 
                                     const IndexType& end) const
{
  uint64_t total=0;
  for (TorsionHistogram::Iterator i(start, end); !i.AtEnd(); ++i) {
    total+=histogram_.Get(*i);
  }
  return total;                                         
}


uint32_t TorsionStatistics::IndexForAnglePrev(Real angle) const
{
  return (histogram_.FindBucket(0,angle,0,0,0,0,0))[1];
}

uint32_t TorsionStatistics::IndexForAngleCentral(Real angle) const
{
  return (histogram_.FindBucket(0,0,0,angle,0,0,0))[3];
}

uint32_t TorsionStatistics::IndexForAngleNext(Real angle) const
{
  return (histogram_.FindBucket(0,0,0,0,0,angle,0))[5];
}

    
  
TorsionStatistics::TorsionStatistics():
  prev_torsion_bucket_size_(0),
  central_torsion_bucket_size_(0),
  next_torsion_bucket_size_(0)
{
}

} }
