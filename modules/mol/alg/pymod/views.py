"""
 Algorithms operating on two views. 

All these algorithms return a tuple of views that contain the same number of
residues. Thes residues at corresponding indices in these two views form pairs.

Author: Marco Biasini  
"""

from ost import io,mol

def _GetChain(view):
  if view.chain_count!=1:
    raise RuntimeError("chain count of view must be one")
  return view.chains[0]
  
def _GetChains(view_a, view_b):
  return _GetChain(view_a), _GetChain(view_b)


def _EmptyView(view):
  if isinstance(view, mol.EntityHandle):
    return view.CreateEmptyView()
  return view.handle.CreateEmptyView()


def PairResiduesByNum(view_a, view_b, 
                      view_add_flags=mol.ViewAddFlag.INCLUDE_ATOMS):
  """
  Pair residues by residue number.
  """
  not_supported="PairResiduesByNum has no support for unsorted chains"
  chain_a, chain_b=_GetChains(view_a, view_b)
  if not chain_a.InSequence() or not chain_b.InSequence():
    raise RuntimeError(not_supported)
  residues_a=chain_a.residues
  residues_b=chain_b.residues
  residues_a=iter(chain_a.residues)
  residues_b=iter(chain_b.residues)
  result_a=_EmptyView(view_a)
  result_b=_EmptyView(view_b)
  try:
    while True:
      r1=residues_a.next()
      r2=residues_b.next()
      while r1.number<r2.number:
        r1=residues_a.next()
      while r2.number<r1.number:
        r2=residues_b.next()
      assert r1.number==r2.number
      result_a.AddResidue(r1, view_add_flags)
      result_b.AddResidue(r2, view_add_flags)
  except StopIteration:
    pass
  return result_a, result_b
