"""
Superposition of structures made simple.

Authors: Stefan Bienert
"""

import math
import ost.mol.alg
import ost.seq.alg

def ParseAtomNames(atoms):
  """
  Parses different representations of a list of atom names and returns a
  :class:`set`, understandable by :func:`~ost.mol.alg.MatchResidueByNum`. In
  essence, this function translates

  * None to ``None``

  * 'all' to ``None``

  * 'backbone' to ``set(['N', 'CA', 'C', 'O'])``

  * 'aname1, aname2' to ``set(['aname1', 'aname2'])``

  * ``['aname1', 'aname2']``  to ``set(['aname1', 'aname2'])``

  :param atoms: Identifier or list of atoms
  :type atoms: :class:`str`, :class:`list`, :class:`set`
  :returns: A :class:`set` of atoms.
  """
  ## get a set of atoms or None
  if atoms==None:
    return None
  if isinstance(atoms, str):
    if atoms.upper()=='ALL':
      return None
    if atoms.upper()=='BACKBONE':
      return set(['N', 'CA', 'C', 'O'])
    ## if no recognised expression, split at ','
    return set([a.strip() for a in atoms.split(',')])
  return set(atoms)


def _EmptyView(view):
  """
  for internal use, only
  """
  if isinstance(view, ost.mol.EntityHandle):
    return view.CreateEmptyView()
  return view.handle.CreateEmptyView()


def _fetch_atoms(r_a, r_b, result_a, result_b, atmset):
  """
  for internal use, only
  """
  ## compare atoms of residues
  for a_a in r_a.GetAtomList():
    if atmset==None or a_a.name in atmset:
      a_b = r_b.FindAtom(a_a.name)
      if a_b.IsValid():
        result_a.AddAtom(a_a)
        result_b.AddAtom(a_b)
  return result_a, result_b


def _no_of_chains(ent_a, ent_b):
  """
  for internal use, only
  """
  ## get lower no. of chains
  if ent_a.chain_count < ent_b.chain_count:
    return ent_a.chain_count
  return ent_b.chain_count


def MatchResidueByNum(ent_a, ent_b, atoms='all'):
  """
  Returns a tuple of views containing exactly the same number of atoms.
  Residues are matched by residue number. A subset of atoms to be included in
  the views can be specified in the **atoms** argument. Regardless of what the
  list of **atoms** says, only those present in two matched residues will be
  included in the views. Chains are processed in the order they occur in the
  entities. If **ent_a** and **ent_b** contain a different number of chains,
  processing stops with the lower count.

  :param ent_a: The first entity
  :type ent_a: :class:`~ost.mol.EntityView` or :class:`~ost.mol.EntityHandle`
  :param ent_b: The second entity
  :type ent_b: :class:`~ost.mol.EntityView` or :class:`~ost.mol.EntityHandle`
  :param atoms: The subset of atoms to be included in the two views.
  :type atoms: :class:`str`, :class:`list`, :class:`set`
  :returns: Two :class:`~ost.mol.EntityView` instances with the same amount of
            residues matched by number. Each residue will have the same number
            & type of atoms.
  """
  ## init. final views
  result_a=_EmptyView(ent_a)
  result_b=_EmptyView(ent_b)
  n_chains=_no_of_chains(ent_a, ent_b)
  atmset=ParseAtomNames(atoms)
  ## iterate chains
  for i in range(0, n_chains):
    chain_a=ent_a.chains[i]
    chain_b=ent_b.chains[i]
    residues_a=iter(chain_a.residues)
    ## decide on order of residues
    if chain_a.InSequence() and chain_b.InSequence():
      residues_b=iter(chain_b.residues)
      ## check residues & copy to views
      try:
        while True:
          r_a=residues_a.next()
          r_b=residues_b.next()
          while r_a.number<r_b.number:
            r_a=residues_a.next()
          while r_b.number<r_a.number:
            r_b=residues_b.next()
          assert r_a.number==r_b.number
          result_a,result_b=_fetch_atoms(r_a, r_b, result_a, result_b, atmset)
      except StopIteration:
        pass
    else:
      ## iterate one list of residues, search in other list
      try:
        while True:
          r_a=residues_a.next()
          r_b=chain_b.FindResidue(r_a.number)
          if r_b.IsValid():
            result_a,result_b=_fetch_atoms(r_a, r_b, result_a, result_b, atmset)
      except StopIteration:
        pass
  result_a.AddAllInclusiveBonds()
  result_b.AddAllInclusiveBonds()
  return result_a, result_b


