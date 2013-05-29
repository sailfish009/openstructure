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

#include <boost/python.hpp>
#include <boost/python/suite/indexing/map_indexing_suite.hpp>
#include <ost/config.hh>
#include <ost/mol/alg/local_dist_diff_test.hh>
#include <ost/mol/alg/distance_test_common.hh>
#include <ost/mol/alg/distance_rmsd_test.hh>
#include <ost/mol/alg/superpose_frames.hh>
#include <ost/mol/alg/filter_clashes.hh>
#include <ost/mol/alg/consistency_checks.hh>
#include <ost/mol/alg/pdbize.hh>
#include <ost/export_helper/pair_to_tuple_conv.hh>
#include <ost/export_helper/vec_to_list_conv.hh>

using namespace boost::python;
using namespace ost;

void export_svdSuperPose();
void export_TrajectoryAnalysis();
void export_StructureAnalysis();
void export_Clash();
#if OST_IMG_ENABLED
void export_entity_to_density();
#endif

namespace {
  
std::pair<long int,long int> (*lddt_a)(const mol::EntityView&, const mol::alg::GlobalRDMap& , std::vector<Real>, int, const String&)=&mol::alg::LocalDistDiffTest;
Real (*lddt_c)(const mol::EntityView&, const mol::EntityView& , Real, Real, const String&)=&mol::alg::LocalDistDiffTest;
Real (*lddt_b)(const seq::AlignmentHandle&,Real, Real, int, int)=&mol::alg::LocalDistDiffTest;
std::pair<mol::EntityView,mol::alg::ClashingInfo> (*fc_a)(const mol::EntityView&, const mol::alg::ClashingDistances&,bool)=&mol::alg::FilterClashes;
std::pair<mol::EntityView,mol::alg::ClashingInfo> (*fc_b)(const mol::EntityHandle&, const mol::alg::ClashingDistances&, bool)=&mol::alg::FilterClashes;
std::pair<mol::EntityView,mol::alg::StereoChemistryInfo> (*csc_a)(const mol::EntityView&, const mol::alg::StereoChemicalParams&, const mol::alg::StereoChemicalParams&, Real, Real, bool)=&mol::alg::CheckStereoChemistry;
std::pair<mol::EntityView,mol::alg::StereoChemistryInfo> (*csc_b)(const mol::EntityHandle&, const mol::alg::StereoChemicalParams&, const mol::alg::StereoChemicalParams&, Real, Real, bool)=&mol::alg::CheckStereoChemistry;
mol::CoordGroupHandle (*superpose_frames1)(mol::CoordGroupHandle&, mol::EntityView&, int, int, int)=&mol::alg::SuperposeFrames;
mol::CoordGroupHandle (*superpose_frames2)(mol::CoordGroupHandle&,  mol::EntityView&, mol::EntityView&, int, int)=&mol::alg::SuperposeFrames;

ost::mol::alg::StereoChemicalParams fill_stereochemical_params_wrapper (const String& header, const list& stereo_chemical_props_file)  
{
  int stereo_chemical_props_file_length = boost::python::extract<int>(stereo_chemical_props_file.attr("__len__")());
  std::vector<String> stereo_chemical_props_file_vector;
  
  for (int i=0; i<stereo_chemical_props_file_length; i++) {
    String extracted_string = boost::python::extract<String>(stereo_chemical_props_file[i]);
    stereo_chemical_props_file_vector.push_back(extracted_string);    
  }
  return ost::mol::alg::FillStereoChemicalParams(header,stereo_chemical_props_file_vector);
}

ost::mol::alg::ClashingDistances fill_clashing_distances_wrapper (const list& stereo_chemical_props_file)
{
  int stereo_chemical_props_file_length = boost::python::extract<int>(stereo_chemical_props_file.attr("__len__")());
  std::vector<String> stereo_chemical_props_file_vector(stereo_chemical_props_file_length);
  
  for (int i=0; i<stereo_chemical_props_file_length; i++) {
    stereo_chemical_props_file_vector[i] = boost::python::extract<char const*>(stereo_chemical_props_file[i]);
  }
 
 return ost::mol::alg::FillClashingDistances(stereo_chemical_props_file_vector);
}

ost::mol::alg::GlobalRDMap create_distance_list_from_multiple_references(const list& ref_list, const list& cutoff_list, int sequence_separation, Real max_dist)
{
  int ref_list_length = boost::python::extract<int>(ref_list.attr("__len__")());
  std::vector<ost::mol::EntityView> ref_list_vector(ref_list_length);
  
  for (int i=0; i<ref_list_length; i++) {
    ref_list_vector[i] = boost::python::extract<ost::mol::EntityView>(ref_list[i]);
  }

  int cutoff_list_length = boost::python::extract<int>(cutoff_list.attr("__len__")());
  std::vector<Real> cutoff_list_vector(cutoff_list_length);
  
  for (int i=0; i<cutoff_list_length; i++) {
    cutoff_list_vector[i] = boost::python::extract<Real>(cutoff_list[i]);
  }
  return ost::mol::alg::CreateDistanceListFromMultipleReferences(ref_list_vector, cutoff_list_vector, sequence_separation, max_dist);  	
}


}


