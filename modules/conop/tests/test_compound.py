import unittest
import os
from ost import GetSharedDataPath, SetPrefixPath
from ost import mol
from ost import conop

def setUpModule():
    SetPrefixPath(os.path.abspath(os.path.join(conop.__path__[0], os.pardir,
                                               os.pardir, os.pardir,
                                               os.pardir, os.pardir)))
    compound_lib_path = os.path.join(GetSharedDataPath(),
                                     'compounds.chemlib')
    compound_lib = conop.CompoundLib.Load(compound_lib_path)
    conop.SetDefaultLib(compound_lib)

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
        self.assertEqual(compound.inchi,
                        "1S/C3H7NO2/c1-2(4)3(5)6/h2H,4H2,1H3,(H,5,6)/t2-/m0/s1")
        self.assertEqual(compound.inchi_key, "QNAYBMKLOCPYGJ-REOHCLBHSA-N")

     
if __name__=='__main__':
    from ost import testutils
    testutils.RunTests()

