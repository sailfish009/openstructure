import unittest
from ost import *
from ost import settings
from ost import seq


def fixture():
  e=mol.CreateEntity()
  ede=e.RequestXCSEditor()
  chain=ede.InsertChain('A')
  for res in 'ABCDEFGH':
    r=ede.AppendResidue(chain, res)
    r.SetOneLetterCode(res)
    ede.InsertAtom(r, "XXX", geom.Vec3())
    
  return e
  
class TestSeq(unittest.TestCase):
  
  def setUp(self):
    self.ent=fixture()

  def testViewsFromSequences_01(self):
    seq_a=seq.CreateSequence("A", "ABCD-FGH")
    seq_a.AttachView(self.ent.Select('rname=A,B,C,D,F,G,H'))
    seq_b=seq.CreateSequence("B", "ABCDEFGH")
    seq_b.AttachView(self.ent.Select(''))
    a, b=seq.ViewsFromSequences(seq_a, seq_b)
    string_a=''.join([r.one_letter_code for r in a.residues])
    string_b=''.join([r.one_letter_code for r in b.residues])
    self.assertEqual(string_a, 'ABCDFGH')
    self.assertEqual(string_b, 'ABCDFGH')
 
  def testViewsFromSequences_02(self):
    seq_a=seq.CreateSequence("A", "ABCD-FGH")
    seq_a.AttachView(self.ent.Select('rname=A,B,C,D,F,G,H'))
    seq_b=seq.CreateSequence("B", "ABCD-FGH")
    seq_b.AttachView(self.ent.Select('rname=A,B,C,D,F,G,H'))
    a, b=seq.ViewsFromSequences(seq_a, seq_b)
    string_a=''.join([r.one_letter_code for r in a.residues])
    string_b=''.join([r.one_letter_code for r in b.residues])
    self.assertEqual(string_a, 'ABCDFGH')
    self.assertEqual(string_b, 'ABCDFGH')
    
  def testViewsFromSequences_03(self):
    seq_a=seq.CreateSequence("A", "ABCD--GH")
    seq_a.AttachView(self.ent.Select('rname=A,B,C,D,G,H'))
    seq_b=seq.CreateSequence("B", "ABCD-FGH")
    seq_b.AttachView(self.ent.Select('rname=A,B,C,D,F,G,H'))
    a, b=seq.ViewsFromSequences(seq_a, seq_b)
    string_a=''.join([r.one_letter_code for r in a.residues])
    string_b=''.join([r.one_letter_code for r in b.residues])
    self.assertEqual(string_a, 'ABCDGH')
    self.assertEqual(string_b, 'ABCDGH')
 
  def testViewsFromSequences_04(self):
    seq_a=seq.CreateSequence("A", "ABCD-FGH")
    seq_a.AttachView(self.ent.Select('rname=A,B,C,D,F,G,H'))
    seq_b=seq.CreateSequence("B", "ABCD--GH")
    seq_b.AttachView(self.ent.Select('rname=A,B,C,D,G,H'))
    a, b=seq.ViewsFromSequences(seq_a, seq_b)
    string_a=''.join([r.one_letter_code for r in a.residues])
    string_b=''.join([r.one_letter_code for r in b.residues])
    self.assertEqual(string_a, 'ABCDGH')
    self.assertEqual(string_b, 'ABCDGH')
    
  def testViewsFromSequences_05(self):
    seq_a=seq.CreateSequence("A", "ABCD-F--")
    seq_a.AttachView(self.ent.Select('rname=A,B,C,D,F'))
    seq_b=seq.CreateSequence("B", "ABCDEFGH")
    seq_b.AttachView(self.ent.Select('rname=A,B,C,D,E,F,G,H'))
    a, b=seq.ViewsFromSequences(seq_a, seq_b)
    string_a=''.join([r.one_letter_code for r in a.residues])
    string_b=''.join([r.one_letter_code for r in b.residues])
    self.assertEqual(string_a, 'ABCDF')
    self.assertEqual(string_b, 'ABCDF')
    
  def testViewsFromSequences_06(self):
    seq_a=seq.CreateSequence("A", "--CD-FGH")
    seq_a.AttachView(self.ent.Select('rname=C,D,F,G,H'))
    seq_b=seq.CreateSequence("B", "ABCDEFGH")
    seq_b.AttachView(self.ent.Select('rname=A,B,C,D,E,F,G,H'))
    a, b=seq.ViewsFromSequences(seq_a, seq_b)
    string_a=''.join([r.one_letter_code for r in a.residues])
    string_b=''.join([r.one_letter_code for r in b.residues])
    self.assertEqual(string_a, 'CDFGH')
    self.assertEqual(string_b, 'CDFGH')


  def testViewsFromSequences_07(self):
    seq_a=seq.CreateSequence("A", "AB-D-FGH")
    seq_a.AttachView(self.ent.Select('rname=A,B,D,F,G,H'))
    seq_b=seq.CreateSequence("B", "AB-DEF-H")
    seq_b.AttachView(self.ent.Select('rname=A,B,D,E,F,H'))
    a, b=seq.ViewsFromSequences(seq_a, seq_b)
    string_a=''.join([r.one_letter_code for r in a.residues])
    string_b=''.join([r.one_letter_code for r in b.residues])
    self.assertEqual(string_a, 'ABDFH')
    self.assertEqual(string_b, 'ABDFH')


  def testViewsFromSequences_08(self):
    seq_a=seq.CreateSequence("A", "A-C-E-G")
    seq_a.AttachView(self.ent.Select('rname=A,C,E,G'))
    seq_b=seq.CreateSequence("B", "-B-D-H-")
    seq_b.AttachView(self.ent.Select('rname=B,D,H'))
    a, b=seq.ViewsFromSequences(seq_a, seq_b)
    string_a=''.join([r.one_letter_code for r in a.residues])
    string_b=''.join([r.one_letter_code for r in b.residues])
    self.assertEqual(string_a, '')
    self.assertEqual(string_b, '')

  def testViewsFromSequences_09(self):
    seq_a=seq.CreateSequence("A", "B-D-FGH")
    seq_a.AttachView(self.ent.Select('rname=A,B,D,F,G,H'))
    seq_a.offset=1
    seq_b=seq.CreateSequence("B", "B-DEF-H")
    seq_b.offset=1
    seq_b.AttachView(self.ent.Select('rname=A,B,D,E,F,H'))
    a, b=seq.ViewsFromSequences(seq_a, seq_b)
    string_a=''.join([r.one_letter_code for r in a.residues])
    string_b=''.join([r.one_letter_code for r in b.residues])
    self.assertEqual(string_a, 'BDFH')
    self.assertEqual(string_b, 'BDFH')
suite = unittest.TestLoader().loadTestsFromTestCase(TestSeq)
unittest.TextTestRunner().run(suite)

