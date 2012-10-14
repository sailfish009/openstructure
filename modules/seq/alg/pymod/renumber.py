from ost import io, seq, mol, conop
from ost import *


def _RenumberSeq(seq_handle):
  if not seq_handle.HasAttachedView():
    raise RuntimeError("Sequence Handle has no attached view")
  ev = seq_handle.attached_view.CreateEmptyView()
  new_numbers = mol.ResNumList()
  for pos in range(len(seq_handle)):
    if seq_handle[pos]!='-':
      r=seq_handle.GetResidue(pos)
      if r.IsValid():
          #print seq_handle[pos],r.number.num,pos+1
          ev.AddResidue(r, mol.INCLUDE_ALL)
          new_numbers.append(pos+1)
      else:
        raise RuntimeError('Error: renumbering failed at position %s' %pos)
  return ev, new_numbers

def _RenumberAln(aln, seq_index):
  if not aln.sequences[seq_index].HasAttachedView():
    raise RuntimeError("Sequence Handle has no attached view")
  counter=0
  ev = aln.sequences[seq_index].attached_view.CreateEmptyView()
  new_numbers = mol.ResNumList()
  for col in aln:
    if col[0]!='-' and col[seq_index]!='-':
      if col[0]!=col[seq_index]:
        raise RuntimeError("residue mismatch at position %d (%s vs %s) (renumbering failed)"%(counter, col[0],col[1]))
      rnum=aln.GetSequence(seq_index).GetResidueIndex(counter)
      r=aln.GetSequence(seq_index).GetResidue(counter)
      if not r.IsValid():
        raise RuntimeError("invalid residue at postion %s (renumbering failed)" %(counter))
      ev.AddResidue(r, mol.INCLUDE_ALL)
      new_numbers.append(counter+1)
    counter+=1
  return ev, new_numbers


def Renumber(seq_handle, sequence_number_with_attached_view=1):
  """
  Function to renumber an entity according to an alignment between the model sequence 
  and the full-length target sequence. The aligned model sequence or the alignment itself
  with an attached view needs to be provided. Upon succcess, the renumbered entity is returned.

  .. code-block:: python

    from ost.seq.alg import renumber
    from ost.bindings.clustalw import *
    ent=io.LoadPDB("path_to_model")
    s=io.LoadSequence("path_to_full_length_fasta_seqeunce")
    pdb_seq=seq.SequenceFromChain("model", ent.chains[0])
    aln=ClustalW(s,pdb_seq)
    aln.AttachView(1,ent.chains[0].Select(""))
    e=Renumber(aln.GetSequence(sequence_number_with_attached_view))
    io.SavePDB(e, "renum.pdb")
   
  """
  if isinstance(seq_handle, seq.SequenceHandle):
    ev, new_numbers = _RenumberSeq(seq_handle)
  elif isinstance(seq_handle, seq.AlignmentHandle):
    ev, new_numbers = _RenumberAln(seq_handle, sequence_number_with_attached_view)

  ev.AddAllInclusiveBonds()
  new_ent = mol.CreateEntityFromView(ev,False);
  new_ent.EditXCS().RenumberChain(new_ent.chains[0], new_numbers)
  return new_ent
