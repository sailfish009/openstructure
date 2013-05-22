import unittest
from ost import conop


class TestNonStandard(unittest.TestCase):
 
  def test_fastModified(self):
    # phoshoserine: test if we correctly strip off modifications
    tpl=io.LoadPDB('testfiles/sep.pdb')
    new_hdl=mol.CreateEntity();
    ed=new_hdl.EditXCS()
    c=ed.InsertChain('A')
    ed.AppendResidue(c, 'SER')

    err, has_cbeta=conop.CopyConserved(tpl.residues[0], new_hdl.residues[0], ed)
    self.assertTrue(err)
    self.assertTrue(has_cbeta)
    residues=new_hdl.residues
    self.assertEqual(len(residues), 1)
    self.assertEqual(len(residues[0].atoms), 6)
    self.assertTrue(new_hdl.FindAtom("A", mol.ResNum(1), "N").IsValid())
    self.assertTrue(new_hdl.FindAtom("A", mol.ResNum(1), "CA").IsValid())
    self.assertTrue(new_hdl.FindAtom("A", mol.ResNum(1), "C").IsValid())
    self.assertTrue(new_hdl.FindAtom("A", mol.ResNum(1), "O").IsValid())
    self.assertTrue(new_hdl.FindAtom("A", mol.ResNum(1), "CB").IsValid())
    self.assertTrue(new_hdl.FindAtom("A", mol.ResNum(1), "OG").IsValid())
    
    
  def test_CBeta(self):
    # test if the dst residues contain cbeta, unless they are glycines
    tpl=io.LoadPDB('testfiles/cbeta.pdb')
    new_hdl=mol.CreateEntity();
    ed=new_hdl.EditXCS()
    c=ed.InsertChain('A')
    ed.AppendResidue(c, 'MET')
    ed.AppendResidue(c, 'GLY')
    ed.AppendResidue(c, 'GLY')
    ed.AppendResidue(c, 'HIS')
    err, has_cbeta=conop.CopyConserved(tpl.residues[0], new_hdl.residues[0], ed)
    self.assertTrue(has_cbeta)
    self.assertTrue(err)
    err, has_cbeta=conop.CopyConserved(tpl.residues[1], new_hdl.residues[1], ed)
    self.assertFalse(has_cbeta)
    self.assertTrue(err)
    err, has_cbeta=conop.CopyConserved(tpl.residues[2], new_hdl.residues[2], ed)
    self.assertFalse(has_cbeta)
    self.assertTrue(err)
    err, has_cbeta=conop.CopyConserved(tpl.residues[3], new_hdl.residues[3], ed)
    self.assertTrue(has_cbeta)
    self.assertTrue(err)
      
    residues=new_hdl.residues
    self.assertEqual(len(residues), 4)
    self.assertTrue(residues[0].FindAtom("CB").IsValid())
    self.assertFalse(residues[1].FindAtom("CB").IsValid())
    self.assertFalse(residues[2].FindAtom("CB").IsValid())
    self.assertTrue(residues[3].FindAtom("CB").IsValid())


  def test_CopyResidue(self):
    tpl=io.LoadPDB('testfiles/cbeta.pdb')
    new_hdl=mol.CreateEntity();
    ed=new_hdl.EditXCS()
    c=ed.InsertChain('A')
    ed.AppendResidue(c, 'MET')
    ed.AppendResidue(c, 'GLY')
    ed.AppendResidue(c, 'GLY')
    ed.AppendResidue(c, 'HIS')
    ed.AppendResidue(c, 'HIS')
    ed.AppendResidue(c, 'GLY')
    ed.AppendResidue(c, 'HIS')
    ed.AppendResidue(c, 'MET')
    
    # MET to MET
    err =conop.CopyResidue(tpl.residues[0], new_hdl.residues[0], ed)
    self.assertTrue(err)
    #GLY to GLY
    err =conop.CopyResidue(tpl.residues[1], new_hdl.residues[1], ed)
    self.assertTrue(err)
    # GLY to GLY
    err =conop.CopyResidue(tpl.residues[2], new_hdl.residues[2], ed)
    self.assertTrue(err)
    #now we copy a HIS to a HIS
    err =conop.CopyResidue(tpl.residues[3], new_hdl.residues[3], ed)
    self.assertTrue(err)
    # copy a GLY to a HIS
    err, has_cbeta=conop.CopyNonConserved(tpl.residues[1], new_hdl.residues[4], ed)
    self.assertFalse(has_cbeta)
    # copy a MET to a GLY 
    err =conop.CopyResidue(tpl.residues[0], new_hdl.residues[5], ed)
    self.assertFalse(err)
    # copy a MET to a HIS 
    err =conop.CopyResidue(tpl.residues[0], new_hdl.residues[6], ed)
    self.assertFalse(err)
    # copy a GLY to a MET with adding CB
    err=conop.CopyResidue(tpl.residues[1], new_hdl.residues[7], ed)
    self.assertFalse(err)
      
    residues=new_hdl.residues
    self.assertEqual(len(residues), 8)
    # MET to MET
    self.assertTrue(residues[0].FindAtom("CB").IsValid())
    #GLY to GLY
    self.assertFalse(residues[1].FindAtom("CB").IsValid())
    #now we copy a GLY to a GLY
    self.assertFalse(residues[2].FindAtom("CB").IsValid())
    #now we copy a HIS to a HIS
    self.assertTrue(residues[3].FindAtom("CB").IsValid())
    #now we copy a GLY to a HIS without adding CB
    self.assertFalse(residues[4].FindAtom("CB").IsValid())
    #now we copy a MET to a GLY
    self.assertFalse(residues[5].FindAtom("CB").IsValid())
    # copy a MET to a HIS
    self.assertTrue(residues[6].FindAtom("CB").IsValid())
    # copy a GLY to a MET with adding CB
    self.assertTrue(residues[7].FindAtom("CB").IsValid())
    

if __name__ == "__main__":
  if not conop.GetDefaultLib():
    print 'No compound library available. Ignoring unit tests'
  else:
    from ost import testutils
    testutils.RunTests()


