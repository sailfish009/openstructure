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
#include <OpenMM.h>
#include <boost/python.hpp>
#include <boost/shared_ptr.hpp>
#include <ost/mol/mm/force_field.hh>
#include <ost/mol/mm/simulation.hh>
#include <ost/mol/mm/mm_algorithms.hh>
#include <ost/mol/residue_handle.hh>
#include <ost/mol/mm/mm_settings.hh>

using namespace boost::python;
using namespace ost::mol::mm;

namespace{

std::vector<std::pair<String, ost::mol::ResNum> > list_to_vec(boost::python::list& l){
  std::vector<std::pair<String,ost::mol::ResNum> > return_vec;
  boost::python::tuple t;
  String s;
  ost::mol::ResNum rn;
  for(int i = 0; i < boost::python::len(l); ++i){
    t = boost::python::extract<boost::python::tuple>(l[0]);
    s = boost::python::extract<String>(t[0]);
    rn = boost::python::extract<ost::mol::ResNum>(t[1]);
    return_vec.push_back(std::make_pair(s,rn));
  }
  return return_vec;
}

ost::mol::mm::SimulationPtr WrapSimulationConstructor(ost::mol::EntityHandle& e, ost::mol::mm::ForcefieldPtr ff,
                                                       ost::mol::mm::IntegratorPtr integrator, ost::mol::mm::MMSettingsPtr settings,
                                                       list& d_protonated_his,
                                                       list& e_protonated_his,
                                                       list& positive_his,
                                                       list& neutral_glutamates,
                                                       list& neutral_aspartates){

 std::vector<std::pair<String,ost::mol::ResNum> > v_d_protonated_his = list_to_vec(d_protonated_his);
 std::vector<std::pair<String,ost::mol::ResNum> > v_e_protonated_his = list_to_vec(e_protonated_his);
 std::vector<std::pair<String,ost::mol::ResNum> > v_positive_his = list_to_vec(positive_his);
 std::vector<std::pair<String,ost::mol::ResNum> > v_neutral_glutamates = list_to_vec(neutral_glutamates);
 std::vector<std::pair<String,ost::mol::ResNum> > v_neutral_aspartates = list_to_vec(neutral_aspartates);

 ost::mol::mm::SimulationPtr p(new ost::mol::mm::Simulation(e,ff,integrator, settings,
                                                              v_d_protonated_his,
                                                              v_e_protonated_his,
                                                              v_positive_his,
                                                              v_neutral_glutamates,
                                                              v_neutral_aspartates));
 return p;
}

ost::mol::mm::SimulationPtr WrapSimulationConstructorNoSettings(ost::mol::EntityHandle& e, ost::mol::mm::ForcefieldPtr ff,
                                                       ost::mol::mm::IntegratorPtr integrator,
                                                       list& d_protonated_his,
                                                       list& e_protonated_his,
                                                       list& positive_his,
                                                       list& neutral_glutamates,
                                                       list& neutral_aspartates){

  ost::mol::mm::MMSettingsPtr p(new ost::mol::mm::MMSettings);
  return WrapSimulationConstructor(e,ff,integrator,p,
                                   d_protonated_his,
                                   e_protonated_his,
                                   positive_his,
                                   neutral_glutamates,
                                   neutral_aspartates);


}

void WrapAssignFFSpecificNames(ost::mol::mm::ForcefieldPtr p,
                               ost::mol::EntityHandle& e,
                               boost::python::list& d_protonated_his,
                               boost::python::list& e_protonated_his,
                               boost::python::list& positive_his,
                               boost::python::list& neutral_glutamates,
                               boost::python::list& neutral_aspartates){

 std::vector<std::pair<String,ost::mol::ResNum> > v_d_protonated_his = list_to_vec(d_protonated_his);
 std::vector<std::pair<String,ost::mol::ResNum> > v_e_protonated_his = list_to_vec(e_protonated_his);
 std::vector<std::pair<String,ost::mol::ResNum> > v_positive_his = list_to_vec(positive_his);
 std::vector<std::pair<String,ost::mol::ResNum> > v_neutral_glutamates = list_to_vec(neutral_glutamates);
 std::vector<std::pair<String,ost::mol::ResNum> > v_neutral_aspartates = list_to_vec(neutral_aspartates);

 p->AssignFFSpecificNames(e,v_d_protonated_his,
                          v_e_protonated_his,
                          v_positive_his,
                          v_neutral_glutamates,
                          v_neutral_aspartates);

}

}



