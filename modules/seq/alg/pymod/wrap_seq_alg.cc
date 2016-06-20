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
#include <ost/seq/alg/sequence_similarity.hh>
#include <ost/seq/alg/ins_del.hh>
#include <ost/seq/alg/conservation.hh>
#include <ost/seq/alg/subst_weight_matrix.hh>
#include <ost/seq/alg/local_align.hh>
#include <ost/seq/alg/global_align.hh>
#include <ost/seq/alg/semiglobal_align.hh>
#include <ost/seq/alg/entropy.hh>
#include <ost/seq/alg/pair_subst_weight_matrix.hh>
#include <ost/seq/alg/contact_weight_matrix.hh>
#include <ost/seq/alg/contact_prediction_score.hh>

using namespace boost::python;
using namespace ost::seq;
using namespace ost::seq::alg;

BOOST_PYTHON_MODULE(_ost_seq_alg)
{
  enum_<RefMode::Type>("RefMode")
    .value("ALIGNMENT", RefMode::ALIGNMENT)
    .value("LONGER_SEQUENCE", RefMode::LONGER_SEQUENCE)
    .export_values()
  ;

  def("SequenceIdentity", &SequenceIdentity, 
      (arg("ref_mode")=RefMode::ALIGNMENT, arg("seq_a")=0, arg("seq_b")=1));

  def("SequenceSimilarity", &SequenceSimilarity,
      (arg("aln"),arg("subst_weight"),arg("normalize")=false,
       arg("seq_a")=0,arg("seq_b")=1));

  class_<AlignedRegionList>("AlignedRegionList", init<>())
    .def(vector_indexing_suite<AlignedRegionList>())
  ;

  class_<InsDel>("InsDel", init<const AlignmentHandle&, int, int>())
    .def(init<const AlignmentHandle&>())
    .def("GetDeletions", &InsDel::GetDeletions)
    .def("GetInsertions", &InsDel::GetInsertions)
  ;
  
  def("MergePairwiseAlignments", &MergePairwiseAlignments);
  def("Conservation", &Conservation, (arg("assign")=true, arg("prop_name")="cons", arg("ignore_gap")=false));
  def("LocalAlign", &LocalAlign, (arg("seq1"), arg("seq2"),arg("subst_weight"), 
      arg("gap_open")=-5, arg("gap_ext")=-2));
  def("GlobalAlign", &GlobalAlign,(arg("seq1"),arg("seq2"),arg("subst_weight"), 
      arg("gap_open")=-5, arg("gap_ext")=-2));
  def("SemiGlobalAlign", &SemiGlobalAlign,(arg("seq1"),arg("seq2"),arg("subst_weight"), 
      arg("gap_open")=-5, arg("gap_ext")=-2));
  def("ShannonEntropy", &ShannonEntropy, (arg("aln"), arg("ignore_gaps")=true));

  class_<PairSubstWeightMatrix>("PairSubstWeightMatrix",init< std::vector <std::vector <std::vector <std::vector <Real> > > >,std::vector <char> >())
    .def(init<>())
    .def_readonly("weights",&PairSubstWeightMatrix::weights)
    .def_readonly("aa_list",&PairSubstWeightMatrix::aa_list)
  ;

  class_<ContactWeightMatrix>("ContactWeightMatrix",init< std::vector <std::vector <Real> >,std::vector <char> >())
    .def(init<>())
    .def_readonly("weights",&ContactWeightMatrix::weights)
    .def_readonly("aa_list",&ContactWeightMatrix::aa_list)
  ;

  class_<ContactPredictionScoreResult>("ContactPredictionScoreResult",no_init)
    .def_readonly("matrix",&ContactPredictionScoreResult::matrix)
    .def_readonly("sorted_indices",&ContactPredictionScoreResult::sorted_indices)
    .def("RefreshSortedIndices",&ContactPredictionScoreResult::RefreshSortedIndices)
    .def("GetScore",&ContactPredictionScoreResult::GetScore,(arg("i"),arg("j")))
    .def("SetScore",&ContactPredictionScoreResult::SetScore,(arg("i"),arg("j"),arg("score")))
  ;
  def("CalculateMutualInformation", &CalculateMutualInformation,(arg("aln"),arg("w")=LoadConstantContactWeightMatrix(),
                                                        arg("apc_correction")=true,arg("zpx_transformation")=true,arg("small_number_correction")=0.05));
  def("CalculateContactScore", &CalculateContactScore,(arg("aln"), arg("w")=LoadDefaultContactWeightMatrix()));
  def("CalculateContactSubstitutionScore", &CalculateContactSubstitutionScore,(arg("aln"), arg("ref_seq_index")=0, arg("w")=LoadDefaultPairSubstWeightMatrix()));
  def("LoadDefaultContactWeightMatrix",LoadDefaultContactWeightMatrix);
  def("LoadConstantContactWeightMatrix",LoadConstantContactWeightMatrix);
  def("LoadDefaultPairSubstWeightMatrix",LoadDefaultPairSubstWeightMatrix);

  scope mat_scope = class_<SubstWeightMatrix, SubstWeightMatrixPtr>("SubstWeightMatrix", init<>())
                      .def("GetWeight", &SubstWeightMatrix::GetWeight)
                      .def("SetWeight", &SubstWeightMatrix::SetWeight)
                      .def("GetMinWeight", &SubstWeightMatrix::GetMinWeight)
                      .def("GetMaxWeight", &SubstWeightMatrix::GetMaxWeight)
                      .def("AssignPreset", &SubstWeightMatrix::AssignPreset)
  ;

  enum_<SubstWeightMatrix::Preset>("Preset")
    .value("BLOSUM45", SubstWeightMatrix::BLOSUM45)
    .value("BLOSUM62", SubstWeightMatrix::BLOSUM62)
    .value("BLOSUM80", SubstWeightMatrix::BLOSUM80)
    .value("BLOSUM100", SubstWeightMatrix::BLOSUM100)
  ;

}
