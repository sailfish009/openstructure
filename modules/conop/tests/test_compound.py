import unittest
from ost import mol
from ost import conop

class TestCompound(unittest.TestCase):
  
  def setUp(self):
    self.compound_lib=conop.GetBuilder().compound_lib

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
  builder=conop.GetBuilder()
  if not hasattr(builder, 'compound_lib'):
    print 'default builder does not use compound library. ignoring unit tests'
  else:
    suite = unittest.TestLoader().loadTestsFromTestCase(TestCompound)
    unittest.TextTestRunner().run(suite)

