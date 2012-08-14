import unittest
from ost import mol
from ost import conop

class TestCompound(unittest.TestCase):
  
  def setUp(self):
    self.compound_lib=conop.GetDefaultLib()

  def testFindCompound(self):
    compound=self.compound_lib.FindCompound('***')
    self.assertEqual(compound, None)
    compound=self.compound_lib.FindCompound('ALA')
    self.assertNotEqual(compound, None)
    self.assertEqual(compound.id, 'ALA')
    self.assertEqual(compound.three_letter_code, 'ALA')
    self.assertEqual(compound.one_letter_code, 'A')
    self.assertTrue(compound.IsPeptideLinking())
    self.assertEqual(compound.dialect, 'PDB')
    self.assertEqual(compound.formula, 'C3 H7 N O2')
    self.assertEqual(compound.chem_class, mol.L_PEPTIDE_LINKING)

     
if __name__=='__main__':
  if not conop.GetDefaultLib():
    print 'No compound library available. ignoring compound unit tests'
  else:
    from ost import testutils
    testutils.RunTests()

