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
#include <boost/python.hpp>
using namespace boost::python;

#include <ost/qa/interaction_statistics.hh>
#include <ost/qa/all_atom_potential.hh>
#include <boost/python/register_ptr_to_python.hpp>
using namespace ost::qa;  
using namespace ost;

namespace {
typedef uint64_t(InteractionStatistics::*F1)(Real) const;  
typedef uint64_t(InteractionStatistics::*F2)(atom::ChemType, 
                                           atom::ChemType, Real) const;  
F1 f1=&InteractionStatistics::GetCount;
F2 f2=&InteractionStatistics::GetCount;



typedef float (AllAtomPotential::*oneArg)(mol::EntityView);
typedef float (AllAtomPotential::*twoArgs)(mol::EntityView,
                                          mol::EntityView);

typedef float (AllAtomPotential::*threeArgs)(atom::ChemType,
                                             atom::ChemType, float);

oneArg one_arg =&AllAtomPotential::GetTotalEnergy;
twoArgs two_args =&AllAtomPotential::GetTotalEnergy;
threeArgs three_args =&AllAtomPotential::GetEnergy;
}

void export_Interaction()
{
  enum_<atom::ChemType>("ChemType")
    .value("O_GLY", atom::O_GLY)
    .value("N_GLY", atom::N_GLY)
    .value("C_GLY", atom::C_GLY)    
    .value("C_GLY_A", atom::C_GLY_A)

    .value("O_ALA", atom::O_ALA)
    .value("N_ALA", atom::N_ALA)
    .value("C_ALA", atom::C_ALA)    
    .value("C_ALA_A", atom::C_ALA_A)
    .value("C_ALA_B", atom::C_ALA_B)

    .value("O_VAL", atom::O_VAL)
    .value("N_VAL", atom::N_VAL)
    .value("C_VAL", atom::C_VAL)    
    .value("C_VAL_A", atom::C_VAL_A)
    .value("C_VAL_B", atom::C_VAL_B)
    .value("C_VAL_G", atom::C_VAL_G)

    .value("O_LEU", atom::O_LEU)
    .value("N_LEU", atom::N_LEU)
    .value("C_LEU", atom::C_LEU)  
    .value("C_LEU_A", atom::C_LEU_A)
    .value("C_LEU_B", atom::C_LEU_B)
    .value("C_LEU_G", atom::C_LEU_G)
    .value("C_LEU_D", atom::C_LEU_D)  

    .value("O_ILE", atom::O_ILE)
    .value("N_ILE", atom::N_ILE)
    .value("C_ILE", atom::C_ILE)
    .value("C_ILE_A", atom::C_ILE_A)
    .value("C_ILE_B", atom::C_ILE_B)
    .value("C_ILE_G1", atom::C_ILE_G1)
    .value("C_ILE_G2", atom::C_ILE_G2)
    .value("C_ILE_D1", atom::C_ILE_D1)

    .value("O_THR", atom::O_THR)
    .value("N_THR", atom::N_THR)
    .value("C_THR", atom::C_THR)
    .value("C_THR_A", atom::C_THR_A)
    .value("C_THR_B", atom::C_THR_B)
    .value("O_THR_G1", atom::O_THR_G1)
    .value("C_THR_G2", atom::C_THR_G2)

    .value("O_SER", atom::O_SER)
    .value("N_SER", atom::N_SER)
    .value("C_SER", atom::C_SER)
    .value("C_SER_A", atom::C_SER_A)
    .value("C_SER_B", atom::C_SER_B)
    .value("O_SER_G", atom::O_SER_G)

    .value("O_CYS", atom::O_CYS)
    .value("N_CYS", atom::N_CYS)
    .value("C_CYS", atom::C_CYS)
    .value("C_CYS_A", atom::C_CYS_A)
    .value("C_CYS_B", atom::C_CYS_B)
    .value("S_CYS_G", atom::S_CYS_G)    

    .value("O_MET", atom::O_MET)
    .value("N_MET", atom::N_MET)
    .value("C_MET", atom::C_MET)
    .value("C_MET_A", atom::C_MET_A)
    .value("C_MET_B", atom::C_MET_B)
    .value("C_MET_G", atom::C_MET_G)
    .value("S_MET_D", atom::S_MET_D)
    .value("C_MET_E", atom::C_MET_E)

    .value("O_ASP", atom::O_ASP)
    .value("N_ASP", atom::N_ASP)
    .value("C_ASP", atom::C_ASP)
    .value("C_ASP_A", atom::C_ASP_A)
    .value("C_ASP_B", atom::C_ASP_B)
    .value("C_ASP_G", atom::C_ASP_G)
    .value("O_ASP_D", atom::O_ASP_D)

    .value("O_GLU", atom::O_GLU)
    .value("N_GLU", atom::N_GLU)
    .value("C_GLU", atom::C_GLU)
    .value("C_GLU_A", atom::C_GLU_A)
    .value("C_GLU_B", atom::C_GLU_B)
    .value("C_GLU_G", atom::C_GLU_G)
    .value("C_GLU_D", atom::C_GLU_D)
    .value("O_GLU_E", atom::O_GLU_E)

    .value("O_ASN", atom::O_ASN)
    .value("N_ASN", atom::N_ASN)
    .value("C_ASN", atom::C_ASN)
    .value("C_ASN_A", atom::C_ASN_A)
    .value("C_ASN_B", atom::C_ASN_B)
    .value("C_ASN_G", atom::C_ASN_G)
    .value("O_ASN_D", atom::O_ASN_D)
    .value("N_ASN_D", atom::N_ASN_D)

    .value("O_GLN", atom::O_GLN)
    .value("N_GLN", atom::N_GLN)
    .value("C_GLN", atom::C_GLN)
    .value("C_GLN_A", atom::C_GLN_A)
    .value("C_GLN_B", atom::C_GLN_B)
    .value("C_GLN_G", atom::C_GLN_G)
    .value("C_GLN_D", atom::C_GLN_D)
    .value("O_GLN_E", atom::O_GLN_E)
    .value("N_GLN_E", atom::N_GLN_E)   

    .value("O_LYS", atom::O_LYS)
    .value("N_LYS", atom::N_LYS)
    .value("C_LYS", atom::C_LYS)
    .value("C_LYS_A", atom::C_LYS_A)
    .value("C_LYS_B", atom::C_LYS_B)
    .value("C_LYS_G", atom::C_LYS_G)
    .value("C_LYS_D", atom::C_LYS_D)
    .value("C_LYS_E", atom::C_LYS_E)
    .value("N_LYS_Z", atom::N_LYS_Z)     

    .value("O_ARG", atom::O_ARG)
    .value("N_ARG", atom::N_ARG)
    .value("C_ARG", atom::C_ARG)
    .value("C_ARG_A", atom::C_ARG_A)
    .value("C_ARG_B", atom::C_ARG_B)
    .value("C_ARG_G", atom::C_ARG_G)
    .value("C_ARG_D", atom::C_ARG_D)
    .value("N_ARG_E", atom::N_ARG_E)
    .value("C_ARG_Z", atom::C_ARG_Z)    
    .value("N_ARG_H", atom::N_ARG_H)

    .value("O_TYR", atom::O_TYR)
    .value("N_TYR", atom::N_TYR)
    .value("C_TYR", atom::C_TYR)
    .value("C_TYR_A", atom::C_TYR_A)
    .value("C_TYR_B", atom::C_TYR_B)
    .value("C_TYR_G", atom::C_TYR_G)
    .value("C_TYR_D", atom::C_TYR_D)
    .value("C_TYR_E", atom::C_TYR_E)
    .value("C_TYR_Z", atom::C_TYR_Z)
    .value("O_TYR_H", atom::O_TYR_H)

    .value("O_PHE", atom::O_PHE)
    .value("N_PHE", atom::N_PHE)
    .value("C_PHE", atom::C_PHE)
    .value("C_PHE_A", atom::C_PHE_A)
    .value("C_PHE_B", atom::C_PHE_B)
    .value("C_PHE_G", atom::C_PHE_G)
    .value("C_PHE_D", atom::C_PHE_D)
    .value("C_PHE_E", atom::C_PHE_E)
    .value("C_PHE_Z", atom::C_PHE_Z)

    .value("O_HIS", atom::O_HIS)
    .value("N_HIS", atom::N_HIS)
    .value("C_HIS", atom::C_HIS)
    .value("C_HIS_A", atom::C_HIS_A)
    .value("C_HIS_B", atom::C_HIS_B)
    .value("C_HIS_G", atom::C_HIS_G) 
    .value("N_HIS_D1", atom::N_HIS_D1)
    .value("C_HIS_D2", atom::C_HIS_D2)
    .value("C_HIS_E1", atom::C_HIS_E1)
    .value("N_HIS_E2", atom::N_HIS_E2)

    .value("O_TRP", atom::O_TRP)
    .value("N_TRP", atom::N_TRP)
    .value("C_TRP", atom::C_TRP)
    .value("C_TRP_A", atom::C_TRP_A)
    .value("C_TRP_B", atom::C_TRP_B)
    .value("C_TRP_G", atom::C_TRP_G) 
    .value("C_TRP_D1", atom::C_TRP_D1)
    .value("C_TRP_D2", atom::C_TRP_D2)
    .value("N_TRP_E1", atom::N_TRP_E1)
    .value("C_TRP_E2", atom::C_TRP_E2) 
    .value("C_TRP_E3", atom::C_TRP_E3) 
    .value("C_TRP_Z2", atom::C_TRP_Z2)
    .value("C_TRP_Z3", atom::C_TRP_Z3)     
    .value("C_TRP_H2", atom::C_TRP_H2)
    
    .value("O_PRO", atom::O_PRO)
    .value("N_PRO", atom::N_PRO)
    .value("C_PRO", atom::C_PRO)
    .value("C_PRO_A", atom::C_PRO_A)
    .value("C_PRO_B", atom::C_PRO_B)
    .value("C_PRO_G", atom::C_PRO_G)
    .value("C_PRO_D", atom::C_PRO_D)
  ;
  
  class_<InteractionStatistics>("InteractionStatistics", 
                                init<Real, Real, Real, int>(args("lower_cutoff","upper_cutoff", 
                                                                       "bucket_size", "sequence_sep")))
    .def("Extract", &InteractionStatistics::Extract, args("view"))
    .def("GetCount", f1, args("distance"))
    .def("GetCount", f2, args("partner1", "partner2", "distance"))
    .def("SaveToFile", &InteractionStatistics::SaveToFile, args("file_name"))
    .def("LoadFromFile", &InteractionStatistics::LoadFromFile).staticmethod("LoadFromFile")
    .def("RepairCbetaStatistics", &InteractionStatistics::RepairCbetaStatistics)
    .def("IsCBetaOnly", &InteractionStatistics::IsCBetaOnly)
  ;  
  
  class_<AllAtomPotentialOpts>("AllAtomPotentialOpts", init<>())
    .def_readwrite("LowerCutoff", &AllAtomPotentialOpts::lower_cutoff)
    .def_readwrite("UpperCutoff", &AllAtomPotentialOpts::upper_cutoff)
    .def_readwrite("DistanceBucketSize", 
                   &AllAtomPotentialOpts::distance_bucket_size)
    .def_readwrite("SequenceSeparation", &AllAtomPotentialOpts::sequence_sep)
    .def_readwrite("Sigma", &AllAtomPotentialOpts::sigma)
  ;
  class_<AllAtomPotential>("AllAtomPotential", no_init)
    .def("Create", &AllAtomPotential::Create).staticmethod("Create")
    .def("LoadFromFile", 
         &AllAtomPotential::LoadFromFile).staticmethod("LoadFromFile")    
    .def("GetTotalEnergy", one_arg, args("view"))
    .def("GetTotalEnergy", two_args, args("view", "target_view"))
    .def("GetEnergyCounts", &AllAtomPotential::GetEnergyCounts)
    .def("GetEnergy", three_args, args("atom_type_1", "atom_type_1", "distance"))
    .def("SaveToFile", &AllAtomPotential::SaveToFile)
    .def("SetSequenceSeparation", &AllAtomPotential::SetSequenceSeparation)
  ;
  register_ptr_to_python<AllAtomPotentialPtr>();  
  register_ptr_to_python<InteractionStatisticsPtr>();  
}
