import unittest
from ost import *
from ost import settings
from ost import seq


def fixture(seq_str='MRLDGKTALITGSAR'):
  e=mol.CreateEntity()
  ede=e.RequestXCSEditor()
  chain=ede.InsertChain('A')
  res_str='MET ARG LEU ASP GLY LYS THR ALA LEU ILE THR GLY SER ALA ARG'
  res=res_str.split()
  olc={}
  counter=0
  for i in 'MRLDGKTALITGSAR':
    olc[i]=res[counter]
    counter+=1
  
  for i in seq_str:
    if i!='-':
      r=ede.AppendResidue(chain,olc[i])
      r.SetOneLetterCode(i)
      a=ede.InsertAtom(r,'CA',geom.Vec3())
  #~ for i in e.atoms: print i,i.index
  return e.Copy()
  
class TestSeq(unittest.TestCase):
  
  def setUp(self):
    #sequence and entity must correspond
    print 'initialising'
    ent=fixture()
    self.protein=ent.Select('')
    c=self.protein.chains[0]
    self.seq_a=seq.SequenceFromChain('0',c) # MRLDGKTALITGSAR
    self.seq_a.AttachView(self.protein)
    self.seq_b=seq.CreateSequence('1','---DGKTALITGSAR')
    self.seq_b.AttachView(fixture('---DGKTALITGSAR').Select(''))
    #~ self.seq_c=seq.CreateSequence('2','MRLDG---LITGSAR')
    #~ self.seq_c.AttachView(self.protein)
    #~ self.seq_d=seq.CreateSequence('3','MRLDG----ITGSAR')
    #~ self.seq_d.AttachView(self.protein)
    self.seq_e=seq.CreateSequence('4','AMRLDG----ITGSA')
    self.seq_e.AttachView(fixture('AMRLDG----ITGSA').Select(''))
    ##self.seq_e.AttachView(self.protein)
    self.seq_e.SetSequenceOffset(1)
    self.seq_f=seq.CreateSequence('5','MRLDGKTA-ITGSAR')
    self.seq_f.AttachView(fixture('MRLDGKTA-ITGSAR').Select(''))
    self.seq_g=seq.CreateSequence('6','MRLDGKTALITG---')
    self.seq_g.AttachView(fixture('MRLDGKTALITG---').Select(''))
    
#~ MRLDGKTALITGSAR
#~ ---DGKTALITGSAR
  def testViewsFromSequences1(self):
    
    [a,b]=seq.ViewsFromSequences(self.seq_a,
                                 self.seq_b,
                                 self.seq_a.GetAttachedView(),
                                 self.seq_b.GetAttachedView())

    #~ for i,j in zip(a.atoms,b.atoms):
      #~ print i,i.residue.one_letter_code,j,j.residue.one_letter_code
    
    self.assertEqual(a.atoms[0].GetResidue().GetOneLetterCode(),'D');
    self.assertEqual(b.atoms[0].GetResidue().GetOneLetterCode(),'D');
    self.assertEqual(a.atoms[11].GetResidue().GetOneLetterCode(),'R');
    self.assertEqual(b.atoms[11].GetResidue().GetOneLetterCode(),'R');

#~ ---DGKTALITGSAR
#~ MRLDGKTALITGSAR
  def testViewsFromSequences2(self):
    [a,b]=seq.ViewsFromSequences(self.seq_b,
                                 self.seq_a,
                                 self.seq_b.GetAttachedView(),
                                 self.seq_a.GetAttachedView())

    self.assertEqual(a.atoms[0].GetResidue().GetOneLetterCode(),'D');
    self.assertEqual(b.atoms[0].GetResidue().GetOneLetterCode(),'D');
    self.assertEqual(a.atoms[11].GetResidue().GetOneLetterCode(),'R');
    self.assertEqual(b.atoms[11].GetResidue().GetOneLetterCode(),'R');


##~ MRLDGKTALITGSAR
##~ MRLDGKTALITG---
  def testViewsFromSequences3(self):
    [a,b]=seq.ViewsFromSequences(self.seq_a,
                                 self.seq_g,
                                 self.seq_a.GetAttachedView(),
                                 self.seq_g.GetAttachedView())

    self.assertEqual(a.atoms[0].GetResidue().GetOneLetterCode(),'M');
    self.assertEqual(b.atoms[0].GetResidue().GetOneLetterCode(),'M');
    self.assertEqual(a.atoms[-1].GetResidue().GetOneLetterCode(),'G');
    self.assertEqual(b.atoms[-1].GetResidue().GetOneLetterCode(),'G');
    

