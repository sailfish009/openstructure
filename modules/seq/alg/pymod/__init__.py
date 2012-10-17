from _ost_seq_alg import *
from ost.seq.alg.mat import *

def ValidateSEQRESAlignment(aln, chain=None):
  """
  Checks a sequence aligned to a SEQRES sequence to be free of strand breaks.
  Residues divided by gaps are not considered as breakage but may also not be
  connected.

  :param aln: Alignment
  :type aln: :class:`~ost.seq.AlignmentHandle`
  :param chain: Source of the sequence
  :type chain: :class:`~ost.mol.ChainHandle`

  :returns: True if all residues (beside gaped ones) are connected, False
            otherwise.
  """
  from ost import LogWarning
  from ost import seq
  from ost import mol
  if aln.GetCount() != 2:
    raise ValueError('Alignment contains more than 2 sequences!')
  sequence = aln.GetSequence(1)
  if len(sequence) == 0:
    return True
  if chain == None:
    if sequence.HasAttachedView() == False:
      raise ValueError("Alignment is missing an attached chain view.")
    chain = sequence.GetAttachedView()
  residues = chain.residues
  # eat up all beginning gaps
  j = 1
  for s in sequence:
    if s != '-':
      break
    j += 1;
  l = sequence[j-1]
  i = 0
  # run over sequence & alignment
  for s in sequence[j:]:
    if s != '-':
      i += 1
      r1 = residues[i-1]
      r2 = residues[i]
      if r1.one_letter_code=='?' or r2.one_letter_code=='?':
        continue
      if l != '-':
        if not mol.InSequence(r1.handle, r2.handle):
          LogWarning('%s and %s are not connected by peptide bond' % (str(r1), str(r2)))
          return False
      else:
        if mol.InSequence(r1.handle, r2.handle):
          LogWarning('%s and %s are connected by peptide bond' % (str(r1), str(r2)))
          return False
    l = s
  return True

def AlignToSEQRES(chain, seqres, try_resnum_first=False, validate=True):
  """
  Aligns the residues of chain to the SEQRES sequence, inserting gaps where 
  needed. The function uses the connectivity of the protein backbone to find 
  consecutive peptide fragments. These fragments are then aligned to the SEQRES 
  sequence.
  
  All the non-ligand, peptide-linking residues of the chain must be listed in 
  SEQRES. If there are any additional residues in the chain, the function
  raises a ValueError.

  If 'try_resnum_first' is set, building the alignment following residue numbers
  is tried first.

  If 'validate' is set (default), the alignment is checked using
  :func:`~ost.seq.alg.ValidateSEQRESAlignment`.

  :param chain: Source of the sequence
  :type chain: :class:`~ost.mol.ChainHandle`
  :param seqres: SEQRES sequence
  :type seqres: :class:`str`
  :param try_resnum_first: Try to align by residue number
  :type try_resnum_first: :class:`bool`
  :param validate: Validate alignment by
                   :func:`~ost.seq.alg.ValidateSEQRESAlignment`
  :type validate: :class:`bool`

  :returns: The alignment of the residues in the chain and the SEQRES entries.
  :rtype: :class:`~ost.seq.AlignmentHandle`
  """

  def IsEqual(olc1, olc2):
    return olc1 in ('X', '?') or olc2 in ('X', '?') or olc1 == olc2

  from ost import seq
  from ost import mol
  from ost import LogWarning
  view=chain
  residues=view.residues
  if len(residues)==0:
    return seq.CreateAlignment()
  if try_resnum_first:
    aln_seq = seq.CreateSequence('atoms', '-'*len(seqres))
    for r1 in residues:
      if r1.number.num <= len(seqres) and r1.number.num > 0:
        if IsEqual(seqres[r1.number.num - 1], r1.one_letter_code):
          aln_seq[r1.number.num - 1] = r1.one_letter_code
        else:
          LogWarning('Sequence mismatch: chain has "' + r1.one_letter_code +
                     '", while SEQRES is "' + seqres[r1.number.num - 1] +
                     '" at the corresponding position.')
          try_resnum_first = False
          break
  if not try_resnum_first:
    fragments=[residues[0].one_letter_code]
    for r1, r2 in zip(residues[:-1], residues[1:]):
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
    aln_seq = seq.CreateSequence('atoms',
                                 aln_seq+('-'*(len(seqres)-len(aln_seq))))
  alignment = seq.CreateAlignment(seq.CreateSequence('SEQRES', str(seqres)), 
                                  aln_seq)
  if validate and not ValidateSEQRESAlignment(alignment, view):
    raise ValueError("SEQRES cannot be aligned with its corresponding chain.")
  return alignment


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
