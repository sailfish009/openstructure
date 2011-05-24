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
#include <boost/python/suite/indexing/vector_indexing_suite.hpp>

#include <ost/seq/alg/merge_pairwise_alignments.hh>
#include <ost/seq/alg/sequence_identity.hh>
#include <ost/seq/alg/ins_del.hh>
#include <ost/seq/alg/conservation.hh>
#include <ost/seq/alg/subst_weight_matrix.hh>
#include <ost/seq/alg/local_align.hh>
using namespace boost::python;
using namespace ost::seq;
using namespace ost::seq::alg;

BOOST_PYTHON_MODULE(_seq_alg)
{
  enum_<RefMode::Type>("RefMode")
    .value("ALIGNMENT", RefMode::ALIGNMENT)
    .value("LONGER_SEQUENCE", RefMode::LONGER_SEQUENCE)
    .export_values()
  ;
  def("SequenceIdentity", &SequenceIdentity, 
      (arg("ref_mode")=RefMode::ALIGNMENT, arg("seq_a")=0, arg("seq_b")=1));
  class_<AlignedRegionList>("AlignedRegionList", init<>())
    .def(vector_indexing_suite<AlignedRegionList>())
  ;
  class_<InsDel>("InsDel", init<const AlignmentHandle&, int, int>())
    .def(init<const AlignmentHandle&>())
    .def("GetDeletions", &InsDel::GetDeletions)
    .def("GetInsertions", &InsDel::GetInsertions)
  ;
  
  class_<SubstWeightMatrix, SubstWeightMatrixPtr>("SubstWeightMatrix", init<>())
    .def("GetWeight", &SubstWeightMatrix::GetWeight)
    .def("SetWeight", &SubstWeightMatrix::SetWeight)
  ;
  def("MergePairwiseAlignments", &MergePairwiseAlignments);
  def("Conservation", &Conservation, (arg("assign")=true, arg("prop_name")="cons"));
  def("LocalAlign", &LocalAlign, (arg("seq1"), arg("seq2"), arg("subst_weight"), 
      arg("gap_open")=-5, arg("gap_ext")=-2));

}
