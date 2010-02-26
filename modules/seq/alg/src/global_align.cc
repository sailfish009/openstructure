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
/*
  Author: Marco Biasini
 */
#include <ost/seq/alg/global_align.hh>

namespace ost { namespace seq { namespace alg {

namespace {
  
class GlobalAlnMatrix {
public:
  GlobalAlnMatrix(int num_cols, int num_rows)
    : num_rows_(num_rows), num_cols_(num_cols) {    
      data_.resize(num_cols_*num_rows_,0);
  }
  short Get(int x, int y) const {
    assert(x>=0 && x<num_cols_); 
    assert(y>=0 && y<num_rows_);       
    return data_[num_cols_*y+x];
  }
  void Set(int x, int y, short value) {
    data_[num_cols_*y+x]=value;
  }

  int num_rows_;
  int num_cols_;
  std::vector<short>  data_;
};

}

AlignmentHandle GlobalSequenceAlign(const SequenceHandle& seq_a, 
                                    const SequenceHandle& seq_b,
                                    const AlignmentOpts& opts)
{
  // using the strings directly is much better for performance reasons
  String ssa=seq_a.GetString();
  String ssb=seq_b.GetString();
  GlobalAlnMatrix mat(ssa.size()+1, ssb.size()+1);
  
  // build-up aln mattrix
  for (size_t i=0; i<ssa.size()+1; ++i) {
    mat.Set(i, 0, i*opts.gap_open_penalty);
  }
  for (size_t i=0; i<ssb.size()+1; ++i) {
    mat.Set(0, i, i*opts.gap_open_penalty);
  }
  for (size_t i=1; i<ssa.size()+1; ++i) {
    for (size_t j=1; j<ssb.size()+1; ++j) {
      short mm=opts.subst_weights->GetWeight(ssa[i-1], ssb[j-1]);
      short m11=mat.Get(i-1, j-1)+mm;
      short m10=mat.Get(i-1, j)-opts.gap_open_penalty;
      short m01=mat.Get(i, j-1)-opts.gap_open_penalty;
      mat.Set(i, j, std::max(m11, std::max(m10, m01)));
    }
  }
  
  // backtracking
  size_t i=ssa.size();
  size_t j=ssb.size();
  String asa;
  String asb;
  asa.reserve(ssa.size());
  asb.reserve(ssb.size());

  while (i>0 && j>0) { 
    // find out where we came from
    short m11=mat.Get(i-1, j-1);
    short m10=mat.Get(i-1, j);
    short m01=mat.Get(i, j-1);
    short m00=mat.Get(i, j);
    short mm=opts.subst_weights->GetWeight(ssa[i-1], ssb[j-1]);    
    if (m11+mm==m00) { // match
      asa.append(1, ssa[i-1]);
      asb.append(1, ssb[j-1]);      
      --j;
      --i;
    } else if (m10-opts.gap_open_penalty==m00) { // seq b has a gap
      asa.append(1, ssa[i-1]);
      asb.append(1, '-');      
      --i;      
    } else if (m01-opts.gap_open_penalty==m00) { // seq a has a gap
      asa.append(1, '-');
      asb.append(1, ssb[j-1]);      
      --j;      
    }
  }
  while (i>0) {
    asa.append(1, ssa[i-1]);
    asb.append(1, '-');    
    --i;
  }
  while (j>0) {
    asa.append(1, '-');
    asb.append(1, ssb[j-1]);        
    --j;
  } 
  // reverse the two strings
  String rasa, rasb;
  rasa.reserve(asa.size());
  rasb.reserve(asb.size());  
  std::copy(asa.rbegin(), asa.rend(), std::back_inserter(rasa));
  std::copy(asb.rbegin(), asb.rend(), std::back_inserter(rasb));
  AlignmentHandle aln=CreateAlignment();
  aln.AddSequence(CreateSequence(seq_a.GetName(), rasa));
  aln.AddSequence(CreateSequence(seq_b.GetName(), rasb));  
  return aln;
}

}}}
