from ost.bindings import dssp
ent=io.LoadMMCIF('1ake.cif')
dssp.AssignDSSP(ent, extract_burial_status=True)
for chain in ent.chains:
  if chain.is_polypeptide:
    for res in chain.residues:
      print res.GetFloatProp('relative_solvent_accessibility')
