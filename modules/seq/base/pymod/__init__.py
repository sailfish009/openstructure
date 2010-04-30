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

def ViewsFromAlignment(multi_seq_ali, ent_a, ent_b, include_atoms=True):
  seq_a=multi_seq_ali.GetSequence(0)
  seq_b=multi_seq_ali.GetSequence(1)

  # set offset according to ent_a and ent_b


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
  # get all residues
  res_a=ent_a.residues
  res_b=ent_b.residues

  # map sequence onto
  s1 = seq_a.gapless_string
  s2 = seq_b.gapless_string

  # get sequence of ent1
  seq_ent_a = ""
  seq_ent_b= ""

  sub_a = ent_a.Select("peptide = 1")
  sub_b = ent_b.Select("peptide = 1")
  for res1 in sub_a.residues:
      seq_ent_a = "%s%s" % (seq_ent_a, res1.one_letter_code)

  seq_ent_b = ""
  for res2 in sub_b.residues:
      seq_ent_b = "%s%s" % (seq_ent_b, res2.one_letter_code)

  #~ print seq_ent_a
  #~ print s1

  print seq_ent_a
  print s1
  off_a = seq_ent_a.index(s1)
  off_b = seq_ent_b.index(s2)

  #off_a=seq_a.GetSequenceOffset()
  #off_b=seq_b.GetSequenceOffset()


  index_a, index_b=(0, 0)
  done=False

  align_index = 0

  # iterate over residues
  while index_a<seq_a.GetLength() and index_b<seq_b.GetLength():
      # print 'index',index_a, seq_a.GetLength(), index_b, seq_b.GetLength()
   while seq_a.GetOneLetterCode(index_a)=='-':
     index_a+=1
     if seq_a.GetLength()<index_a:
       done=True
       break
   while seq_b.GetOneLetterCode(index_b)=='-':
     index_b+=1
     if seq_b.GetLength()<index_b:
       done=True
       break
   if done or len(res_a)<=off_a+index_b or len(res_b)<=off_b+index_a:
     break
   ra=res_a[off_a+index_b]
   rb=res_b[off_b+index_a]
   #ra = res_a[index_b]
   #rb = res_a[index_a]
   #~ print "set align_index %d for %s %s" % (align_index, ra, rb)
   #ra.SetIntProp("align_index",align_index)
   #rb.SetIntProp("align_index",align_index)
   align_index += 1
   ev_a.AddResidue(is_ha and ra or ra.handle, flags)
   ev_b.AddResidue(is_hb and rb or rb.handle, flags)
   index_a+=1
   index_b+=1
   eda=ev_a.handle.RequestXCSEditor()
   eda.RenameChain(ent_a.chains[0].handle,ent_a.chains[0].name)
   edb=ev_b.handle.RequestXCSEditor()
   edb.RenameChain(ent_b.chains[0].handle, ent_b.chains[0].name)
  return ev_a, ev_b
