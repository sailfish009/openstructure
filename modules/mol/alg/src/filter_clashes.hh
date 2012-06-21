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

/// \brief List of reference atom-atom distances to detect clashes between non-bonded atoms 
class ClashingDistances
{

public:
  /// \brief Default constructor (creates an empty list)
  ClashingDistances(): valid_flag_(true) {}

  /// \brief Adds or replaces an entry 
  ///
  /// Requires an atom-atom distance and a tolerance threshold
  void SetClashingDistance(const String& ele1,const String& ele2, Real min_distance, Real tolerance);

  /// \brief Recovers a reference distance and a tolerance threshold (respectively) from the list
  std::pair<Real,Real> GetClashingDistance(const String& ele1,const String& ele2) const;

  /// \brief Recovers the longest distance in the list, corrected by tolerance
  Real GetMaxAdjustedDistance() const;

  /// \brief Returns true if the list is empty (i.e. in an invalid, useless state)
  bool IsEmpty() const;
  
  /// \brief Prints all distances in the list to standard output
  void PrintAllDistances() const;
  
private:

  std::map <String,std::pair<float,float> > min_distance_;
  Real default_min_distance_;
  Real default_min_distance_tolerance_;
  bool valid_flag_;
  
};

/// \brief List of stereo chemical parameters (Bonds and angles)
///
/// For each item (bond or angle in a specific residue), stores the mean and standard deviation 
class StereoChemicalParams
{

public:
  /// \brief Adds or replaces an entry 
  void SetParam(const String& param, const String& residue, Real value, Real st_dev);

  /// \brief Recovers mean and standard deviation (respectively) of a stereo⁻chemical item (bond or angle) from the list
  ///
  /// Item format: Bond: X-Y, Angle:X-Y-Z
  std::pair<Real,Real> GetParam(const String& element,const String& residue) const;

  /// \brief Checks if the list contains an entry for a specific stereo-chemical item (a bond or atom in a specific residue)
  ///
  /// Item format: Bond: X-Y, Angle:X-Y-Z
  bool ContainsParam(const String& param,const String& residue) const;

  /// \brief Returns true if the list is empty (i.e. in an invalid, useless state)
  ///
  /// Item format: Bond: X-Y, Angle:X-Y-Z
  bool IsEmpty() const;
  
  /// \brief Prints all distances in the list to standard output
  void PrintAllParameters() const;
  
private:

  std::map<std::pair<String,String>,std::pair<float,float> >  params_;
 
};

/// \brief Fills a list of reference clashing distances from the content of a parameter file
///
/// Requires a list of strings holding the contents of a parameter file, one line per string
ClashingDistances DLLEXPORT_OST_MOL_ALG FillClashingDistances(std::vector<String>& stereo_chemical_props_file);

/// \brief Fills a list of stereo-chemical statistics from the content of a parameter file
///
/// Requires a list of strings holding the contents of a parameter file, one line per string
/// The header can be 'Bonds' to read bond statistics or 'Angles' to read angle statistics
StereoChemicalParams DLLEXPORT_OST_MOL_ALG FillStereoChemicalParams(const String& header, std::vector<String>& stereo_chemical_props_file);  

/// \brief Filters a structure based on detected clashes between non bonded atoms. Entity version
///
/// If a clash between two atoms (distance shorter than reference clashing distance - tolerance threshold)
/// is detected in a residue's side-chain, all atoms in the side chain are removed from the structure
/// If a clash is detected in the backbone, all atoms in the residue are removed. This behavior is changed 
/// by the always_remove_bb flag: when the flag is set to true all atoms in the residue are removed even if
/// a clash is just detected in the side-chain
EntityView DLLEXPORT_OST_MOL_ALG FilterClashes(const EntityView& ent, 
                                               const ClashingDistances& min_distances, bool always_remove_bb=false);

/// \brief Filters a structure based on detected clashes between non bonded atoms. Handle version
///
/// If a clash between two atoms (distance shorter than reference clashing distance - tolerance threshold)
/// is detected in a residue's side-chain, all atoms in the side chain are removed from the structure
/// If a clash is detected in the backbone, all atoms in the residue are removed. This behavior is changed 
/// by the always_remove_bb flag: when the flag is set to true all atoms in the residue are removed even if
/// a clash is just detected in the side-chain
EntityView DLLEXPORT_OST_MOL_ALG FilterClashes(const EntityHandle& ent, 
                                               const ClashingDistances& min_distances, bool always_remove_bb=false);

/// \brief Filters a structure based on detected stereo-chemical violations. Entity version
///
/// If a stereo-chemical violation (i.e., a bond or an angle with a value outside the range defined by
/// the mean value, the standard deviation and the tolerance parameter) is detected in a residue's side-chain, 
/// all atoms in the side chain are removed from the structure. If a violation is detected in the backbone, all 
/// atoms in the residue are removed. This behavior is changed by the always_remove_bb flag: when the flag is 
/// set to true all atoms in the residue are removed even if a violation is just detected in the side-chain
EntityView DLLEXPORT_OST_MOL_ALG CheckStereoChemistry(const EntityView& ent, 
                                                      const StereoChemicalParams& bond_table, 
                                                      const StereoChemicalParams& angle_table,
                                                      Real bond_tolerance,
                                                      Real angle_tolerance,
                                                      bool always_remove_bb=false);

/// \brief Filters a structure based on detected stereo-chemical violations. Handle version
///
/// If a stereo-chemical violation (i.e., a bond or an angle with a value outside the range defined by
/// the mean value, the standard deviation and the tolerance parameter) is detected in a residue's side-chain, 
/// all atoms in the side chain are removed from the structure. If a violation is detected in the backbone, all 
/// atoms in the residue are removed. This behavior is changed by the always_remove_bb flag: when the flag is 
/// set to true all atoms in the residue are removed even if a violation is just detected in the side-chain
EntityView DLLEXPORT_OST_MOL_ALG CheckStereoChemistry(const EntityHandle& ent, 
                                                      const StereoChemicalParams& bond_table, 
                                                      const StereoChemicalParams& angle_table,
                                                      Real bond_tolerance,
                                                      Real angle_tolerance,
                                                      bool always_remove_bb=false);


}}}


#endif
