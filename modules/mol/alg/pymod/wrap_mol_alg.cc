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
#include <ost/config.hh>
#include <ost/mol/alg/local_dist_test.hh>
#include <ost/mol/alg/superpose_frames.hh>
#include <ost/mol/alg/filter_clashes.hh>
using namespace boost::python;
using namespace ost;

void export_svdSuperPose();
void export_TrajectoryAnalysis();
void export_Clash();
#if OST_IMG_ENABLED
void export_entity_to_density();
#endif

namespace {
  
Real (*ldt_a)(const mol::EntityView&, const mol::EntityView& ref, Real, Real, const String&)=&mol::alg::LocalDistTest;
Real (*ldt_b)(const seq::AlignmentHandle&,Real, Real, int, int)=&mol::alg::LocalDistTest;
mol::EntityView (*fc_a)(const mol::EntityView&, const mol::alg::ClashingDistances&,bool)=&mol::alg::FilterClashes;
mol::EntityView (*fc_b)(const mol::EntityHandle&, const mol::alg::ClashingDistances&, bool)=&mol::alg::FilterClashes;
mol::EntityView (*csc_a)(const mol::EntityView&, const mol::alg::StereoChemicalParams&, const mol::alg::StereoChemicalParams&, Real, Real, bool)=&mol::alg::CheckStereoChemistry;
mol::EntityView (*csc_b)(const mol::EntityHandle&, const mol::alg::StereoChemicalParams&, const mol::alg::StereoChemicalParams&, Real, Real, bool)=&mol::alg::CheckStereoChemistry;
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

ost::mol::alg::ClashingDistances fill_clashing_distances_wrapper (const list& stereo_chemical_props_file, Real min_default_distance, Real min_distance_tolerance)  
{
  int stereo_chemical_props_file_length = boost::python::extract<int>(stereo_chemical_props_file.attr("__len__")());
  std::vector<String> stereo_chemical_props_file_vector(stereo_chemical_props_file_length);
  
  for (int i=0; i<stereo_chemical_props_file_length; i++) {
    stereo_chemical_props_file_vector[i] = boost::python::extract<char const*>(stereo_chemical_props_file[i]);
  }
 
 return ost::mol::alg::FillClashingDistances(stereo_chemical_props_file_vector,min_default_distance,min_distance_tolerance);
}

}



BOOST_PYTHON_MODULE(_ost_mol_alg)
{
  export_svdSuperPose();
  export_TrajectoryAnalysis();
  #if OST_IMG_ENABLED
  export_entity_to_density();
  #endif
  
  def("LocalDistTest", ldt_a, (arg("local_ldt_property_string")=""));
  def("LocalDistTest", ldt_b, (arg("ref_index")=0, arg("mdl_index")=1));
  def("FilterClashes", fc_a, (arg("ent"), arg("clashing_distances"), arg("always_remove_bb")=false));
  def("FilterClashes", fc_b, (arg("ent"), arg("clashing_distances"), arg("always_remove_bb")=false));
  def("CheckStereoChemistry", csc_a, (arg("ent"), arg("bonds"), arg("angles"), arg("bond_tolerance"), arg("angle_tolerance"), arg("always_remove_bb")=false));
  def("CheckStereoChemistry", csc_b, (arg("ent"), arg("bonds"), arg("angles"), arg("bond_tolerance"), arg("angle_tolerance"), arg("always_remove_bb")=false));
  def("LDTHA",&mol::alg::LDTHA);
    
  def("SuperposeFrames", superpose_frames1, 
      (arg("source"), arg("sel")=ost::mol::EntityView(), arg("begin")=0, 
       arg("end")=-1, arg("ref")=-1));
  def("SuperposeFrames", superpose_frames2, 
  (arg("source"), arg("sel")=ost::mol::EntityView(), arg("ref_view")=ost::mol::EntityView(),arg("begin")=0, 
   arg("end")=-1));

  
  class_<mol::alg::ClashingDistances> ("ClashingDistances" ,init<Real,Real>())
    .def("SetClashingDistance",&mol::alg::ClashingDistances::SetClashingDistance)
    .def("GetClashingDistance",&mol::alg::ClashingDistances::GetClashingDistance)
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
 
  def("FillClashingDistances",&fill_clashing_distances_wrapper);
  def("FillStereoChemicalParams",&fill_stereochemical_params_wrapper);
  
}