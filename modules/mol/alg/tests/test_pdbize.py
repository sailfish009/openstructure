from ost import io, mol, geom, seq
import unittest
import os
import random

class TestPDBize(unittest.TestCase):
  
  def test_numbers_water_molecules_with_ins_codes(self):
    m = mol.CreateEntity()
    e = m.EditXCS(mol.BUFFERED_EDIT)
    c = e.InsertChain("A");
    e.SetChainType(c, mol.CHAINTYPE_WATER)
    for i in range(27):
      e.AppendResidue(c, "HOH")
    pdbizer = mol.alg.PDBize()
    transformations = geom.Mat4List()
    transformations.append(geom.Mat4())
    seqs = seq.CreateSequenceList()
    pdbizer.Add(m.Select(''), transformations, seqs)
    pdbized = pdbizer.Finish()
    self.assertEqual([c.name for c in pdbized.chains], ["-"])
    residues = pdbized.residues
    for i in range(26):
      self.assertEqual(residues[i].number.num, 1)
      self.assertEqual(residues[i].number.ins_code, chr(ord('A')+i))
    self.assertEqual(residues[26].number.num, 2)
    self.assertEqual(residues[26].number.ins_code, 'A')

  def test_starts_from_last_water_rnum(self):
    m = mol.CreateEntity()
    e = m.EditXCS(mol.BUFFERED_EDIT)
    c = e.InsertChain("A");
    e.SetChainType(c, mol.CHAINTYPE_WATER)
    e.AppendResidue(c, "HOH")
    pdbizer = mol.alg.PDBize()
    transformations = geom.Mat4List()
    transformations.append(geom.Mat4())
    seqs = seq.CreateSequenceList()
    pdbizer.Add(m.Select(''), transformations,seqs)
    pdbizer.Add(m.Select(''), transformations,seqs)
    pdbized = pdbizer.Finish()
    self.assertEqual([c.name for c in pdbized.chains], ["-"])
    residues = pdbized.residues
    self.assertEqual([r.number for r in residues],
                      [mol.ResNum(1, 'A'), mol.ResNum(1, 'B')])

  def test_large_poly_ligand_inscode(self):
    """Make sure polymeric ligands with 26+ residues get split up and do not
    run into an overflow with the insertion codes.
    """
    m = mol.CreateEntity()
    e = m.EditXCS(mol.BUFFERED_EDIT)
    c = e.InsertChain("A");
    e.SetChainType(c, mol.CHAINTYPE_OLIGOSACCHARIDE)
    for i in range(27):
      e.AppendResidue(c, "NAG")
    pdbizer = mol.alg.PDBize(min_polymer_size=28)
    transformations = geom.Mat4List()
    transformations.append(geom.Mat4())
    seqs = seq.CreateSequenceList()
    pdbizer.Add(m.Select(''), transformations, seqs)
    pdbized = pdbizer.Finish()
    self.assertEqual([c.name for c in pdbized.chains], ["_"])
    residues = pdbized.residues
    for i in range(26):
      self.assertEqual(residues[i].number.num, 1)
      self.assertEqual(residues[i].number.ins_code, chr(ord('A')+i))
    self.assertEqual(residues[26].number.num, 2)
    self.assertEqual(residues[26].number.ins_code, 'A')

  def test_large_poly_ligand_resnum(self):
    """Make sure polymeric ligands with 26 residues do not increase the residue
    number by 2. Otherwise the next ligand is off by 1, like NAG26Z, ZN28.
    """
    m = mol.CreateEntity()
    e = m.EditXCS(mol.BUFFERED_EDIT)
    c = e.InsertChain("A");
    e.SetChainType(c, mol.CHAINTYPE_OLIGOSACCHARIDE)
    for i in range(26):
      e.AppendResidue(c, "NAG")
    c = e.InsertChain("B");
    e.AppendResidue(c, "ZN")
    pdbizer = mol.alg.PDBize(min_polymer_size=28)
    transformations = geom.Mat4List()
    transformations.append(geom.Mat4())
    seqs = seq.CreateSequenceList()
    pdbizer.Add(m.Select(''), transformations, seqs)
    pdbized = pdbizer.Finish()
    self.assertEqual([c.name for c in pdbized.chains], ["_"])
    residues = pdbized.residues
    for i in range(26):
      self.assertEqual(residues[i].number.num, 1)
      self.assertEqual(residues[i].number.ins_code, chr(ord('A')+i))
    self.assertEqual(residues[26].number.num, 2)
    self.assertEqual(residues[26].number.ins_code, '\0')

if __name__ == "__main__":
  from ost import testutils
  testutils.RunTests()
