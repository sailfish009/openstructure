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
#ifndef OST_MOL_ALG_FILTER_CLASHES_HH
#define OST_MOL_ALG_FILTER_CLASHES_HH

#include <ost/mol/entity_view.hh>
#include <ost/mol/alg/module_config.hh>

namespace ost { namespace mol { namespace alg {

class ClashingDistances
{

public:
  ClashingDistances(Real default_dist, Real tolerance): default_min_distance_(default_dist), default_min_distance_tolerance_(tolerance), valid_flag_(true) {}
  void SetClashingDistance(const String& ele1,const String& ele2, Real min_distance, Real tolerance);
  std::pair<Real,Real> GetClashingDistance(const String& ele1,const String& ele2) const;
  Real GetMaxAdjustedDistance() const;
  bool IsEmpty() const;
  
  //DEBUG
  void PrintAllDistances() const;
  
private:

  std::map <String,std::pair<float,float> > min_distance_;
  Real default_min_distance_;
  Real default_min_distance_tolerance_;
  bool valid_flag_;
  
};
  
class StereoChemicalParams
{

public:
  void SetParam(const String& param, const String& residue, Real value, Real st_dev);
  std::pair<Real,Real> GetParam(const String& element,const String& residue) const;
  bool ContainsParam(const String& param,const String& residue) const;
  bool IsEmpty() const;
  
  //DEBUG
  void PrintAllParameters() const;
  
private:

  std::map<std::pair<String,String>,std::pair<float,float> >  params_;
 
};

ClashingDistances DLLEXPORT_OST_MOL_ALG FillClashingDistances(std::vector<String>& stereo_chemical_props_file, Real min_default_distance, Real min_distance_tolerance);
StereoChemicalParams DLLEXPORT_OST_MOL_ALG FillStereoChemicalParams(const String& header, std::vector<String>& stereo_chemical_props_file);  
 
EntityView DLLEXPORT_OST_MOL_ALG FilterClashes(const EntityView& ent, 
                                               const ClashingDistances& min_distances, bool always_remove_bb=false);

EntityView DLLEXPORT_OST_MOL_ALG FilterClashes(const EntityHandle& ent, 
                                               const ClashingDistances& min_distances, bool always_remove_bb=false);

EntityView DLLEXPORT_OST_MOL_ALG CheckStereoChemistry(const EntityView& ent, 
                                                      const StereoChemicalParams& bond_table, 
                                                      const StereoChemicalParams& angle_table,
                                                      Real bond_tolerance,
                                                      Real angle_tolerance,
                                                      bool always_remove_bb=false);

EntityView DLLEXPORT_OST_MOL_ALG CheckStereoChemistry(const EntityHandle& ent, 
                                                      const StereoChemicalParams& bond_table, 
                                                      const StereoChemicalParams& angle_table,
                                                      Real bond_tolerance,
                                                      Real angle_tolerance,
                                                      bool always_remove_bb=false);


}}}


#endif
