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
using namespace boost::python;

#include <ost/qa/solis_torsion_statistics.hh>
#include <ost/qa/torsion_statistics.hh>
#include <ost/mol/mol.hh>
#include <boost/python/suite/indexing/vector_indexing_suite.hpp>
#include <boost/python/register_ptr_to_python.hpp>
#include <ost/qa/solis_torsion_potential.hh>
#include <ost/qa/torsion_potential.hh>

using namespace ost::qa;  
using namespace ost;


namespace {
  typedef Real (TorsionStatisticsSolis::*ADDMethod)(AminoAcid,
                                                 Real, Real) const;
  typedef Real (TorsionStatisticsSolis::*DDMethod)(Real, Real) const;
  typedef void (TorsionStatisticsSolis::*CMethod)(mol::EntityView);
  typedef void (TorsionStatisticsSolis::*EMethod)(mol::EntityHandle);
  ADDMethod GetF1 =  &TorsionStatisticsSolis::GetFrequency;
  DDMethod GetF2 = &TorsionStatisticsSolis::GetFrequency;
  CMethod Extract1 =  &TorsionStatisticsSolis::Extract;
  EMethod Extract2 =  &TorsionStatisticsSolis::Extract;
  
Real (TorsionPotentialSolis::*total_en_view_solis)(mol::EntityView)=
        &TorsionPotentialSolis::GetTotalEnergy;
Real (TorsionPotentialSolis::*total_en_handl_solis)(mol::EntityHandle)=
        &TorsionPotentialSolis::GetTotalEnergy;



  typedef void (TorsionStatistics::*XMethod)(mol::EntityView);
  typedef void (TorsionStatistics::*YMethod)(mol::EntityHandle);
  XMethod Extract3 =  &TorsionStatistics::Extract;
  YMethod Extract4 =  &TorsionStatistics::Extract;

Real (TorsionPotential::*total_en_view)(mol::EntityView)=
        &TorsionPotential::GetTotalEnergy;
Real (TorsionPotential::*total_en_handl)(mol::EntityHandle)=
        &TorsionPotential::GetTotalEnergy;
}

