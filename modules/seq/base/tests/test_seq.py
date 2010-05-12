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
    self.seq_f=seq.CreateSequence('5','MRLDGKTA-ITGSAR')
    self.seq_f.AttachView(self.protein.Select('aname==CA'))
    self.seq_g=seq.CreateSequence('6','MRLDGKTALITG---')
    self.seq_g.AttachView(self.protein.Select('aname==CA'))
    
  def testViewsFromSequencesGapFrontSeqB(self):
    #~ MRLDGKTALITGSAR
    #~ ---DGKTALITGSAR
    [a,b]=seq.ViewsFromSequences(self.seq_a,
                                 self.seq_b,
                                 self.seq_a.GetAttachedView(),
                                 self.seq_b.GetAttachedView())
    self.assertEqual(a.atoms[0].GetResidue().GetOneLetterCode(),'D');
    self.assertEqual(b.atoms[0].GetResidue().GetOneLetterCode(),'D');

  def testViewsFromSequencesGapFrontSeqA(self):
    #~ ---DGKTALITGSAR
    #~ MRLDGKTALITGSAR
    [a,b]=seq.ViewsFromSequences(self.seq_b,
                                 self.seq_a,
                                 self.seq_b.GetAttachedView(),
                                 self.seq_a.GetAttachedView())
    for i,j in zip(a.atoms,b.atoms): print i,j
    self.assertEqual(a.atoms[0].GetResidue().GetOneLetterCode(),'D');
    self.assertEqual(b.atoms[0].GetResidue().GetOneLetterCode(),'D');


  def testViewsFromSequencesGapRearSeqB(self):
    #~ MRLDGKTALITGSAR
    #~ ---DGKTALITGSAR
    [a,b]=seq.ViewsFromSequences(self.seq_a,
                                 self.seq_b,
                                 self.seq_a.GetAttachedView(),
                                 self.seq_b.GetAttachedView())

    self.assertEqual(a.atoms[0].GetResidue().GetOneLetterCode(),'D');
    self.assertEqual(b.atoms[0].GetResidue().GetOneLetterCode(),'D');

  def testViewsFromSequencesGapRearSeqA(self):
    #~ ---DGKTALITGSAR
    #~ MRLDGKTALITGSAR
    [a,b]=seq.ViewsFromSequences(self.seq_b,
                                 self.seq_a,
                                 self.seq_b.GetAttachedView(),
                                 self.seq_a.GetAttachedView())
    #~ for i,j in zip(a.atoms,b.atoms): print i,j
    self.assertEqual(a.atoms[0].GetResidue().GetOneLetterCode(),'D');
    self.assertEqual(b.atoms[0].GetResidue().GetOneLetterCode(),'D');



  def testViewsFromSequencesSingleGapMiddle1(self):
    #~ MRLDGKTALITGSAR
    #~ MRLDGKTA-ITGSAR
    [a,b]=seq.ViewsFromSequences(self.seq_a,
                                 self.seq_f,
                                 self.seq_a.GetAttachedView(),
                                 self.seq_f.GetAttachedView())
    self.assertEqual(a.atoms[0].GetResidue().GetOneLetterCode(),'M');
    self.assertEqual(b.atoms[0].GetResidue().GetOneLetterCode(),'M');
    self.assertEqual(a.atoms[8].GetResidue().GetOneLetterCode(),'I');
    self.assertEqual(b.atoms[8].GetResidue().GetOneLetterCode(),'I');

  def testViewsFromSequencesSingleGapMiddle2(self):
    #~ MRLDGKTA-ITGSAR
    #~ MRLDGKTALITGSAR
    [a,b]=seq.ViewsFromSequences(self.seq_f,
                                 self.seq_a,
                                 self.seq_f.GetAttachedView(),
                                 self.seq_a.GetAttachedView())

    self.assertEqual(a.atoms[0].GetResidue().GetOneLetterCode(),'M');
    self.assertEqual(b.atoms[0].GetResidue().GetOneLetterCode(),'M');
    self.assertEqual(a.atoms[8].GetResidue().GetOneLetterCode(),'I');
    self.assertEqual(b.atoms[8].GetResidue().GetOneLetterCode(),'I');
  #def testViewsFromSequencesGapMiddleBoth(self):
    ##~ MRLDG---LITGSAR
    ##~ MRLDG----ITGSAR
    #[a,b]=seq.ViewsFromSequences(self.seq_c,
                                 #self.seq_d,
                                 #self.seq_c.GetAttachedView(),
                                 #self.seq_d.GetAttachedView())

    #print 'aaa'+a.atoms[4].name+'aaaa'
    #assertEqual(a.atoms[0].GetResidue().GetOneLetterCode()=='M');
    #assertEqual(b.atoms[0].GetResidue().GetOneLetterCode()=='M');
    #assertEqual(a.atoms[4].GetResidue().GetOneLetterCode()=='I');
    #assertEqual(b.atoms[4].GetResidue().GetOneLetterCode()=='I');
                  #~ self.seq_b.GetResidue(j.GetResidue().GetIndex()).GetOneLetterCode());
                  
  #def testViewsFromSequencesGapSeqOffset(self):
    ##~ MRLDGKTALITGSAR
    ##~ AMRLDG----ITGSA
    #[a,b]=seq.ViewsFromSequences(self.seq_a,
                                 #self.seq_e,
                                 #self.seq_a.GetAttachedView(),
                                 #self.seq_e.GetAttachedView())


    #assertEqual(a.atoms[0].GetResidue().GetOneLetterCode()=='M');
    #assertEqual(b.atoms[0].GetResidue().GetOneLetterCode()=='M');
    #assertEqual(a.atoms[5].GetResidue().GetOneLetterCode()=='I');
    #assertEqual(b.atoms[5].GetResidue().GetOneLetterCode()=='I');
                  ##~ self.seq_b.GetResidue(j.GetResidue().GetIndex()).GetOneLetterCode());
 


if __name__ == "__main__":
  # test if python extension for seq module work
  suite = unittest.TestLoader().loadTestsFromTestCase(TestSeq)
  unittest.TextTestRunner(verbosity=2).run(suite)

  #~ unittest.main()

