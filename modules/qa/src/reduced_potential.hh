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
#ifndef OST_QA_REDUCED_POTENTIAL_HH
#define OST_QA_REDUCED_POTENTIAL_HH

#include <ost/qa/reduced_statistics.hh>


namespace ost { namespace qa {


class ReducedPotential;
  
typedef boost::shared_ptr<ReducedPotential> ReducedPotentialPtr;

typedef MultiClassifier<float, int, int, float, float> ReducedEnergies;
/// \brief reduced model statistical potential operating on the residue level
class DLLEXPORT_OST_QA ReducedPotential {
public:

  
  
  static ReducedPotentialPtr Create(const ReducedStatisticsPtr& stats);
  static ReducedPotentialPtr Load(const String& filename);
  
  void Save(const String& filename);
  
  
  template <typename DS>
  void Serialize(DS& ds);
  
  const ReducedStatOptions& GetOptions() const { return opts_; }
  
  Real GetTotalEnergy(ost::mol::EntityHandle ent, bool normalize=true);
  
  Real GetTotalEnergy(ost::mol::EntityView ent, bool normalize=true);
  static bool GetCAlphaCBetaPos(const mol::ResidueHandle& res, 
                                geom::Vec3& ca_pos, 
                                geom::Vec3& cb_pos);

  Real GetEnergy(AminoAcid aa_one, AminoAcid aa_two, 
                 Real ca_dist, Real angle) const
  {
    if (aa_one==Xxx || aa_two==Xxx) { return 0.0; }
    if (ca_dist<opts_.lower_cutoff || ca_dist>=opts_.upper_cutoff) {
      return 0.0;
    }
    if (angle<0.0 || angle>M_PI) { return 0.0; }
    return energies_.Get(aa_one, aa_two, ca_dist, angle);
  }
private:
  void InitFromStats(const ReducedStatisticsPtr& stats);
  ReducedPotential() { }
  
  ReducedStatOptions opts_;
  ReducedEnergies    energies_;
};


}}

#endif
