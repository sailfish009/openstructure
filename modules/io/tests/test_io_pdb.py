import unittest
from ost import *

class TestPDB(unittest.TestCase):
  def setUp(self):
    pass

  def test_compnd_parser(self):
    e=io.LoadPDB('testfiles/pdb/compnd.pdb', restrict_chains="A")
    self.assertEquals(e.GetChainCount(), 1)
    ch = e.FindChain("A");
    self.assertEquals(ch.GetIntProp("mol_id"), 1)

class TestPDB(unittest.TestCase):
  def setUp(self):
    pass

  def test_compnd_parser(self):
    profiles=io.IOProfiles()
    profiles['FEAS_CHECK']=io.IOProfile(bond_feasibility_check=True)
    profiles['NO_FEAS_CHECK']=io.IOProfile(bond_feasibility_check=False)
        
    e1=io.LoadPDB('testfiles/pdb/simple_defective.pdb', restrict_chains="A",profile='FEAS_CHECK')
    e2=io.LoadPDB('testfiles/pdb/simple_defective.pdb', restrict_chains="A",profile='NO_FEAS_CHECK')
    ed=io.LoadPDB('testfiles/pdb/simple_defective.pdb', restrict_chains="A")
    
    res1=e1.FindResidue('A',3)
    res2=e2.FindResidue('A',3)
    resd=ed.FindResidue('A',3)

    self.assertFalse(mol.BondExists(res1.FindAtom("CA"),res1.FindAtom("CB")))
    self.assertTrue(mol.BondExists(res2.FindAtom("CA"),res2.FindAtom("CB")))
    self.assertTrue(mol.BondExists(resd.FindAtom("CA"),resd.FindAtom("CB")))
    
if __name__== '__main__':
  from ost import testutils
  testutils.RunTests()


 