def MatchResidueByIdx(ent_a, ent_b, atoms='all'):
  """
  Returns a tuple of views containing exactly the same number of atoms.
  Residues are matched by position in the chains of an entity. A subset of
  atoms to be included in the views can be specified in the **atoms** argument.
  Regardless of what the list of **atoms** says, only those present in two
  matched residues will be included in the views. Chains are processed in order
  of appearance. If **ent_a** and **ent_b** contain a different number of
  chains, processing stops with the lower count. The number of residues per
  chain is supposed to be the same.

  :param ent_a: The first entity
  :type ent_a: :class:`~ost.mol.EntityView` or :class:`~ost.mol.EntityHandle`
  :param ent_b: The second entity
  :type ent_b: :class:`~ost.mol.EntityView` or :class:`~ost.mol.EntityHandle`
  :param atoms: The subset of atoms to be included in the two views.
  :type atoms: :class:`str`, :class:`list`, :class:`set`
  :returns: Two :class:`~ost.mol.EntityView` instances with the same amount of
            residues matched by position. Each residue will have the same number
            & type of atoms.
  """
  not_supported="MatchResidueByIdx has no support for chains of different "\
               +"lengths"
  ## init. final views
  result_a=_EmptyView(ent_a)
  result_b=_EmptyView(ent_b)
  n_chains=_no_of_chains(ent_a, ent_b)
  atmset=ParseAtomNames(atoms)
  ## iterate chains
  for i in range(0, n_chains):
    chain_a=ent_a.chains[i]
    chain_b=ent_b.chains[i]
    ## check equal no. of residues
    if chain_a.residue_count!=chain_b.residue_count:
      raise RuntimeError(not_supported)
    ## iterate residues & copy to views
    for r_a, r_b in zip(chain_a.residues, chain_b.residues):
      result_a,result_b=_fetch_atoms(r_a, r_b, result_a, result_b, atmset)
  result_a.AddAllInclusiveBonds()
  result_b.AddAllInclusiveBonds()
  return result_a, result_b


def _MatchResidueByAln(ent_a, ent_b, atoms, alnmethod):
  """
  For internal use, only
  """
  ## init. final views
  result_a = _EmptyView(ent_a)
  result_b = _EmptyView(ent_b)
  n_chains = _no_of_chains(ent_a, ent_b)
  atmset = ParseAtomNames(atoms)
  ## iterate chains
  for i in range(0, n_chains):
    chain_a = ent_a.chains[i]
    chain_b = ent_b.chains[i]
    ## fetch residues
    s_a = ''.join([r.one_letter_code
                   for r in chain_a.Select('protein=True').residues])
    s_b = ''.join([r.one_letter_code
                   for r in chain_b.Select('protein=True').residues])
    ## create sequence from residue lists & alignment
    seq_a = ost.seq.CreateSequence(chain_a.name, s_a)
    seq_b = ost.seq.CreateSequence(chain_b.name, s_b)
    aln_a_b = alnmethod(seq_a, seq_b, ost.seq.alg.BLOSUM62)
    ## evaluate alignment
    for aln in aln_a_b:
      ## bind chain to alignment
      aln.AttachView(0, chain_a.Select('protein=True'))
      aln.AttachView(1, chain_b.Select('protein=True'))
      ## select residues (only replacement edges)
      for i in range(0, aln.GetLength()):
        if aln.sequences[0][i]!='-' and aln.sequences[1][i]!='-':
          r_a = aln.GetResidue(0,i)
          r_b = aln.GetResidue(1,i)
          result_a,result_b=_fetch_atoms(r_a, r_b, result_a, result_b, atmset)
  result_a.AddAllInclusiveBonds()
  result_b.AddAllInclusiveBonds()
  return result_a, result_b

