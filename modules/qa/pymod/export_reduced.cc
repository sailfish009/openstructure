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
#include <ost/mol/mol.hh>
#include <ost/qa/reduced_statistics.hh>
#include <ost/qa/reduced_potential.hh>

using namespace ost::qa;

using namespace boost::python;
  
uint64_t (ReducedStatistics::*get_count)(AminoAcid,AminoAcid,int,int)=&ReducedStatistics::GetCount;
void export_Reduced()
{
  class_<ReducedStatOptions>("ReducedStatOptions", init<>())
    .def(init<Real,Real,uint,uint,uint>((arg("l_cutoff"), arg("d_cutoff"), 
          arg("num_ang_bins"), arg("num_dist_bins"), arg("sequence_sep"))))
    .def_readwrite("lower_cutoff", &ReducedStatOptions::lower_cutoff)
    .def_readwrite("upper_cutoff", &ReducedStatOptions::upper_cutoff)
    .def_readwrite("num_angular_bins", &ReducedStatOptions::num_angular_bins)
    .def_readwrite("num_dist_bins", &ReducedStatOptions::num_dist_bins)
    .def_readwrite("sequence_sep", &ReducedStatOptions::sequence_sep)
  ;
  
  
  class_<ReducedStatistics, ReducedStatisticsPtr>("ReducedStatistics", no_init)
    .def(init<Real,Real,uint,uint,uint>((arg("l_cutoff"), arg("u_cutoff"), 
          arg("num_ang_bins"), arg("num_dist_bins"), arg("sequence_sep"))))
    .def("Extract", &ReducedStatistics::Extract)
    .add_property("options", make_function(&ReducedStatistics::GetOptions, 
                  return_value_policy<copy_const_reference>()))
    .def("Save", &ReducedStatistics::Save)
    .def("Load", &ReducedStatistics::Load).staticmethod("Load")
    .def("GetCount", get_count)
  ;
  class_<ReducedPotential, ReducedPotentialPtr>("ReducedPotential", no_init)
    .def("Load", &ReducedPotential::Load).staticmethod("Load")
    .def("Create", &ReducedPotential::Create).staticmethod("Create")
    .def("Save", &ReducedPotential::Save)
    .def("GetEnergy", &ReducedPotential::GetEnergy)
    .def("GetTotalEnergy", &ReducedPotential::GetTotalEnergy, 
         (arg("ent"), arg("normalize")=true))
    .add_property("options", make_function(&ReducedPotential::GetOptions, 
                  return_value_policy<copy_const_reference>()))
  ;
}