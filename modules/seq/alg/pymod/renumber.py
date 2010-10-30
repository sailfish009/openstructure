from ost import io, seq, mol, conop
import ost

def Renumber(seq_handle):
  """
  Function to renumber an entity according to an alignment between the model sequence 
  and the full-length target sequence. The aligned model sequence with an attached 
  view needs to be provided. Upon succcess, the renumbered entity is returned.

  .. code-block:: python

    from ost.seq.alg import renumber
    from ost.bindings.clustalw import *
    ent=io.LoadPDB("path_to_model")
    s=io.LoadSequence("path_to_full_length_fasta_seqeunce")
    pdb_seq=seq.SequenceFromChain("model", ent.chains[0])
    aln=ClustalW(s,pdb_seq)
    aln.AttachView(1,ent.chains[0].Select(""))
    e=Renumber(aln.GetSequence(1))
    io.SavePDB(e, "renum.pdb")
   
  """
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
    ost.LogMessage(err)
  conop.ConnectAll(ent_n)
  return ent_n


