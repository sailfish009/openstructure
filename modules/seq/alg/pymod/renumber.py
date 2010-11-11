from ost import io, seq, mol, conop
from ost import *

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
    if seq_handle.HasAttachedView()==False:
      raise RuntimeError, "Sequence Handle has no attached view"
    changed_residue_count=0
    renumberingFlag = False
    ent_n=mol.CreateEntity()
    ed=ent_n.EditXCS()
    c=ed.InsertChain(" ")
    for pos in range(len(seq_handle)):
      if seq_handle[pos]!='-':
        r=seq_handle.GetResidue(pos)
        if r.IsValid():
            #print seq_handle[pos],r.number.num,pos+1
            if r.number.num!=pos+1:
              changed_residue_count+=1
              renumberingFlag = True
            r_n=ed.AppendResidue(c,r.name, mol.ResNum(pos+1))
            for atom in r.atoms:
              ed.InsertAtom(r_n,atom.name,atom.pos,atom.prop)
        else:
          err='Error: renumbering failed at position %s' %pos
          raise RuntimeError, err
    if renumberingFlag == True:
      err = 'Warning: %s residues have been renumbered!' %changed_residue_count
      LogInfo(err)
    conop.ConnectAll(ent_n)
    return ent_n

  elif isinstance(seq_handle, seq.AlignmentHandle):
    if seq_handle.GetSequence(sequence_number_with_attached_view).HasAttachedView()==False:
      raise RuntimeError, "Sequence Handle has no attached view"
    dir(seq_handle)
    counter=0
    changed_residue_count=0
    renumberingFlag = False
    ent_n=mol.CreateEntity()
    ed=ent_n.EditXCS()
    c=ed.InsertChain(seq_handle.GetSequence(sequence_number_with_attached_view).GetAttachedView().chains[0].name)
    for col in seq_handle:
      if col[0]!='-' and col[1]!='-':
        if col[0]==col[1]:
          rnum=seq_handle.GetSequence(sequence_number_with_attached_view).GetResidueIndex(counter)
          r=seq_handle.GetSequence(sequence_number_with_attached_view).GetResidue(counter)
          if r.IsValid():
            if r.number.num!=counter+1:
              changed_residue_count+=1
              renumberingFlag = True
            r_n=ed.AppendResidue(c,r.name, mol.ResNum(counter+1))
            for atom in r.atoms:
              ed.InsertAtom(r_n,atom.name,atom.pos,atom.element, atom.b_factor, 
                            atom.occupancy, atom.is_hetatom)

          else:
            raise RuntimeError("invalide residue at postion %s (renumbering failed)" %(counter))
        else:
          raise RuntimeError("residue mismatch at position %d (%s vs %s) (renumbering failed)"%(counter, col[0],col[1]))
      counter+=1
    if renumberingFlag == True:
      err = 'Warning: %s residues have been renumbered!' %changed_residue_count
      LogInfo(err)
    conop.ConnectAll(ent_n)
    return ent_n