BOOST_PYTHON_MODULE(_ost_mol_alg)
{
  export_svdSuperPose();
  export_TrajectoryAnalysis();
  export_StructureAnalysis();
  export_Clash();
  #if OST_IMG_ENABLED
  export_entity_to_density();
  #endif
  
  def("LocalDistDiffTest", lddt_a, (arg("sequence_separation")=0,arg("local_lddt_property_string")=""));
  def("LocalDistDiffTest", lddt_c, (arg("local_lddt_property_string")=""));
  def("LocalDistDiffTest", lddt_b, (arg("ref_index")=0, arg("mdl_index")=1));
  def("FilterClashes", fc_a, (arg("ent"), arg("clashing_distances"), arg("always_remove_bb")=false));
  def("FilterClashes", fc_b, (arg("ent"), arg("clashing_distances"), arg("always_remove_bb")=false));
  def("CheckStereoChemistry", csc_a, (arg("ent"), arg("bonds"), arg("angles"), arg("bond_tolerance"), arg("angle_tolerance"), arg("always_remove_bb")=false));
  def("CheckStereoChemistry", csc_b, (arg("ent"), arg("bonds"), arg("angles"), arg("bond_tolerance"), arg("angle_tolerance"), arg("always_remove_bb")=false));
  def("LDDTHA",&mol::alg::LDDTHA, (arg("sequence_separation")=0));
  def("CreateDistanceList",&mol::alg::CreateDistanceList);
  def("CreateDistanceListFromMultipleReferences",&create_distance_list_from_multiple_references);

  def("DistanceRMSDTest", &mol::alg::DistanceRMSDTest, (arg("sequence_separation")=0,arg("local_lddt_property_string")=""));

  def("SuperposeFrames", superpose_frames1, 
      (arg("source"), arg("sel")=ost::mol::EntityView(), arg("begin")=0, 
       arg("end")=-1, arg("ref")=-1));
  def("SuperposeFrames", superpose_frames2, 
  (arg("source"), arg("sel"), arg("ref_view"),arg("begin")=0, arg("end")=-1));

  
  class_<mol::alg::ClashingDistances> ("ClashingDistances",init<>())
    .def("SetClashingDistance",&mol::alg::ClashingDistances::SetClashingDistance)
    .def("GetClashingDistance",&mol::alg::ClashingDistances::GetClashingDistance)
    .def("GetAdjustedClashingDistance",&mol::alg::ClashingDistances::GetAdjustedClashingDistance)
    .def("GetMaxAdjustedDistance",&mol::alg::ClashingDistances::GetMaxAdjustedDistance)    
    .def("IsEmpty",&mol::alg::ClashingDistances::IsEmpty)  
    
    .def("PrintAllDistances",&mol::alg::ClashingDistances::PrintAllDistances)  
  ;

  class_<mol::alg::StereoChemicalParams> ("StereoChemicalParams" ,init<>())
    .def("SetParam",&mol::alg::StereoChemicalParams::SetParam)
    .def("GetParam",&mol::alg::StereoChemicalParams::GetParam)
    .def("ContainsParam",&mol::alg::StereoChemicalParams::ContainsParam)    
    .def("IsEmpty",&mol::alg::StereoChemicalParams::IsEmpty)  
    
    .def("PrintAllParameters",&mol::alg::StereoChemicalParams::PrintAllParameters)  

  ;

  class_<mol::alg::UniqueAtomIdentifier> ("UniqueAtomIdentifier" ,init <const String&, const mol::ResNum&, const String&, const String&>())
    .def("GetChainName",&mol::alg::UniqueAtomIdentifier::GetChainName)
    .def("GetResNum",&mol::alg::UniqueAtomIdentifier::GetResNum)
    .def("GetResidueName",&mol::alg::UniqueAtomIdentifier::GetResidueName)
    .def("GetAtomName",&mol::alg::UniqueAtomIdentifier::GetAtomName)
    .def("GetQualifiedAtomName",&mol::alg::UniqueAtomIdentifier::GetQualifiedAtomName)
  ;    
  
  def("FillClashingDistances",&fill_clashing_distances_wrapper);
  def("FillStereoChemicalParams",&fill_stereochemical_params_wrapper);
  def("IsStandardResidue",&mol::alg::IsStandardResidue);
  def("PrintGlobalRDMap",&mol::alg::PrintGlobalRDMap);
  def("PrintResidueRDMap",&mol::alg::PrintResidueRDMap);
 
  class_<mol::alg::PDBize>("PDBize",
                           init<int>(arg("min_polymer_size")=10))
    .def("Add", &mol::alg::PDBize::Add, 
         (arg("asu"), arg("transformations"), arg("seqres")))
    .def("Finish", &mol::alg::PDBize::Finish, arg("shift_to_fit")=true)
  ;

  def("ResidueNamesMatch",&mol::alg::ResidueNamesMatch,
      (arg("probe"), arg("reference"), arg("log_as_error")=false));

  class_<mol::alg::BondLengthInfo> ("BondLengthInfo" ,init <>())
    .def(init<Real,Real,int>())
    .def("GetAvgLength",&mol::alg::BondLengthInfo::GetAvgLength)
    .def("GetAvgZscore",&mol::alg::BondLengthInfo::GetAvgZscore)
    .def("GetCount",&mol::alg::BondLengthInfo::GetCount)
  ;

  class_<mol::alg::ClashEvent> ("ClashEvent" ,init <>())
      .def(init<const mol::alg::UniqueAtomIdentifier&,const mol::alg::UniqueAtomIdentifier&, Real, Real>())
      .def("GetFirstAtom",&mol::alg::ClashEvent::GetFirstAtom)
      .def("GetSecondAtom",&mol::alg::ClashEvent::GetSecondAtom)
      .def("GetModelDistance",&mol::alg::ClashEvent::GetModelDistance)
      .def("GetAdjustedReferenceDistance",&mol::alg::ClashEvent::GetAdjustedReferenceDistance)
  ;

  class_<mol::alg::StereoChemicalBondViolation> ("StereoChemicalBondViolation" ,init <>())
      .def(init<const mol::alg::UniqueAtomIdentifier&,const mol::alg::UniqueAtomIdentifier&,
           Real, const std::pair<Real, Real>& >())
      .def("GetFirstAtom",&mol::alg::StereoChemicalBondViolation::GetFirstAtom)
      .def("GetSecondAtom",&mol::alg::StereoChemicalBondViolation::GetSecondAtom)
      .def("GetBondLength",&mol::alg::StereoChemicalBondViolation::GetBondLength)
      .def("GetAllowedRange",&mol::alg::StereoChemicalBondViolation::GetAllowedRange)
  ;

  class_<mol::alg::StereoChemicalAngleViolation> ("StereoChemicalAngleViolation" ,init <>())
      .def(init<const mol::alg::UniqueAtomIdentifier&,const mol::alg::UniqueAtomIdentifier&,
          const mol::alg::UniqueAtomIdentifier&, Real, const std::pair<Real, Real>& >())
      .def("GetFirstAtom",&mol::alg::StereoChemicalAngleViolation::GetFirstAtom)
      .def("GetSecondAtom",&mol::alg::StereoChemicalAngleViolation::GetSecondAtom)
      .def("GetThirdAtom",&mol::alg::StereoChemicalAngleViolation::GetThirdAtom)
      .def("GetAngleWidth",&mol::alg::StereoChemicalAngleViolation::GetAngleWidth)
      .def("GetAllowedRange",&mol::alg::StereoChemicalAngleViolation::GetAllowedRange)
  ;

  class_<mol::alg::ClashingInfo> ("ClashingInfo" ,init <>())
    .def(init<int,Real,const std::vector<mol::alg::ClashEvent> >())
    .def("GetClashCount",&mol::alg::ClashingInfo::GetClashCount)
    .def("GetAverageOffset",&mol::alg::ClashingInfo::GetAverageOffset)
    .def("GetClashList",&mol::alg::ClashingInfo::GetClashList)
  ;

  class_<mol::alg::StereoChemistryInfo> ("StereoChemistryInfo" ,init <>())
      .def(init<Real,int,int,Real,int,int, const std::map<String,mol::alg::BondLengthInfo>&,
               const std::vector<mol::alg::StereoChemicalBondViolation>&,
               const std::vector<mol::alg::StereoChemicalAngleViolation>& >())
      .def("GetAvgZscoreBonds",&mol::alg::StereoChemistryInfo::GetAvgZscoreBonds)
      .def("GetBadBondCount",&mol::alg::StereoChemistryInfo::GetBadBondCount)
      .def("GetBondCount",&mol::alg::StereoChemistryInfo::GetBondCount)
      .def("GetAvgZscoreAngles",&mol::alg::StereoChemistryInfo::GetAvgZscoreAngles)
      .def("GetBadAngleCount",&mol::alg::StereoChemistryInfo::GetBadAngleCount)
      .def("GetAngleCount",&mol::alg::StereoChemistryInfo::GetAngleCount)
      .def("GetAvgBondLengthInfo",&mol::alg::StereoChemistryInfo::GetAvgBondLengthInfo)
      .def("GetBondViolationList",&mol::alg::StereoChemistryInfo::GetBondViolationList)
      .def("GetAngleViolationList",&mol::alg::StereoChemistryInfo::GetAngleViolationList)
    ;

  to_python_converter<std::pair<ost::mol::alg::UniqueAtomIdentifier,ost::mol::alg::UniqueAtomIdentifier>,
                      PairToTupleConverter<ost::mol::alg::UniqueAtomIdentifier, ost::mol::alg::UniqueAtomIdentifier> >();

  to_python_converter<std::pair<Real,Real>,
                      PairToTupleConverter<Real, Real> >();

  to_python_converter<std::pair<Real,long int>,
                      PairToTupleConverter<Real, long int> >();

  to_python_converter<std::pair<mol::EntityView,mol::alg::StereoChemistryInfo>,
                      PairToTupleConverter<mol::EntityView, mol::alg::StereoChemistryInfo> >();

  to_python_converter<std::pair<mol::EntityView,mol::alg::ClashingInfo>,
                      PairToTupleConverter<mol::EntityView, mol::alg::ClashingInfo> >();

  to_python_converter<std::vector<mol::alg::ClashEvent>,
                      VectorToListConverter<mol::alg::ClashEvent> >();

  to_python_converter<std::vector<mol::alg::StereoChemicalBondViolation>,
                      VectorToListConverter<mol::alg::StereoChemicalBondViolation> >();

  to_python_converter<std::vector<mol::alg::StereoChemicalAngleViolation>,
                      VectorToListConverter<mol::alg::StereoChemicalAngleViolation> >();

  class_<mol::alg::ResidueRDMap>("ResidueRDMap")
    .def(map_indexing_suite<mol::alg::ResidueRDMap,true>())
  ;

  class_<mol::alg::GlobalRDMap>("GlobalRDMap")
    .def(map_indexing_suite<mol::alg::GlobalRDMap,true>())
  ;

  def("DRMSD",&mol::alg::DRMSD);

}
