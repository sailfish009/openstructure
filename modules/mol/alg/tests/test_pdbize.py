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
    self.assertEquals([c.name for c in pdbized.chains], ["-"])
    residues = pdbized.residues
    for i in range(26):
      self.assertEquals(residues[i].number.num, 1)
      self.assertEquals(residues[i].number.ins_code, chr(ord('A')+i))
    self.assertEquals(residues[26].number.num, 2)
    self.assertEquals(residues[26].number.ins_code, 'A')
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
    self.assertEquals([c.name for c in pdbized.chains], ["-"])
    residues = pdbized.residues
    self.assertEquals([r.number for r in residues],
                      [mol.ResNum(1, 'A'), mol.ResNum(1, 'B')])




if __name__ == "__main__":
  from ost import testutils
  testutils.RunTests()
