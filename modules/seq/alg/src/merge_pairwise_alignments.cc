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


#include <sstream>
#include <map>

#include <ost/integrity_error.hh>
#include "merge_pairwise_alignments.hh"

/*
  Author: Marco Biasini
 */
namespace ost { namespace seq { namespace alg {

typedef std::map<int, int> ShiftMap;

namespace {

void update_shifts(const AlignmentHandle& aln, 
                   const SequenceHandle& ref_seq,
                   ShiftMap& shifts)
{
  ConstSequenceHandle s1=aln.GetSequence(0);  
  if (s1.GetGaplessString()!=ref_seq.GetString()) {
    throw IntegrityError("The gapless version of '"+s1.GetString()+
                         "' is not identical to the reference sequence.");
  }
  for (int i=0; i<s1.GetLength(); ++i) {
    int shift=0;
    int j=i;
    while (j<s1.GetLength() && s1.GetOneLetterCode(j)=='-') {
      shift++;
      j++;
    }
    if (shift>0) {
      assert(i>0);
      int res_index=s1.GetResidueIndex(i-1);
      ShiftMap::iterator p=shifts.find(res_index);
      if (p!=shifts.end()) {
        p->second=std::max(p->second, shift);
      } else {
        shifts.insert(std::make_pair(res_index, shift));
      }
    }
    i=j;
  }  
}

SequenceHandle shift_reference(const SequenceHandle& ref_seq, 
                                    const ShiftMap& shifts)
{
  std::ostringstream new_sequence;
  String ref_str=ref_seq.GetString();
  int last=0;
  for (ShiftMap::const_iterator i=shifts.begin(),e=shifts.end();i!=e; ++i) {
    new_sequence << ref_str.substr(last, i->first-last+1) 
                 << String(i->second, '-');
    last=i->first+1;
  }
  new_sequence << ref_str.substr(last);
  SequenceHandle s=CreateSequence(ref_seq.GetName(), 
                                            new_sequence.str());                                               
  if (ref_seq.HasAttachedView())
    s.AttachView(ref_seq.GetAttachedView());
  s.SetSequenceOffset(s.GetSequenceOffset());
  return s;
}

SequenceHandle realign_sequence(const AlignmentHandle& aln, 
                                  const ShiftMap& shifts)
{
  std::ostringstream new_sequence;
  ConstSequenceHandle s1=aln.GetSequence(0);  
  ConstSequenceHandle s2=aln.GetSequence(1);
  for (int i=0; i<s1.GetLength(); ++i) {
    int shift=0;
    int j=i;
    while (j<s1.GetLength() && s1.GetOneLetterCode(j)=='-') {
      shift++;
      ++j;
    }
    if (i>0 && s1.GetOneLetterCode(i-1)!='-') {
      ShiftMap::const_iterator p=shifts.find(s1.GetResidueIndex(i-1));
      if (p!=shifts.end()) {
        int d=p->second-shift;
        assert(d>=0);
        new_sequence << String(d, '-');
      }      
    }
    new_sequence << s2.GetOneLetterCode(i);    
  }
  SequenceHandle s=CreateSequence(s2.GetName(), new_sequence.str());
  if (s2.HasAttachedView())
    s.AttachView(s2.GetAttachedView());
  s.SetSequenceOffset(s2.GetSequenceOffset());
  return s;
}  

}

AlignmentHandle MergePairwiseAlignments(const AlignmentList& pairwise_alns,
                                        const SequenceHandle& ref_seq)
{

  ShiftMap shifts;
  for (AlignmentList::const_iterator i=pairwise_alns.begin(),
       e=pairwise_alns.end(); i!=e; ++i) {
    update_shifts(*i, ref_seq, shifts);
  }

  AlignmentHandle merged=CreateAlignment();
  merged.AddSequence(shift_reference(ref_seq, shifts));
  for (AlignmentList::const_iterator i=pairwise_alns.begin(),
       e=pairwise_alns.end(); i!=e; ++i) {
    SequenceHandle new_seq=realign_sequence(*i, shifts);
    merged.AddSequence(new_seq);
  }  
  return merged;
}


}}}
