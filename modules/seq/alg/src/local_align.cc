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
#include "local_align.hh"

namespace ost { namespace seq { namespace alg {

namespace {

class AliMatrix {
public:
  AliMatrix(int num_rows, int num_cols)
    : num_rows_(num_rows), num_cols_(num_cols) {    
      data_.resize(num_cols_*num_rows_,0);
  }
  short Get(int x, int y) const {
    assert(x>=0 && x<num_rows_);
    assert(y>=0 && y<num_cols_);    
    return data_[num_cols_*x+y];
  }
  void Set(int x, int y, short value) {
    data_[num_cols_*x+y]=value;
  }

  int num_rows_;
  int num_cols_;
  std::vector<short>  data_;
};

std::pair<int, int> FindMax(const AliMatrix& mat, int min_col,
                           int max_col, int min_row, int max_row) {
  int max_i=0, max_j=0, max_value=-1;
  for (int i=min_col; i<max_col; ++i) {
    for (int j=min_row; j<max_row; ++j) {
      int x=mat.Get(i, j);
      if (x>max_value) {
        max_value=x;
        max_i=i; max_j=j;
      }
    }
  }
  return std::make_pair(max_i, max_j);
}

// divide and conquer the alignment matrix to get all patches.
void CollectPatches(AlignedPatchList& patches, const String& seq_a, 
                    const String& seq_b, const AliMatrix& ali_mat, int min_col,
                    int max_col, int min_row, int max_row) {
  if (max_col-min_col<2 || max_row-min_row<2)
    return;
  String seq_c, seq_d;
  seq_c.reserve(std::max(seq_a.length(), seq_b.length()));
  seq_d.reserve(std::max(seq_a.length(), seq_b.length()));
  std::pair<int, int> best=FindMax(ali_mat, min_col, max_col, min_row, max_row);  
  std::pair<int, int> max_pair=best;

  typedef std::pair<int, int> XY;
  std::vector<XY> stack;
  while(ali_mat.Get(best.first, best.second)>0 && best.first>=min_col && 
          best.second>=min_row) {
    stack.push_back(XY(best.first, best.second));
    int a=ali_mat.Get(best.first-1, best.second-1);
    int b=ali_mat.Get(best.first-1, best.second);
    int c=ali_mat.Get(best.first, best.second-1);    
    if (a>=c) {
      if (a>=b) {
        best.first-=1;
        best.second-=1;
      } else {
        best.first-=1;
      }
    } else {
      if (c>b) {
        best.second-=1;
      } else {
        best.first-=1;
      }
    }
  }
  for (int i=static_cast<int>(stack.size())-2; i>=0; --i) {
    XY next=stack[i];
    XY curr=stack[i+1];
    if (next.first==curr.first+1) {
      if (next.second==curr.second+1) {
        seq_c.append(1, seq_a[next.first-1]);
        seq_d.append(1, seq_b[next.second-1]);
      } else {
        seq_c.append(1, seq_a[next.first-1]);
        seq_d.append(1, '-');        
      }
    } else {
      if (next.second==curr.second+1) {
        seq_c.append(1, '-');
        seq_d.append(1, seq_b[next.second-1]);
      } else {
        seq_c.append(1, seq_a[next.first-1]);
        seq_d.append(1, '-');
      }
    }
  }
  if (seq_c.size()>2 && seq_c.size()>2) {
    patches.push_back(AlignedPatch());  
    patches.back().offset_a=best.first;
    patches.back().offset_b=best.second;            
    if (seq_c[0]=='-' || seq_d[0]=='-') {
      seq_c.erase(seq_c.begin());
      seq_d.erase(seq_d.begin());    
    }
    patches.back().alignment=CreateAlignment();
    patches.back().alignment.AddSequence(CreateSequence("seq1", seq_c));
    patches.back().alignment.AddSequence(CreateSequence("seq2  ", seq_d));  
    CollectPatches(patches, seq_a, seq_b, ali_mat, max_pair.first+1, max_col,
                   max_pair.second+1, max_row);    
  }
}

}

AlignedPatchList LocalSequenceAlign(const SequenceHandle& sseq_a, 
                                    const SequenceHandle& sseq_b, 
                                    const AlignmentOpts& opts)
{
  
  String seq_a=sseq_a.GetString();
  String seq_b=sseq_b.GetString();  
  AliMatrix ali_mat(seq_a.length()+1, seq_b.length()+1);
  // build alignment matrix.
  for (size_t i=0; i<seq_a.length(); ++i) {
    for (size_t j=0; j<seq_b.length(); ++j) {
      int w=opts.subst_weights->GetWeight(seq_a[i], seq_b[j]);
      int match_mism=std::max(0, w+int(ali_mat.Get(i, j)));
      int del=ali_mat.Get(i, j+1)-opts.gap_open_penalty;
      int ins=ali_mat.Get(i+1, j)-opts.gap_open_penalty;
      ali_mat.Set(i+1, j+1, std::max(match_mism, std::max(del, ins)));
    }
  }
  AlignedPatchList patches;
  // backtracking  
  CollectPatches(patches, seq_a, seq_b, ali_mat, 0, seq_a.length()+1, 
                 0, seq_b.length()+1);
  return patches;  
}

}}}

