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
#include <ost/mol/mm/simulation.hh>

using namespace boost::python;



namespace{


template<typename T>
std::vector<T> ListToVec(const boost::python::list& l){
  std::vector<T> vec;

  for (int i = 0; i < boost::python::len(l); ++i){
    vec.push_back(boost::python::extract<T>(l[i]));
  }
  return vec;
}

void WrapAddPositionConstraints(ost::mol::mm::SimulationPtr p, const boost::python::list& l){
  std::vector<uint> vec  = ListToVec<uint>(l);
  p->AddPositionConstraints(vec);
}

}


void export_Simulation()
{

  class_<ost::mol::mm::Simulation>("Simulation",no_init)
    .def(init<const ost::mol::EntityHandle, const ost::mol::mm::SettingsPtr>())
    .def(init<const ost::mol::mm::TopologyPtr,const ost::mol::EntityHandle&,const ost::mol::mm::SettingsPtr>())
    .def("Save",&ost::mol::mm::Simulation::Save)
    .def("Load",&ost::mol::mm::Simulation::Load).staticmethod("Load")
    .def("Steps",&ost::mol::mm::Simulation::Steps)
    .def("GetPositions",&ost::mol::mm::Simulation::GetPositions,(arg("enforce_periodic_box")=false, arg("in_angstrom")=true))
    .def("GetVelocities",&ost::mol::mm::Simulation::GetVelocities)
    .def("GetForces",&ost::mol::mm::Simulation::GetForces)
    .def("SetPositions",&ost::mol::mm::Simulation::SetPositions,(arg("positions"),arg("in_angstrom")=true))
    .def("SetVelocities",&ost::mol::mm::Simulation::SetVelocities)
    .def("GetEntity",&ost::mol::mm::Simulation::GetEntity)
    .def("ApplySD",&ost::mol::mm::Simulation::ApplySD, (arg("tolerance")=100.0,arg("max_iterations")=1000))
    .def("ApplyLBFGS",&ost::mol::mm::Simulation::ApplyLBFGS,(arg("tolerance")=1.0,arg("max_iterations")=1000))
    .def("GetEnergy",&ost::mol::mm::Simulation::GetEnergy)                                                                             
    .def("GetPotentialEnergy",&ost::mol::mm::Simulation::GetPotentialEnergy)                                                                             
    .def("GetKineticEnergy",&ost::mol::mm::Simulation::GetKineticEnergy) 
    .def("Register",&ost::mol::mm::Simulation::Register) 
    .def("UpdatePositions",&ost::mol::mm::Simulation::UpdatePositions, (arg("enforce_periodic_box")=false))
    .def("GetTopology",&ost::mol::mm::Simulation::GetTopology)
    .def("ResetHarmonicBond",&ost::mol::mm::Simulation::ResetHarmonicBond) 
    .def("ResetHarmonicAngle",&ost::mol::mm::Simulation::ResetHarmonicAngle)
    .def("ResetUreyBradleyAngle",&ost::mol::mm::Simulation::ResetUreyBradleyAngle)
    .def("ResetPeriodicDihedral",&ost::mol::mm::Simulation::ResetPeriodicDihedral)
    .def("ResetPeriodicImproper",&ost::mol::mm::Simulation::ResetPeriodicImproper)
    .def("ResetHarmonicImproper",&ost::mol::mm::Simulation::ResetHarmonicImproper)
    .def("ResetLJPair",&ost::mol::mm::Simulation::ResetLJPair)
    .def("ResetDistanceConstraint",&ost::mol::mm::Simulation::ResetDistanceConstraint)  
    .def("ResetHarmonicPositionRestraint",&ost::mol::mm::Simulation::ResetHarmonicPositionRestraint,(arg("index"),arg("ref_position"),arg("k"),arg("x_scale")=1.0,arg("y_scale")=1.0,arg("z_scale")=1.0))                                                                     
    .def("ResetHarmonicDistanceRestraint",&ost::mol::mm::Simulation::ResetHarmonicDistanceRestraint)
    .def("ResetLJ",&ost::mol::mm::Simulation::ResetLJ)
    .def("ResetGBSA",&ost::mol::mm::Simulation::ResetGBSA)
    .def("ResetCharge",&ost::mol::mm::Simulation::ResetCharge)
    .def("ResetMass",&ost::mol::mm::Simulation::ResetMass)
    .def("AddPositionConstraint",&ost::mol::mm::Simulation::AddPositionConstraint)
    .def("AddPositionConstraints",&WrapAddPositionConstraints)
    .def("ResetPositionConstraints",&ost::mol::mm::Simulation::ResetPositionConstraints)
    .def("GetPeriodicBoxExtents",&ost::mol::mm::Simulation::GetPeriodicBoxExtents)
    .def("SetPeriodicBoxExtents",&ost::mol::mm::Simulation::SetPeriodicBoxExtents,(arg("extents")))
  ;

  boost::python::register_ptr_to_python<ost::mol::mm::SimulationPtr>();
}
