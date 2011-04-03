from _seq_alg import *
from ost.seq.alg.mat import *

def AlignToSEQRES(chain, seqres):
  """
  Aligns the residues of chain to the SEQRES sequence, inserting gaps where 
  needed. The function uses the connectivity of the protein backbone to find 
  consecutive peptide fragments. These fragments are then aligned to the SEQRES 
  sequence.
  
  All the non-ligand, peptide-linking residues of the chain must be listed in 
  SEQRES. If there are any additional residues in the chain, the function raises 
  a ValueError.
  
  :returns: The alignment of the residues in the chain and the SEQRES entries.
  :rtype: :class:`~ost.seq.AlignmentHandle`
  """
  from ost import seq
  from ost import mol
  view=chain.Select('ligand=false and peptide=true')
  residues=view.residues
  if len(residues)==0:
    return seq.CreateAlignment()
  fragments=[residues[0].one_letter_code]
  for r1, r2 in zip(residues[:-2], residues[1:]):
    if not mol.InSequence(r1.handle, r2.handle):
      fragments.append('')
    fragments[-1]+=r2.one_letter_code
  ss=str(seqres)
  pos=0
  aln_seq=''
  for frag in fragments:
    new_pos=ss.find(frag, pos)
    if new_pos==-1:
      raise ValueError('"%s" is not a substring of "%s"' % (frag, ss))
    aln_seq+='-'*(new_pos-pos)+frag
    pos=new_pos+len(frag)
  aln_seq+='-'*(len(seqres)-len(aln_seq))
  return seq.CreateAlignment(seq.CreateSequence('SEQRES', str(seqres)), 
                             seq.CreateSequence('atoms', aln_seq))


def AlignmentFromChainView(chain, handle_seq_name='handle', 
                           view_seq_name='view'):
  """
  Creates and returns the sequence alignment of the given chain view to the 
  chain handle. The alignment contains two sequences, the first containing all 
  non-ligand peptide-linking residues, the second containing all non-ligand 
  peptide-linking residues that are part of the view. 
  
  :param chain: A valid chain
  :type chain: :class:`~ost.mol.ChainView`
  
  :param handle_seq_name: Name of the handle sequence in the output alignment
  :param view_seq_name: Name of the view sequence in the output alignment
  :returns: The alignment
  :rtype: :class:`~ost.seq.AlignmentHandle`
  
  """
  from ost import seq
  v0=chain.handle.Select('ligand=false and peptide=true')
  v1=chain.Select('ligand=false and peptide=true')
  s0=seq.CreateSequence(handle_seq_name, '')
  s1=seq.CreateSequence(view_seq_name, '')
  s0.AttachView(v0)
  s1.AttachView(v1)
  res0=v0.residues
  res1=v1.residues
  idx0, idx1=(0, 0)
  while idx0<len(res0):
    s0.Append(res0[idx0].one_letter_code)
    if idx1<len(res1) and res1[idx1].handle==res0[idx0].handle:
      s1.Append(res1[idx1].one_letter_code)
      idx1+=1
    else:
      s1.Append('-')
    idx0+=1
  return seq.CreateAlignment(s0, s1)
