"""
Superposition of structures made simple.

Authors: Stefan Bienert
"""

import math
import ost.mol.alg


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
  """
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
  """
  ## init. final views
  result_a=_EmptyView(ent_a)
  result_b=_EmptyView(ent_b)
  ## get lower no. of chains
  if ent_a.chain_count < ent_b.chain_count:
    n_chains=ent_a.chain_count
  else:
    n_chains=ent_b.chain_count
  ## get a set of atoms or None
  atmset=ParseAtomNames(atoms)
  ## iterate chains
  for i in range(0, n_chains):
    chain_a=ent_a.chains[i]
    chain_b=ent_b.chains[i]
    ## residues of chains need to be consecutively numbered (sorted)
    if chain_a.InSequence() and chain_b.InSequence():
      residues_a=iter(chain_a.residues)
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
      residues_a=iter(chain_a.residues)
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


def Superpose(ent_a, ent_b, match='number', atoms='all'):
  """
  Superposes the first entity onto the second. To do so, two views are created,
  returned with the result. **atoms** describes what goes in to these views and
  **match** the selection method. For superposition,
  :func:`~ost.mol.alg.SuperposeSVD` is called. For matching, following methods
  are recognised:

  * ``number`` - select residues by residue number, includes **atoms**, calls
    :func:`~ost.mol.alg.MatchResidueByNum`

  :param ent_a: The first entity
  :type ent_a: :class:`~ost.mol.EntityView` or :class:`~ost.mol.EntityHandle`
  :param ent_b: The second entity
  :type ent_b: :class:`~ost.mol.EntityView` or :class:`~ost.mol.EntityHandle`
  :param match: Method to gather residues/ atoms
  :type match: :class:`str`
  :param atoms: The subset of atoms to be used in the superposition.
  :type atoms: :class:`str`, :class:`list`, :class:`set`
  """
  not_supported="Superpose called with unsupported matching request."
  ## create views to superpose
  if match.upper()=='NUMBER':
    view_a, view_b=MatchResidueByNum(ent_a, ent_b, atoms)
  else:
    raise ValueError(not_supported)
  ## action
  res=ost.mol.alg.SuperposeSVD(view_a, view_b)
  return res
