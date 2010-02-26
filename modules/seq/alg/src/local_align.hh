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
#ifndef OST_SEQ_LOCAL_ALIGN_HH
#define OST_SEQ_LOCAL_ALIGN_HH


#include <ost/seq/alignment_handle.hh>
#include <ost/seq/alg/module_config.hh>
#include <ost/seq/alg/alignment_opts.hh>
#include <ost/seq/alg/subst_weight_matrix.hh>
/*
  Author: Marco Biasini
 */

namespace ost { namespace seq { namespace alg {

/// \brief holds the result of a local sequence alignment
struct DLLEXPORT_OST_SEQ_ALG AlignedPatch {
  /// \brief offset of aligned patch relative to the start of the first sequence
  int offset_a;
  /// \brief offset of aligned patch relative to the start of the first sequence  
  int offset_b;
  /// \brief the aligned patch
  AlignmentHandle alignment;
  bool operator==(const AlignedPatch& rhs) const {
    return offset_a==rhs.offset_a && offset_b==rhs.offset_b &&
           rhs.alignment==alignment;
  }
  bool operator!=(const AlignedPatch& rhs) const {
    return !this->operator==(rhs);
  }
};

typedef std::vector<AlignedPatch> AlignedPatchList;

/// \brief  Align pair of sequences locally with Waterman-Smith algorithm
/// 
/// The alignment algorithm will return the set of optimal alignments given the
/// weights of the substitution matrix and gap opening and extension penalties.
/// The sequence patches are non-overlapping
/// 
/// \param seq_a is the first sequence to be aligned
/// \param seq_b is the second sequence to be aligned
/// 
/// \param opts holds a set of alignment options such as gap opening and 
///     extension penalties and the substitution matrix
/// 
/// \return list of locally aligned sequence patches.
AlignedPatchList DLLEXPORT_OST_SEQ_ALG 
LocalSequenceAlign(const SequenceHandle& seq_a, const SequenceHandle& seq_b, 
                   const AlignmentOpts& opts);
}}}

#endif