def MatchResidueByLocalAln(ent_a, ent_b, atoms='all'):
  """
  Match residues by local alignment. Takes **ent_a** and **ent_b**, extracts
  the sequences chain-wise and aligns them in Smith/Waterman manner using the
  BLOSUM62 matrix for scoring. The residues of the entities are then matched
  based on this alignment. Only atoms present in both residues are included in
  the views. Chains are processed in order of appearance. If **ent_a** and
  **ent_b** contain a different number of chains, processing stops with
  the lower count.

  :param ent_a: The first entity
  :type ent_a: :class:`~ost.mol.EntityView` or :class:`~ost.mol.EntityHandle`
  :param ent_b: The second entity
  :type ent_b: :class:`~ost.mol.EntityView` or :class:`~ost.mol.EntityHandle`
  :param atoms: The subset of atoms to be included in the two views.
  :type atoms: :class:`str`, :class:`list`, :class:`set`
  :returns: Two :class:`~ost.mol.EntityView` instances with the same number of
            residues. Each residue will have the same number & type of atoms.
  """
  return _MatchResidueByAln(ent_a, ent_b, atoms, ost.seq.alg.LocalAlign)

def MatchResidueByGlobalAln(ent_a, ent_b, atoms='all'):
  """
  Match residues by global alignment. Takes **ent_a** and **ent_b**, extracts
  the sequences chain-wise and aligns them in Needleman/Wunsch manner using the
  BLOSUM62 matrix for scoring. The residues of the entities are then matched
  based on this alignment. Only atoms present in both residues are included in
  the views. Chains are processed in order of appearance. If **ent_a** and
  **ent_b** contain a different number of chains, processing stops with
  the lower count.

  :param ent_a: The first entity
  :type ent_a: :class:`~ost.mol.EntityView` or :class:`~ost.mol.EntityHandle`
  :param ent_b: The second entity
  :type ent_b: :class:`~ost.mol.EntityView` or :class:`~ost.mol.EntityHandle`
  :param atoms: The subset of atoms to be included in the two views.
  :type atoms: :class:`str`, :class:`list`, :class:`set`
  :returns: Two :class:`~ost.mol.EntityView` instances with the same number of
            residues. Each residue will have the same number & type of atoms.
  """
  return _MatchResidueByAln(ent_a, ent_b, atoms, ost.seq.alg.GlobalAlign)


def Superpose(ent_a, ent_b, match='number', atoms='all'):
  """
  Superposes the model entity onto the reference. To do so, two views are
  created, returned with the result. **atoms** describes what goes in to these
  views and **match** the selection method. For superposition,
  :func:`~ost.mol.alg.SuperposeSVD` is called. For matching, following methods
  are recognised:

  * ``number`` - select residues by residue number, includes **atoms**, calls
    :func:`~ost.mol.alg.MatchResidueByNum`

  * ``index`` - select residues by index in chain, includes **atoms**, calls
    :func:`~ost.mol.alg.MatchResidueByIdx`

  * ``local-aln`` - select residues from a Smith/Waterman alignment, includes
    **atoms**, calls :func:`~ost.mol.alg.MatchResidueByLocalAln`

  * ``global-aln`` - select residues from a Needleman/Wunsch alignment, includes
    **atoms**, calls :func:`~ost.mol.alg.MatchResidueByGlobalAln`

  :param ent_a: The model entity
  :type ent_a: :class:`~ost.mol.EntityView` or :class:`~ost.mol.EntityHandle`
  :param ent_b: The reference entity
  :type ent_b: :class:`~ost.mol.EntityView` or :class:`~ost.mol.EntityHandle`
  :param match: Method to gather residues/ atoms
  :type match: :class:`str`
  :param atoms: The subset of atoms to be used in the superposition.
  :type atoms: :class:`str`, :class:`list`, :class:`set`
  :returns: An instance of :class:`SuperpositionResult`, containing members

            * ``rmsd`` - RMSD of the superposed entities

            * ``view1`` - First :class:`~ost.mol.EntityView` used

            * ``view2`` - Second :class:`~ost.mol.EntityView` used
  """
  not_supported="Superpose called with unsupported matching request."
  ## create views to superpose
  if match.upper() == 'NUMBER':
    view_a, view_b = MatchResidueByNum(ent_a, ent_b, atoms)
  elif match.upper() == 'INDEX':
    view_a, view_b=MatchResidueByIdx(ent_a, ent_b, atoms)
  elif match.upper() == 'LOCAL-ALN':
    view_a, view_b=_MatchResidueByAln(ent_a, ent_b, atoms,
                                      ost.seq.alg.LocalAlign)
  elif match.upper() == 'GLOBAL-ALN':
    view_a, view_b=_MatchResidueByAln(ent_a, ent_b, atoms,
                                      ost.seq.alg.GlobalAlign)
  else:
    raise ValueError(not_supported)
  ## action
  res=ost.mol.alg.SuperposeSVD(view_a, view_b)
  return res
