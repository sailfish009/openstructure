import unittest
from ost import *
from ost import settings
from ost.bindings import blast

class TestBlastBindings(unittest.TestCase):
  def setUp(self):
    self.query=seq.CreateSequence('A', 'MKPHPWFFGKIPRAKAEEMLSKQRHDGAFLIRESESAPG'+
                                       'DFSLSVKFGNDVQHFKVLRDGAGKYFLWVVKFNSLNELV'+
                                       'DYHRSTSVSRNQQIFLRDIEQVP')
  def testAllowedMatricesAndPenalties(self):
    self.assertRaises(blast.BlastError, blast.Blast, self.query, 
                      'testfiles/seqdb', matrix='BLOSUM45')
    blast.Blast(self.query, 'testfiles/seqdb', matrix='BLOSUM45', 
                gap_open=13, gap_ext=3)
    blast.Blast(self.query, 'testfiles/seqdb', matrix='BLOSUM80')
    blast.Blast(self.query, 'testfiles/seqdb', matrix='PAM70')
    blast.Blast(self.query, 'testfiles/seqdb', matrix='PAM30', gap_open=7,
                gap_ext=2)
    blast.Blast(self.query, 'testfiles/seqdb', matrix='BLOSUM62')
    self.assertRaises(ValueError, blast.Blast, self.query, 'testfiles/seqdb', 
                      matrix='MUAHA')
  def testMissingDB(self):
    self.assertRaises(IOError, blast.Blast, self.query, 
                      'testfiles/nonexistentdb')
  
  def testParseBlastOutput(self):
    hits=blast.Blast(self.query, 'testfiles/seqdb')
    blast_version=blast.BlastVersion()
    if blast_version=='2.2.16':
      expected_output=[{'evalue':4.808930E-59,'bitscore':210.305,'score':534}, 
                       {'evalue':2.366130E-59,'bitscore':210.305,'score':534}, 
                       {'evalue':5.361450E-58,'bitscore':206.068,'score':523}, 
                       {'evalue':2.965230E+00,'bitscore':15.0086,'score':27}, 
                       {'evalue':9.696520E+00,'bitscore':13.4678,'score':23}]
 
    else:
      expected_output=[{'evalue':2.366130E-59,'bitscore':211.460,'score':537}, 
                       {'evalue':4.808930E-59,'bitscore':210.305,'score':534}, 
                       {'evalue':5.361450E-58,'bitscore':206.838,'score':525}, 
                       {'evalue':3.277500E+00,'bitscore':15.0086,'score':27}, 
                       {'evalue':9.696520E+00,'bitscore':13.4678,'score':23}]
    self.assertEqual(len(hits), 4)
    for expected, hit in zip(expected_output, hits):
      patch=hit.aligned_patches[0]
      self.assertAlmostEqual(patch.evalue, expected['evalue'])
      self.assertAlmostEqual(patch.bit_score, expected['bitscore'])
      self.assertAlmostEqual(patch.score, expected['score'])
if __name__ == "__main__":
  # test if blast package is available on system, otherwise ignore tests
  try:
    blastpath=settings.Locate('blastall')
  except(settings.FileNotFound):
    print "Could not find blastall executable: ignoring unit tests"
    sys.exit(0)
  from ost import testutils
  testutils.RunTests()
