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
#include <boost/python/register_ptr_to_python.hpp>
#include <ost/qa/packing_statistics.hh>
#include <ost/qa/packing_potential.hh>
using namespace ost::qa;  
using namespace ost;

namespace {
  typedef uint64_t (PackingStatistics::*F1)(AminoAcid) const;
  typedef uint64_t (PackingStatistics::*F2)(AminoAcid, int) const;  
  F1 f1=&PackingStatistics::GetCount;
  F2 f2=&PackingStatistics::GetCount;  
}

void export_Packing()
{
  class_<PackingStatistics>("PackingStatistics", 
                            init<Real,int,int>())
    .def(init<>())
    .def("Extract", &PackingStatistics::Extract, arg("model"))
    .def("GetCount", f1,
         arg("amino_acid"))
     .def("GetCount", f2,
          arg("amino_acid"), arg("count"))    
     .def("LoadFromFile", &PackingStatistics::LoadFromFile).staticmethod("LoadFromFile")
     .def("SaveToFile", &PackingStatistics::SaveToFile)
  ;
  register_ptr_to_python<PackingStatisticsPtr>();
  
  class_<PackingPotentialOpts>("PackingPotentialOpts", init<>())
    .def_readwrite("MaxCounts", &PackingPotentialOpts::max_counts)
    .def_readwrite("Sigma", &PackingPotentialOpts::sigma)
    .def_readwrite("Cutoff", &PackingPotentialOpts::cutoff)
    .def_readwrite("BucketSize", &PackingPotentialOpts::bucket_size)
  ;
  class_<PackingPotential>("PackingPotential", no_init)
    .def("Create", &PackingPotential::Create).staticmethod("Create")
    .def("LoadFromFile", &PackingPotential::LoadFromFile).staticmethod("LoadFromFile")
    .def("GetTotalEnergy", &PackingPotential::GetTotalEnergy)
    .def("GetEnergyCounts", &PackingPotential::GetEnergyCounts)
    .def("SaveToFile", &PackingPotential::SaveToFile)
  ;
  register_ptr_to_python<PackingPotentialPtr>();
}