void export_MM()
{

  //export the settings

  class_<ost::mol::mm::MMSettings>("MMSettings",init<>())
    .def_readwrite("add_bonds",&ost::mol::mm::MMSettings::add_bonds)
    .def_readwrite("add_angles",&ost::mol::mm::MMSettings::add_angles)
    .def_readwrite("add_dihedrals",&ost::mol::mm::MMSettings::add_dihedrals)
    .def_readwrite("add_impropers",&ost::mol::mm::MMSettings::add_impropers)
    .def_readwrite("add_cmaps",&ost::mol::mm::MMSettings::add_cmaps)
    .def_readwrite("add_exclusions",&ost::mol::mm::MMSettings::add_exclusions)
    .def_readwrite("add_nonbonded",&ost::mol::mm::MMSettings::add_nonbonded)
    .def_readwrite("kill_electrostatics",&ost::mol::mm::MMSettings::kill_electrostatics)
    .def_readwrite("nonbonded_method",&ost::mol::mm::MMSettings::nonbonded_method)
    .def_readwrite("nonbonded_cutoff",&ost::mol::mm::MMSettings::nonbonded_cutoff)
    .def_readwrite("rigid_water",&ost::mol::mm::MMSettings::rigid_water)
    .def_readwrite("removeCMMMotion",&ost::mol::mm::MMSettings::removeCMMMotion)
    .def_readwrite("constraints",&ost::mol::mm::MMSettings::constraints)
  ;

  boost::python::register_ptr_to_python<ost::mol::mm::MMSettingsPtr>();

  //we have to tell boost, that the Integrator class is around...
  class_<OpenMM::Integrator, boost::noncopyable>("Integrator",no_init);
  //We do not export any further function for the Verlet integrator, since we want
  //to perform the steps via the simulation...
  class_<OpenMM::VerletIntegrator, bases<OpenMM::Integrator> >("VerletIntegrator", init<Real>());
  boost::python::register_ptr_to_python<ost::mol::mm::IntegratorPtr>();
  boost::python::register_ptr_to_python<ost::mol::mm::VerletIntegratorPtr>();

  class_<ost::mol::mm::Forcefield>("Forcefield", init<const String&>())
    .def("AssignFFSpecificNames", &WrapAssignFFSpecificNames, (arg("entity"),
                                                               arg("d_protonated_his") = boost::python::list(),
                                                               arg("e_protonated_his") = boost::python::list(),
                                                               arg("positive_his") = boost::python::list(),
                                                               arg("neutral_glutamates") = boost::python::list(),
                                                               arg("neutral_aspartates") = boost::python::list()))
    .def("AddHydrogens", &ost::mol::mm::Forcefield::AddHydrogens)
    .def("ReadResidueDatabase", &ost::mol::mm::Forcefield::ReadResidueDatabase)
  ;

  class_<ost::mol::mm::Simulation>("Simulation",no_init)
    .def("__init__",boost::python::make_constructor(&WrapSimulationConstructor,default_call_policies(),
                                                (arg("entity"),arg("forcefield"),
                                                 arg("integrator"),arg("settings"),
                                                 arg("d_protonated_his") = boost::python::list(),
                                                 arg("e_protonated_his") = boost::python::list(),
                                                 arg("positive_his") = boost::python::list(),
                                                 arg("neutral_glutamates") = boost::python::list(),
                                                 arg("neutral_aspartates") = boost::python::list())))
    .def("__init__",boost::python::make_constructor(&WrapSimulationConstructorNoSettings,default_call_policies(),
                                                (arg("entity"),arg("forcefield"),
                                                 arg("integrator"),
                                                 arg("d_protonated_his") = boost::python::list(),
                                                 arg("e_protonated_his") = boost::python::list(),
                                                 arg("positive_his") = boost::python::list(),
                                                 arg("neutral_glutamates") = boost::python::list(),
                                                 arg("neutral_aspartates") = boost::python::list())))
    .def("GetEntity",&ost::mol::mm::Simulation::GetEntity)
    .def("GetMMEntity",&ost::mol::mm::Simulation::GetMMEntity)
    .def("UpdatePositions",&ost::mol::mm::Simulation::UpdatePositions)
    .def("MinimizeEnergy",&ost::mol::mm::Simulation::MinimizeEnergy, (arg("tolerance")=1.0,arg("max_iterations")=1000))
    .def("GetTimeSteps",&ost::mol::mm::Simulation::GetTimeSteps)
    .def("Steps",&ost::mol::mm::Simulation::Steps, (arg("steps")=1))                                                                              
  ;

  boost::python::register_ptr_to_python<ost::mol::mm::SimulationPtr>();

  //functions from mm_algorithms
  def("Minimize",&ost::mol::mm::Minimize, (arg("ent"), arg("ff")));   

}


