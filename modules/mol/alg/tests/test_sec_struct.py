from ost import io, mol, settings
import unittest
import os
from ost.bindings import dssp


class TestSecStruct(unittest.TestCase):
  
  def testSecStruct(self):

    # unit test only makes sense, when a dssp binary is around
    dssp_path = None
    try:
      dssp_path = settings.Locate("dssp")
    except:
      try:
        dssp_path = settings.Locate("mkdssp")
      except:
        print "Could not find dssp, could not compare sec struct assignment..."
        return

    dssp_ent = io.LoadPDB(os.path.join("testfiles", "1a0s.pdb"))
    ost_ent = io.LoadPDB(os.path.join("testfiles", "1a0s.pdb"))

    dssp.AssignDSSP(dssp_ent, dssp_bin = dssp_path)
    mol.alg.AssignSecStruct(ost_ent)

    for a, b in zip(dssp_ent.residues, ost_ent.residues):
      self.assertTrue(str(a.GetSecStructure()) == str(b.GetSecStructure()))
      

if __name__ == "__main__":

  from ost import testutils
  testutils.RunTests()