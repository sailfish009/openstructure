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
  
  def testBlastOutput(self):
    hits=blast.Blast(self.query, 'testfiles/seqdb')
    expected_hits=['1griA','1fhsA','3n84E','1mw9X']
    self.assertEqual(len(hits), 4)

    found=0
    hit_ids=[]

    for h in hits:
      hit_ids.append(h.identifier)

    for identifier in expected_hits:
      if identifier in hit_ids:
        found+=1
    self.assertEqual(found, 4)

  def testBlastParseOutput(self):
    raw_out=open('testfiles/raw_blastout.txt','r').readlines()
    parsed_out=blast.ParseBlastOutput(raw_out)

    expected_ids=['1griA','1fhsA','3n84E','1mw9X']
    expected_evalues=[4.80893e-59,4.80893e-59,9.06925e-58,2.96523]
    expected_scores=[534,534,523,27]
    expected_bitscores[210.305,210.305,206.068,15.0086]
    expected_seqid=[1.0,1.0,1.0,0.30769]

    for i in range(4):
      patch=parsed_out[i].aligned_patches[0]
      self.assertEqual(expected_ids[i],parsed_out[i].identifier)
      self.assertAlmostEqual(patch.evalue,expected_evalues[i])
      self.assertAlmostEqual(patch.score,expected_scores[i])
      self.assertAlmostEqual(patch.bit_score,expected_bitscores[i])
      self.assertAlmostEqual(patch.seqid,expected_seqid[i],places=5)


    
if __name__ == "__main__":
  # test if blast package is available on system, otherwise ignore tests
  try:
    blastpath=settings.Locate('blastp')
  except(settings.FileNotFound):
    try:
      blastpath=settings.Locate('blastall')
    except:
      print "Could not find blast executable: ignoring unit tests"
      sys.exit(0)
  from ost import testutils
  testutils.RunTests()
