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
#include <ost/mol/mm/mm_settings.hh>

using namespace boost::python;


void export_MMSettings()
{

  enum_<ost::mol::mm::Platform>("Platform")
    .value("Reference", ost::mol::mm::Reference)
    .value("OpenCL", ost::mol::mm::OpenCL)
    .value("CUDA", ost::mol::mm::CUDA)
    .value("CPU", ost::mol::mm::CPU)
  ;

  //export the termini exceptions object
  class_<ost::mol::mm::TerminiExceptions>("TerminiExceptions",init<>())
    .def("SetException", &ost::mol::mm::TerminiExceptions::SetException)
    .def("HasException", &ost::mol::mm::TerminiExceptions::HasException)
    .def("GetException", &ost::mol::mm::TerminiExceptions::GetException)
  ;


  //export the settings

  class_<ost::mol::mm::MMSettings>("MMSettings",init<>())
    .def_readwrite("add_bonds",&ost::mol::mm::MMSettings::add_bonds)
    .def_readwrite("add_angles",&ost::mol::mm::MMSettings::add_angles)
    .def_readwrite("add_dihedrals",&ost::mol::mm::MMSettings::add_dihedrals)
    .def_readwrite("add_impropers",&ost::mol::mm::MMSettings::add_impropers)
    .def_readwrite("add_cmaps",&ost::mol::mm::MMSettings::add_cmaps)
    .def_readwrite("add_exclusions",&ost::mol::mm::MMSettings::add_exclusions)
    .def_readwrite("add_nonbonded",&ost::mol::mm::MMSettings::add_nonbonded)
    .def_readwrite("add_gbsa",&ost::mol::mm::MMSettings::add_gbsa)
    .def_readwrite("constrain_hbonds",&ost::mol::mm::MMSettings::constrain_hbonds)
    .def_readwrite("constrain_hangles",&ost::mol::mm::MMSettings::constrain_hangles)
    .def_readwrite("constrain_bonds",&ost::mol::mm::MMSettings::constrain_bonds)
    .def_readwrite("rigid_water",&ost::mol::mm::MMSettings::rigid_water)
    .def_readwrite("strict_interactions",&ost::mol::mm::MMSettings::strict_interactions)
    .def_readwrite("ideal_bond_length_constraints",&ost::mol::mm::MMSettings::ideal_bond_length_constraints)
    .def_readwrite("fix_heavy_atoms",&ost::mol::mm::MMSettings::fix_heavy_atoms)
    .def_readwrite("kill_electrostatics",&ost::mol::mm::MMSettings::kill_electrostatics)
    .def_readwrite("generate_disulfid_bonds",&ost::mol::mm::MMSettings::generate_disulfid_bonds)
    .def_readwrite("nonbonded_method",&ost::mol::mm::MMSettings::nonbonded_method)
    .def_readwrite("nonbonded_cutoff",&ost::mol::mm::MMSettings::nonbonded_cutoff)
    .def_readwrite("remove_cmm_motion",&ost::mol::mm::MMSettings::remove_cmm_motion)
    .def_readwrite("periodic_box_extents",&ost::mol::mm::MMSettings::periodic_box_extents)
    .def_readwrite("init_temperature",&ost::mol::mm::MMSettings::init_temperature)
    .def_readwrite("forcefield",&ost::mol::mm::MMSettings::forcefield)
    .def_readwrite("termini_exceptions",&ost::mol::mm::MMSettings::termini_exceptions)
    .def_readwrite("platform",&ost::mol::mm::MMSettings::platform)
    .def_readwrite("add_thermostat",&ost::mol::mm::MMSettings::add_thermostat)
    .def_readwrite("thermostat_temperature",&ost::mol::mm::MMSettings::thermostat_temperature)
    .def_readwrite("thermostat_collision_frequency",&ost::mol::mm::MMSettings::thermostat_collision_frequency)
    .def_readwrite("add_barostat",&ost::mol::mm::MMSettings::add_barostat)
    .def_readwrite("barostat_temperature",&ost::mol::mm::MMSettings::barostat_temperature)
    .def_readwrite("barostat_pressure",&ost::mol::mm::MMSettings::barostat_pressure)
    .def_readwrite("barostat_frequency",&ost::mol::mm::MMSettings::barostat_frequency)
    .def_readwrite("integrator",&ost::mol::mm::MMSettings::integrator)
    .def_readwrite("solvent_dielectric",&ost::mol::mm::MMSettings::solvent_dielectric)
    .def_readwrite("solute_dielectric",&ost::mol::mm::MMSettings::solute_dielectric)
    .def_readwrite("reaction_field_dielectric",&ost::mol::mm::MMSettings::reaction_field_dielectric)
    .def_readwrite("use_dispersion_correction",&ost::mol::mm::MMSettings::use_dispersion_correction)
    .def_readwrite("openmm_plugin_directory",&ost::mol::mm::MMSettings::openmm_plugin_directory)
    .def_readwrite("custom_plugin_directory",&ost::mol::mm::MMSettings::custom_plugin_directory)

  ;

  boost::python::register_ptr_to_python<ost::mol::mm::MMSettingsPtr>();
  boost::python::register_ptr_to_python<ost::mol::mm::TerminiExceptionsPtr>();

}

