import unittest
from ost import *
from ost import settings
from ost import seq

class TestSeq(unittest.TestCase):
  
  def setUp(self):
    self.protein = io.LoadEntity("testfiles/testprotein.pdb")
    c=self.protein.chains[0]
    self.seq_a=seq.SequenceFromChain('0',c) # MRLDGKTALITGSAR
    self.seq_a.AttachView(self.protein.Select('aname==CA'))
    self.seq_b=seq.CreateSequence('1','---DGKTALITGSAR')
    self.seq_b.AttachView(self.protein.Select('aname==CA'))
    self.seq_c=seq.CreateSequence('2','MRLDG---LITGSAR')
    self.seq_c.AttachView(self.protein.Select('aname==CA'))
    self.seq_d=seq.CreateSequence('3','MRLDG----ITGSAR')
    self.seq_d.AttachView(self.protein.Select('aname==CA'))
    self.seq_e=seq.CreateSequence('4','AMRLDG----ITGSA')
    self.seq_e.AttachView(self.protein.Select('aname==CA'))
    self.seq_e.SetSequenceOffset(1)
    
  def testViewsFromSequencesGapFrontSeqB(self):
    #~ MRLDGKTALITGSAR
    #~ ---DGKTALITGSAR
    [a,b]=seq.ViewsFromSequences(self.seq_a,
                                 self.seq_b,
                                 self.seq_a.GetAttachedView(),
                                 self.seq_b.GetAttachedView())


    assert(a.atoms[0].GetResidue().GetOneLetterCode()=='D');
    assert(b.atoms[0].GetResidue().GetOneLetterCode()=='D');
                  #~ self.seq_b.GetResidue(j.GetResidue().GetIndex()).GetOneLetterCode());
  
  def testViewsFromSequencesGapMiddleBoth(self):
    #~ MRLDG---LITGSAR
    #~ MRLDG----ITGSAR
    [a,b]=seq.ViewsFromSequences(self.seq_c,
                                 self.seq_d,
                                 self.seq_c.GetAttachedView(),
                                 self.seq_d.GetAttachedView())

    print 'aaa'+a.atoms[4].name+'aaaa'
    assert(a.atoms[0].GetResidue().GetOneLetterCode()=='M');
    assert(b.atoms[0].GetResidue().GetOneLetterCode()=='M');
    assert(a.atoms[4].GetResidue().GetOneLetterCode()=='I');
    assert(b.atoms[4].GetResidue().GetOneLetterCode()=='I');
                  #~ self.seq_b.GetResidue(j.GetResidue().GetIndex()).GetOneLetterCode());
                  
  def testViewsFromSequencesGapSeqOffset(self):
    #~ MRLDGKTALITGSAR
    #~ AMRLDG----ITGSA
    [a,b]=seq.ViewsFromSequences(self.seq_a,
                                 self.seq_e,
                                 self.seq_a.GetAttachedView(),
                                 self.seq_e.GetAttachedView())


    assert(a.atoms[0].GetResidue().GetOneLetterCode()=='M');
    assert(b.atoms[0].GetResidue().GetOneLetterCode()=='M');
    assert(a.atoms[5].GetResidue().GetOneLetterCode()=='I');
    assert(b.atoms[5].GetResidue().GetOneLetterCode()=='I');
                  #~ self.seq_b.GetResidue(j.GetResidue().GetIndex()).GetOneLetterCode());
 


if __name__ == "__main__":
  # test if python extension for seq module work
  unittest.main()
