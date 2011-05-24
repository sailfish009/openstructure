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
#include <ost/log.hh>
#include "local_align.hh"

namespace ost { namespace seq { namespace alg {

typedef enum {
  DIAG,
  INS1,
  INS2,
  UNKN
} Path;

struct AlnPos {
  AlnPos(): score(0), from(UNKN) { }
  int   score;
  Path  from;
};

struct AlnMat {
  AlnMat(int width, int height): 
    mat_(width*height), width_(width), height_(height)
  { }
  
  AlnPos& operator()(int x, int y) { return mat_[x*height_+y]; }
  
  const AlnPos& operator()(int x, int y) const { return mat_[x*height_+y]; } 
  
  int GetWidth() const { return width_; }
  int GetHeight() const { return height_; }
  
private:
  std::vector<AlnPos>  mat_;
  int               width_;
  int               height_;  
};

bool CollectPatches(AlnMat& mat, int min_i, int max_i, int min_j, int max_j,
                    const ConstSequenceHandle& s1, 
                    const ConstSequenceHandle& s2,
                    AlignmentList& alignments)
{
  int max_val=0, mmax_i=0, mmax_j=0;
  for (int i=min_i; i<max_i; ++i) {
    for (int j=min_j; j<max_j; ++j) {
      if (mat(i, j).score>=max_val) {
        mmax_i=i;
        mmax_j=j;
        max_val=mat(i, j).score;
      }
    }
  }
  int i=mmax_i; int j=mmax_j;
  String aln_str1;
  String aln_str2;
  while (i>min_i && j>min_j && mat(i, j).score>0) {
    switch (mat(i, j).from) {
      case DIAG:
        --i;
        --j;
        aln_str1.push_back(s1[i]);
        aln_str2.push_back(s2[j]);
        break;
      case INS1:
        --j;
        aln_str1.push_back('-');
        aln_str2.push_back(s2[j]);
        break;
      case INS2:
        --i;
        aln_str1.push_back(s1[i]);
        aln_str2.push_back('-');
        break;
       default:
         assert(0 && "should never get here");
    }
  }
  if (aln_str1.size()<2) {
    return false;
  }
  for (size_t x=0; x<aln_str1.size()/2; ++x) {
    std::swap(aln_str1[x], aln_str1[aln_str1.size()-x-1]);
    std::swap(aln_str2[x], aln_str2[aln_str1.size()-x-1]);    
  }
  AlignmentHandle aln=CreateAlignment();
  aln.AddSequence(CreateSequence(s1.GetName(), aln_str1));
  aln.AddSequence(CreateSequence(s2.GetName(), aln_str2));  
  aln.SetSequenceOffset(0, i);
  aln.SetSequenceOffset(1, j);
  alignments.push_back(aln);
  // clear used regions
  for (int x=i; x<=mmax_i; ++x) {
    for (int y=0; y<mat.GetHeight(); ++y) {
      mat(x, y).score=0;
    }
  }
  for (int x=0; x<mat.GetWidth(); ++x) {
    for (int y=j; y<mmax_j; ++y) {
      mat(x, y).score=0;
    }
  }  
  return true;
}

AlignmentList LocalAlign(const ConstSequenceHandle& s1, 
                         const ConstSequenceHandle& s2,
                         alg::SubstWeightMatrixPtr& subst,
                         int gap_open, int gap_ext)
{
  AlnMat mat(s1.GetLength()+1, s2.GetLength()+1);
  for (int i=0; i<mat.GetWidth()-1; ++i) {
    for (int j=0; j<mat.GetHeight()-1; ++j) {
      char c1=s1[i];
      char c2=s2[j];
      short weight=subst->GetWeight(c1, c2);
      int diag=weight+mat(i, j).score;
      int ins1=mat(i+1, j).score+(mat(i+1, j).from==INS1 ? gap_ext : gap_open);
      ins1=std::max(0, ins1);
      int ins2=mat(i, j+1).score+(mat(i, j+1).from==INS2 ? gap_ext : gap_open);
      ins2=std::max(0, ins2);
      if (diag>=ins1) {
        if (diag>=ins2) {
          mat(i+1, j+1).score=diag;
          mat(i+1, j+1).from=DIAG;
        } else {
          mat(i+1, j+1).score=ins2;
          mat(i+1, j+1).from=INS2;
        }
      } else if (ins1>ins2) {
        mat(i+1, j+1).score=ins1;
        mat(i+1, j+1).from=INS1;
      } else {
        mat(i+1, j+1).score=ins2;
        mat(i+1, j+1).from=INS2;
      }
    }
  }
#if !defined(NDEBUG)
  std::stringstream ss;
  for (int i=0; i<mat.GetWidth(); ++i) {
    for (int j=0; j<mat.GetHeight(); ++j) {
      if (mat(i, j).from==DIAG) {
        ss << "\\";
      } else if (mat(i, j).from==INS1) {
        ss << "-";
      } else if (mat(i, j).from==INS2) {
        ss << "|";
      }
      if (i==0 && j>0) {
        ss << s2[j-1];
      }
      if (j==0 && i>0) {
        ss << s1[i-1] << " ";
      }
      if (i==0 && j==0) {
        ss << "  ";
      }
    }
    ss << std::endl;
  }
  LOG_DEBUG(ss.str());
#endif
  AlignmentList alignments;
  while (CollectPatches(mat, 0, mat.GetWidth(), 0, mat.GetHeight(), 
         s1, s2, alignments)) {
    LOG_DEBUG(alignments.back().aln.ToString(80));
  }
  return alignments;
}

}}}