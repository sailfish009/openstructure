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

if __name__== '__main__':
    unittest.main()


