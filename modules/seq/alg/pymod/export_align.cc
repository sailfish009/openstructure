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
#include <boost/python/register_ptr_to_python.hpp>
#include <boost/python/suite/indexing/vector_indexing_suite.hpp>
using namespace boost::python;

#include <ost/info/info.hh>

#include <ost/mol/mol.hh>
#include <ost/seq/alg/subst_weight_matrix.hh>
#include <ost/seq/alg/local_align.hh>
#include <ost/seq/alg/global_align.hh>
using namespace ost;
using namespace ost::seq;
using namespace ost::seq::alg;

void export_Align()
{
  class_<SubstWeightMatrix, SubstWeightMatrixPtr>("SubstWeightMatrix", init<>())
    .def("GetWeight", &SubstWeightMatrix::GetWeight)
    .def("SetWeight", &SubstWeightMatrix::SetWeight)
  ;
class_<AlignmentOpts>("AlignmentOpts", init<int,int, SubstWeightMatrixPtr>())
  .def_readwrite("gap_open_penalty", &AlignmentOpts::gap_open_penalty)
  .def_readwrite("gap_extension_penalty", 
                 &AlignmentOpts::gap_extension_penalty)    
  .def_readwrite("subst_weights", &AlignmentOpts::subst_weights)
;  
  def("SubstWeightMatrixToInfo", &SubstWeightMatrixToInfo);
  def("SubstWeightMatrixFromInfo", &SubstWeightMatrixFromInfo);
  
  class_<AlignedPatch>("AlignedPatch", no_init)
    .def_readwrite("alignment", &AlignedPatch::alignment)
    .def_readwrite("offset_a", &AlignedPatch::offset_a)
    .def_readwrite("offset_b", &AlignedPatch::offset_b)
  ;    
  class_<AlignedPatchList>("AlignedPatchList", init<>())
    .def(vector_indexing_suite<AlignedPatchList>())
  ;
  def("LocalSequenceAlign", &LocalSequenceAlign);
  def("GlobalSequenceAlign", &GlobalSequenceAlign);
}