##~ MRLDGKTALITG---
##~ MRLDGKTALITGSAR
  def testViewsFromSequences4(self):
    [a,b]=seq.ViewsFromSequences(self.seq_g,
                                 self.seq_a,
                                 self.seq_g.GetAttachedView(),
                                 self.seq_a.GetAttachedView())

    self.assertEqual(a.atoms[0].GetResidue().GetOneLetterCode(),'M');
    self.assertEqual(b.atoms[0].GetResidue().GetOneLetterCode(),'M');
    self.assertEqual(a.atoms[-1].GetResidue().GetOneLetterCode(),'G');
    self.assertEqual(b.atoms[-1].GetResidue().GetOneLetterCode(),'G');



##~ MRLDGKTALITGSAR
##~ MRLDGKTA-ITGSAR
  def testViewsFromSequences5(self):
    [a,b]=seq.ViewsFromSequences(self.seq_a,
                                 self.seq_f,
                                 self.seq_a.GetAttachedView(),
                                 self.seq_f.GetAttachedView())
    print 'result'
    for i,j in zip(a.residues,b.residues): print i,i.index,j,j.index
                                 
    self.assertEqual(a.atoms[0].GetResidue().GetOneLetterCode(),'M');
    self.assertEqual(b.atoms[0].GetResidue().GetOneLetterCode(),'M');
    #~ for i,j in zip(a.atoms,b.atoms): print i,j
    self.assertEqual(a.atoms[8].GetResidue().GetOneLetterCode(),'I');
    self.assertEqual(b.atoms[8].GetResidue().GetOneLetterCode(),'I');

##~ MRLDGKTA-ITGSAR
##~ MRLDGKTALITGSAR
  #def testViewsFromSequences6(self):
    #[a,b]=seq.ViewsFromSequences(self.seq_f,
                                 #self.seq_a,
                                 #self.seq_f.GetAttachedView(),
                                 #self.seq_a.GetAttachedView())

    #self.assertEqual(a.atoms[0].GetResidue().GetOneLetterCode(),'M');
    #self.assertEqual(b.atoms[0].GetResidue().GetOneLetterCode(),'M');
    #self.assertEqual(a.atoms[8].GetResidue().GetOneLetterCode(),'I');
    #self.assertEqual(b.atoms[8].GetResidue().GetOneLetterCode(),'I');
    
##~ MRLDG---LITGSAR
##~ MRLDG----ITGSAR
  #~ def testViewsFromSequences7(self):
    #~ [a,b]=seq.ViewsFromSequences(self.seq_c,
                                 #~ self.seq_d,
                                 #~ self.seq_c.GetAttachedView(),
                                 #~ self.seq_d.GetAttachedView())
#~ 
    #~ self.assertEqual(a.atoms[0].GetResidue().GetOneLetterCode(),'M');
    #~ self.assertEqual(b.atoms[0].GetResidue().GetOneLetterCode(),'M');
    #~ self.assertEqual(a.atoms[5].GetResidue().GetOneLetterCode(),'I');
    #~ self.assertEqual(b.atoms[5].GetResidue().GetOneLetterCode(),'I');
                  
#~ MRLDGKTALITGSAR
#~ AMRLDG----ITGSA
  def testViewsFromSequences8(self):
    [a,b]=seq.ViewsFromSequences(self.seq_a,
                                 self.seq_e,
                                 self.seq_a.GetAttachedView(),
                                 self.seq_e.GetAttachedView())
    print 'result'
    for i,j in zip(a.residues,b.residues): print i,i.index,j,j.index
    self.assertEqual(a.atoms[0].GetResidue().GetOneLetterCode(),'M');
    self.assertEqual(b.atoms[0].GetResidue().GetOneLetterCode(),'M');
    self.assertEqual(a.atoms[6].GetResidue().GetOneLetterCode(),'I');
    self.assertEqual(b.atoms[6].GetResidue().GetOneLetterCode(),'I');
               
 


if __name__ == "__main__":
  # test if python extension for seq module work
  suite = unittest.TestLoader().loadTestsFromTestCase(TestSeq)
  unittest.TextTestRunner(verbosity=3).run(suite)

  #~ unittest.main()

