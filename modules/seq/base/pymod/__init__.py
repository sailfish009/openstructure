#------------------------------------------------------------------------------
# This file is part of the OpenStructure project <www.openstructure.org>
#
# Copyright (C) 2008-2010 by the OpenStructure authors
#
# This library is free software; you can redistribute it and/or modify it under
# the terms of the GNU Lesser General Public License as published by the Free
# Software Foundation; either version 3.0 of the License, or (at your option)
# any later version.
# This library is distributed in the hope that it will be useful, but WITHOUT
# ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
# FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public License for more
# details.
#
# You should have received a copy of the GNU Lesser General Public License
# along with this library; if not, write to the Free Software Foundation, Inc.,
# 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
#------------------------------------------------------------------------------
from _seq import *
from ost import mol

## \brief Create two views from sequence alignment such that the matching 
#      residues occur at the same index within the chain.
#
# The first sequence in the sequence alignment belongs to ent_a, the second to
# ent_b.
#
# Requires that each of the entities/views only contains one chain and that
# the sequence offset of the sequences in the alignment is set appropriately.
def ViewsFromAlignment(multi_seq_ali, ent_a, ent_b, include_atoms=True):
  seq_a=multi_seq_ali.GetSequence(0)
  seq_b=multi_seq_ali.GetSequence(1)
  return ViewsFromSequences(seq_a, seq_b, ent_a, ent_b, include_atoms)

def ViewsFromSequences(seq_a, seq_b, ent_a=None, ent_b=None, 
                       include_atoms=True):
  ent_a=ent_a or seq_a.attached_view
  ent_b=ent_b or seq_b.attached_view
  if ent_a.chain_count>1:
   raise RuntimeError("first entity contains more than one chain")
  if ent_b.chain_count>1:
   raise RuntimeError("second entity contains more than one chain")
  is_ha=isinstance(ent_a, mol.EntityHandle)
  is_hb=isinstance(ent_b, mol.EntityHandle)  

  ev_a= is_ha and ent_a.CreateEmptyView() or ent_a.handle.CreateEmptyView()
  ev_b= is_hb and ent_b.CreateEmptyView() or ent_b.handle.CreateEmptyView()

  flags=include_atoms and mol.ViewAddFlag.INCLUDE_ATOMS or 0
  res_a=ent_a.residues
  res_b=ent_b.residues
  off_a=seq_a.GetSequenceOffset()
  off_b=seq_b.GetSequenceOffset()

  index_a, index_b=(0, 0)
  done=False
  #~ for i,j in zip(ent_a.residues,ent_b.residues): print i,i.index,j,j.index
  #~ print  
  while index_a<seq_a.GetLength() and index_b<seq_b.GetLength():
    while seq_a.GetOneLetterCode(index_a+off_b)=='-':
      index_a+=1
      print 'gap a'
      if seq_a.GetLength()<=index_a+off_b:
        done=True        
        break
    while seq_b.GetOneLetterCode(index_b+off_a)=='-':
      index_b+=1
      print 'gap b'
      if seq_b.GetLength()<=index_b+off_a:
        done=True
        break
    if done or len(res_a)<=index_b+off_a or len(res_b)<=index_a+off_b:
      break
    ra=res_a[off_a+index_b]
    rb=res_b[off_b+index_a]
    print 'index_a',index_a, seq_a.GetLength(),seq_a.GetOneLetterCode(index_a+off_a),ra
    print 'index_b',index_b, seq_b.GetLength(),seq_b.GetOneLetterCode(index_b+off_b),rb
    ev_a.AddResidue(is_ha and ra or ra.handle, flags)
    ev_b.AddResidue(is_hb and rb or rb.handle, flags)
    index_a+=1
    index_b+=1
  return ev_a, ev_b
