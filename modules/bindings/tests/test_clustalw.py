import unittest
from ost import *
from ost import settings
from ost.bindings import clustalw

class TestClustalWBindings(unittest.TestCase):
  
  def setUp(self):
    self.protein = io.LoadEntity("testfiles/testprotein.pdb").Select("cname=A")

    self.targetseq = io.LoadSequence("testfiles/test.fasta")
    self.targetseq.AttachView(self.protein)
    self.templseq = io.LoadSequence("testfiles/similar.fasta")
    self.multseq = io.LoadSequenceList("testfiles/multiple.fasta")
    self.pw_alignment = io.LoadAlignment("testfiles/pairwise_aln.fasta")
    self.nopgap_pw_alignment = io.LoadAlignment("testfiles/nopgap_pairwise_aln.fasta")
    self.mult_alignment = io.LoadAlignment("testfiles/multiple_aln.fasta")
    
  def testPairwiseClustalW(self):
    aln=clustalw.ClustalW(self.targetseq, self.templseq)
    assert self.pw_alignment.ToString(80) == aln.ToString(80), \
           "Pairwise alignment differs from precomputed one"
  
  def testNoPGapPariwiseClustalW(self):
    aln=clustalw.ClustalW(self.targetseq, self.templseq, nopgap=True)
    assert self.nopgap_pw_alignment.ToString(80) == aln.ToString(80), \
           "NoPGap pairwise alignment differs from precomputed one"

  def testAttachedViewClustalW(self):
    aln=clustalw.ClustalW(self.targetseq, self.templseq)
    assert aln.FindSequence("testseq").HasAttachedView(), \
           "Aligned sequence doesn't have an attached view"
    
  def testMultipleClustalW(self):
    aln=clustalw.ClustalW(self.multseq)
    assert self.mult_alignment.ToString(80) == aln.ToString(80), \
           "Multiple alignment differs from precomputed one"


if __name__ == "__main__":
  # test if clustalw package is available on system, otherwise ignore tests
  try:
    clustalw_path=settings.Locate(('clustalw', 'clustalw2'))
  except(settings.FileNotFound):
    print "Could not find clustalw executable: ignoring unit tests"
    exit(0)
  try:
    unittest.main()
  except Exception, e:
    print e