import unittest, os
from ost import io
from ost.mol.alg.qsscoring import *


def _LoadFile(file_name):
  """Helper to avoid repeating input path over and over."""
  return io.LoadPDB(os.path.join('testfiles', file_name))


class TestQSscore(unittest.TestCase):

  def test_HeteroCase1(self):
    # additional chains
    ent_1 = _LoadFile('4ux8.1.pdb') # A2 B2 C2, symmetry: C2
    ent_2 = _LoadFile('3fub.2.pdb') # A2 B2   , symmetry: C2
    qs_scorer = QSscorer(ent_1, ent_2)
    # check properties
    self.assertEqual(qs_scorer.calpha_only, False)
    # check mappings
    self.assertEqual(qs_scorer.chem_mapping,
                     {('D', 'F'): ('B', 'D'), ('C', 'E'): ('A', 'C')})
    self.assertEqual(sorted(qs_scorer.symm_1), [('D', 'C'), ('F', 'E')])
    self.assertEqual(sorted(qs_scorer.symm_2), [('A', 'B'), ('C', 'D')])
    self.assertEqual(qs_scorer.chain_mapping,
                     {'C': 'A', 'E': 'C', 'D': 'B', 'F': 'D'})
    # check scoring
    self.assertAlmostEqual(qs_scorer.global_score, 0.825, 2)
    # without penalties the interface is the same
    self.assertAlmostEqual(qs_scorer.best_score, 1.0, 2)

  def test_HeteroCase1b(self):
    # as above but with assymetric unit of 3fub
    # -> no overlap found: check if extensive search can deal with it
    ent_1 = _LoadFile('4ux8.1.pdb')
    ent_2 = _LoadFile('3fub.au.pdb')
    qs_scorer = QSscorer(ent_1, ent_2)
    # check properties
    self.assertEqual(qs_scorer.calpha_only, False)
    # check mappings
    self.assertEqual(qs_scorer.chem_mapping,
                     {('C', 'E'): ('A', 'C'), ('D', 'F'): ('D', 'B')})
    self.assertEqual(sorted(qs_scorer.symm_1), [('D', 'C'), ('F', 'E')])
    self.assertEqual(sorted(qs_scorer.symm_2), [('B', 'A'), ('D', 'C')])
    self.assertEqual(qs_scorer.chain_mapping,
                     {'C': 'A', 'E': 'C', 'D': 'B', 'F': 'D'})
    # check scoring
    self.assertAlmostEqual(qs_scorer.global_score, 0.356, 2)
    self.assertAlmostEqual(qs_scorer.best_score, 0.419, 2)

  def test_HeteroCase2(self):
    # different stoichiometry
    ent_1 = _LoadFile('1efu.1.pdb') # A2 B2, symmetry: C2
    ent_2 = _LoadFile('4pc6.1.pdb') # A B  , no symmetry
    qs_scorer = QSscorer(ent_1, ent_2)
    self.assertAlmostEqual(qs_scorer.global_score, 0.3131, 2)
    # without penalties the interface is the same
    self.assertAlmostEqual(qs_scorer.best_score, 0.941, 2)

  def test_HeteroCase3(self):
    # more chains
    ent_1 = _LoadFile('2vjt.1.pdb') # A6 B6, symmetry: D3
    ent_2 = _LoadFile('3dbj.1.pdb') # A3 B3, symmetry: C3
    qs_scorer = QSscorer(ent_1, ent_2)
    self.assertAlmostEqual(qs_scorer.global_score, 0.359, 2)
    self.assertAlmostEqual(qs_scorer.best_score, 0.958, 2)
    # user's symmetry groups
    symm_1 = [('A', 'B'), ('C', 'D'), ('E', 'F'),
              ('G', 'H'), ('I', 'J'), ('K', 'L')]
    symm_2 = [('A', 'B'), ('C', 'D'), ('E', 'F')]
    # use QSscoreEntity to go faster
    qs_scorer_symm = QSscorer(qs_scorer.qs_ent_1, qs_scorer.qs_ent_2)
    qs_scorer_symm.SetSymmetries(symm_1, symm_2)
    self.assertAlmostEqual(qs_scorer_symm.global_score, 0.359, 2)
    self.assertAlmostEqual(qs_scorer_symm.best_score, 0.958, 2)

  def test_HeteroCase4(self):
    # inverted chains
    ent_1 = _LoadFile('3ia3.1.pdb') # AB, no symmetry
    ent_2 = _LoadFile('3ia3.2.pdb') # BA, no symmetry
    qs_scorer = QSscorer(ent_1, ent_2)
    self.assertAlmostEqual(qs_scorer.global_score, 0.980, 2)
    self.assertAlmostEqual(qs_scorer.best_score, 0.980, 2)

  def test_HeteroModel(self):
    # uncomplete model missing 2 third of the contacts
    ent_1 = _LoadFile('1eud_ref.pdb')               # AB, no symmetry
    ent_2 = _LoadFile('1eud_mdl_partial-dimer.pdb') # BA, no symmetry
    qs_scorer = QSscorer(ent_1, ent_2)
    self.assertAlmostEqual(qs_scorer.global_score, 0.323, 2)
    self.assertAlmostEqual(qs_scorer.best_score, 0.921, 2)


  # TESTS HOMO

  def test_HomoCase1(self):
    # different stoichiometry SOD
    ent_1 = _LoadFile('4dvh.1.pdb') # A2, symmetry: C2
    ent_2 = _LoadFile('4br6.1.pdb') # A4, symmetry: D2
    qs_scorer = QSscorer(ent_1, ent_2)
    self.assertAlmostEqual(qs_scorer.global_score, 0.147, 2)
    self.assertAlmostEqual(qs_scorer.best_score, 0.866, 2)

  def test_lDDT(self):
    # lDDT is not symmetrical and does not account for overprediction!
    ref = _LoadFile('4br6.1.pdb').Select('cname=A,B')
    mdl = _LoadFile('4br6.1.pdb')
    qs_scorer = QSscorer(ref, mdl)
    self.assertAlmostEqual(qs_scorer.global_score, 0.171, 2)
    self.assertAlmostEqual(qs_scorer.best_score, 1.00, 2)
    self.assertAlmostEqual(qs_scorer.lddt_score, 1.00, 2)
    # flip them (use QSscoreEntity to go faster)
    qs_scorer2 = QSscorer(qs_scorer.qs_ent_2, qs_scorer.qs_ent_1)
    self.assertAlmostEqual(qs_scorer2.global_score, 0.171, 2)
    self.assertAlmostEqual(qs_scorer2.best_score, 1.00, 2)
    self.assertAlmostEqual(qs_scorer2.lddt_score, 0.483, 2)

  def test_HomoCase2(self):
    # broken cyclic symmetry
    ent_1 = _LoadFile('4r7y.1.pdb')   # A6, symmetry: C6
    ent_2 = ent_1.Select('cname=A,B') # A2, no symmetry
    qs_scorer = QSscorer(ent_1, ent_2)
    # only one interface over 6
    self.assertAlmostEqual(qs_scorer.global_score, 1/6., 2)
    # without penalties the interface is the same
    self.assertAlmostEqual(qs_scorer.best_score, 1.0, 2)
    # using user symmetry groups
    symm_1 = [('A', 'B'), ('C', 'D'), ('E', 'F')]
    symm_2 = [('A', 'B')]
    # use QSscoreEntity to go faster
    qs_scorer_symm = QSscorer(qs_scorer.qs_ent_1, qs_scorer.qs_ent_2)
    qs_scorer_symm.SetSymmetries(symm_1, symm_2)
    self.assertEqual(qs_scorer_symm.global_score, qs_scorer.global_score)
    self.assertEqual(qs_scorer_symm.best_score, qs_scorer.best_score)


  # TEST BIG STUFF and FANCY SYMMETRIES

  def test_HeteroBig(self):
    # comparing rubisco with half of it
    ent_1 = _LoadFile('4f0h.1.pdb')      # A8 B8, symmetry: D4
    ent_2 = _LoadFile('4f0h.1_half.pdb') # A4 B4, symmetry: C4
    qs_scorer = QSscorer(ent_1, ent_2)
    self.assertAlmostEqual(qs_scorer.global_score, 1/4., 2)
    self.assertAlmostEqual(qs_scorer.best_score, 1.0, 2)

  def test_Capsid(self):
    ent_1 = _LoadFile('4gh4.2.pdb') # A5 B5 C5 D5, symmetry: C5
    ent_2 = _LoadFile('1qqp.2.pdb') # A5 B5 C5 D5, symmetry: C5
    qs_scorer = QSscorer(ent_1, ent_2)
    self.assertAlmostEqual(qs_scorer.global_score, 0.921, 2)
    self.assertAlmostEqual(qs_scorer.best_score, 0.941, 2)

  def test_TetrahedralSymmetry(self):
    ent_1 = _LoadFile('1mog.1.pdb') # A12, symmetry: T
    ent_2 = _LoadFile('2cc6.1.pdb') # A12, symmetry: T
    qs_scorer = QSscorer(ent_1, ent_2)
    self.assertAlmostEqual(qs_scorer.global_score, 0.954, 2)
    self.assertAlmostEqual(qs_scorer.best_score, 0.994, 2)

  def test_Urease(self):
    ent_1 = _LoadFile('1e9y.1.pdb') # A12 B12, symmetry: T
    ent_2 = _LoadFile('1e9z.1.pdb') # A12 B12, symmetry: T
    qs_scorer = QSscorer(ent_1, ent_2)
    self.assertAlmostEqual(qs_scorer.global_score, 0.958, 2)
    self.assertAlmostEqual(qs_scorer.best_score, 0.958, 2)

  def test_C6SymmetryHetero(self):
    ent_1 = _LoadFile('3j3r.1.pdb') # A6 B6, symmetry: C6
    ent_2 = _LoadFile('3j3s.1.pdb') # A6 B6, symmetry: C6
    qs_scorer = QSscorer(ent_1, ent_2)
    self.assertAlmostEqual(qs_scorer.global_score, 0.559, 2)
    self.assertAlmostEqual(qs_scorer.best_score, 0.559, 2)

  def test_OctahedralSymmetry(self):
    ent_1 = _LoadFile('3vcd.1.pdb') # A24, symmetry: O
    ent_2 = _LoadFile('4ddf.1.pdb') # A24, symmetry: O
    qs_scorer = QSscorer(ent_1, ent_2)
    self.assertAlmostEqual(qs_scorer.global_score, 0.975, 2)
    self.assertAlmostEqual(qs_scorer.best_score, 0.975, 2)


if __name__ == "__main__":
  from ost import testutils
  if testutils.SetDefaultCompoundLib():
    testutils.RunTests()
  else:
    print 'No compound library available. Ignoring test_qsscoring.py tests.'