void export_Torsion()
{

  enum_<AminoAcid>("AminoAcid")
    .value("Ala", Ala)
    .value("Arg", Arg)
    .value("Asn", Asn)
    .value("Asp", Asp)
    .value("Gln", Gln) 
    .value("Glu", Glu)
    .value("Lys", Lys)
    .value("Ser", Ser)
    .value("Cys", Cys)
    .value("Tyr", Tyr)
    .value("Thr", Thr)
    .value("Val", Val)
    .value("Ile", Ile)
    .value("Leu", Leu)
    .value("Gly", Gly)
    .value("Pro", Pro)
    .value("Met", Met)
    .value("His", His)
    .value("Phe", Phe)
    .value("Trp", Trp)
    .export_values()
  ;
  
  class_<AminoAcidSet>("AminoAcidSet", init<>())
    .def("Add", &AminoAcidSet::Add)
    .def("Remove", &AminoAcidSet::Remove)
    .def("Contains", &AminoAcidSet::Contains)
    .def("Empty", &AminoAcidSet::Empty)
    .def("CreatePolarSet", 
         &AminoAcidSet::CreatePolarSet).staticmethod("CreatePolarSet")
    .def("CreateApolarSet", 
         &AminoAcidSet::CreateApolarSet).staticmethod("CreateApolarSet")
    .def("CreateAromaticSet", 
         &AminoAcidSet::CreateAromaticSet).staticmethod("CreateAromaticSet")
    .def("CreateThreeStateSet", 
         &AminoAcidSet::CreateThreeStateSet).staticmethod("CreateThreeStateSet")
    .def("CreatePseudoSet", 
         &AminoAcidSet::CreatePseudoSet).staticmethod("CreatePseudoSet")
    .def("CreateCompleteSet", 
         &AminoAcidSet::CreateCompleteSet).staticmethod("CreateCompleteSet")
    .def("CreateSet", &AminoAcidSet::CreateSet).staticmethod("CreateSet")
    .def(self_ns::str(self))
  ;
  
  class_<AminoAcidAlphabet>("AminoAcidAlphabet", init<>())
    .def(vector_indexing_suite<AminoAcidAlphabet>())
  ;
  

  class_<TorsionPotentialOptsSolis>("TorsionPotentialOptsSolis", init<>())
    .def_readwrite("AngularBucketSize", 
                   &TorsionPotentialOptsSolis::angular_bucket_size)
    .def_readwrite("OuterAlphabet", &TorsionPotentialOptsSolis::outer_alphabet)
    .def_readwrite("InnerAlphabet", &TorsionPotentialOptsSolis::inner_alphabet)
  ;  

    class_<TorsionPotentialOpts>("TorsionPotentialOpts", init<>())
    .def_readwrite("PrevAngularBucketSize",
                   &TorsionPotentialOpts::prev_angular_bucket_size)
    .def_readwrite("CentralAngularBucketSize",
               &TorsionPotentialOpts::central_angular_bucket_size)
    .def_readwrite("NextAngularBucketSize",
           &TorsionPotentialOpts::next_angular_bucket_size)
    .def_readwrite("Alphabet", &TorsionPotentialOpts::alphabet)
  ;

  
  class_<TorsionPotentialSolis, boost::noncopyable>("TorsionPotentialSolis", no_init)
    .def("Create", &TorsionPotentialSolis::Create).staticmethod("Create")
    .def("LoadFromFile", &TorsionPotentialSolis::LoadFromFile).staticmethod("LoadFromFile")
    .def("SaveToFile", &TorsionPotentialSolis::SaveToFile)
    .def("GetEnergyCounts", &TorsionPotentialSolis::GetEnergyCounts)
    .def("GetTotalEnergy", total_en_view_solis)
    .def("GetTotalEnergy", total_en_handl_solis)
  ;

    class_<TorsionPotential, boost::noncopyable>("TorsionPotential", no_init)
    .def("Create", &TorsionPotential::Create).staticmethod("Create")
    .def("LoadFromFile", &TorsionPotential::LoadFromFile).staticmethod("LoadFromFile")
    .def("SaveToFile", &TorsionPotential::SaveToFile)
    .def("GetEnergyCounts", &TorsionPotential::GetEnergyCounts)
    .def("GetTotalEnergy", total_en_view)
    .def("GetTotalEnergy", total_en_handl)
  ;


  register_ptr_to_python<TorsionPotentialSolisPtr>();
  register_ptr_to_python<TorsionStatisticsSolisPtr>();
  class_<TorsionStatisticsSolis>("TorsionStatisticsSolis", init<>())
    .def(init<int>(args("torsion_bucket_size")))
    .def("Extract", Extract2, args("model"))
    .def("Extract", Extract1, args("chain"))    
    .def("GetFrequency", GetF1,
         arg("amino_acid"), arg("phi_angle"), arg("psi_angle"))
    .def("GetFrequency", GetF2,
         arg("phi_angle"), arg("psi_angle"))     
    .def("SaveToFile", &TorsionStatisticsSolis::SaveToFile, arg("file_name"))
    .def("LoadFromFile", 
         &TorsionStatisticsSolis::LoadFromFile,
         arg("file_name")).staticmethod("LoadFromFile")
  ;

  register_ptr_to_python<TorsionPotentialPtr>();
  register_ptr_to_python<TorsionStatisticsPtr>();
  class_<TorsionStatistics>("TorsionStatistics", init<>())
    .def(init<int,int,int>(args("prev_torsion_bucket_size",
                        "central_torsion_bucket_size",
                        "next_torsion_bucket_size")))
    .def("Extract", Extract4, args("model"))
    .def("Extract", Extract3, args("chain"))
    .def("SaveToFile", &TorsionStatistics::SaveToFile, arg("file_name"))
    .def("LoadFromFile",
         &TorsionStatistics::LoadFromFile,
         arg("file_name")).staticmethod("LoadFromFile")
  ;
}
