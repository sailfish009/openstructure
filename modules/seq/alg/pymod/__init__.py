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
  residues=chain.residues
  if len(residues)==0:
    return None
  fragments=[residues[0].one_letter_code]
  for r1, r2 in zip(residues[:-2], residues[1:]):
    if not r2.IsPeptideLinking() or r2.IsLigand():
      continue
    if not mol.InSequence(r1, r2):
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

    
  