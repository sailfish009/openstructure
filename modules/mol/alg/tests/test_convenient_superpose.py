import unittest
import os
import random

class TestConvenientSuperpose(unittest.TestCase):
  
  def setUp(self):
    pass
  
  def runAtomOrdering(self, view1, view2):
    # call atom ordering function here
    view1, view2 = ost.mol.alg.MatchResidueByNum(view1, view2)
    return view1, view2
  
  def assertEqualAtomOrder(self, view1, view2):
    self.assertEquals(len(view1.atoms),len(view2.atoms))
    for a1, a2 in zip(view1.atoms, view2.atoms):
      self.assertEquals(a1.element, a2.element)
      self.assertTrue(geom.Equal(a1.pos, a2.pos))

  def testAssertion(self):
    ent1_ent = io.LoadEntity(os.path.join("testfiles","1aho.pdb"))
    ent1_view = ent1_ent.Select("")
    self.assertEqualAtomOrder(ent1_ent, ent1_ent)
    self.assertEqualAtomOrder(ent1_ent, ent1_view)
    self.assertEqualAtomOrder(ent1_view, ent1_ent)
    self.assertEqualAtomOrder(ent1_view, ent1_view)
    self.assertRaises(AssertionError, self.assertEqualAtomOrder, ent1_view.Select("ele!=H"), ent1_view)
    
  def testCorrectlyOrdered(self):
    ent1_ent = io.LoadEntity(os.path.join("testfiles","1aho.pdb"))
    ent1_view = ent1_ent.Select("")
    view1, view2 = self.runAtomOrdering(ent1_ent, ent1_ent)
    self.assertEqualAtomOrder(view1, view2)    
    view1, view2 = self.runAtomOrdering(ent1_view, ent1_ent)
    self.assertEqualAtomOrder(view1, view2)
    view1, view2 = self.runAtomOrdering(ent1_ent, ent1_view)
    self.assertEqualAtomOrder(view1, view2)
    view1, view2 = self.runAtomOrdering(ent1_view, ent1_view)
    self.assertEqualAtomOrder(view1, view2)
    
  def testMissingFirstAtom(self):
    ent_view = io.LoadEntity(os.path.join("testfiles","1aho.pdb")).Select("")
    ent_view_missing = ent_view.Select("not (cname=A and rnum=1 and aname=N)")
    view1, view2 = self.runAtomOrdering(ent_view, ent_view_missing)
    self.assertEqualAtomOrder(view1, view2)
    view1, view2 = self.runAtomOrdering(ent_view_missing, ent_view)
    self.assertEqualAtomOrder(view1, view2)
    
  def testMissingManyAtoms(self):
    ent_view = io.LoadEntity(os.path.join("testfiles","1aho.pdb")).Select("")
    ent_view_missing = ent_view.Select("not (cname=A and rnum=3,19,32 and aname=CB,CA,CD)")
    view1, view2 = self.runAtomOrdering(ent_view, ent_view_missing)
    self.assertEqualAtomOrder(view1, view2)
    view1, view2 = self.runAtomOrdering(ent_view_missing, ent_view)
    self.assertEqualAtomOrder(view1, view2)
    
  def testMissingFirstResidue(self):
    ent_view = io.LoadEntity(os.path.join("testfiles","1aho.pdb")).Select("")
    ent_view_missing = ent_view.Select("not (cname=A and rnum=1)")
    view1, view2 = self.runAtomOrdering(ent_view, ent_view_missing)
    self.assertEqualAtomOrder(view1, view2)
    view1, view2 = self.runAtomOrdering(ent_view_missing, ent_view)
    self.assertEqualAtomOrder(view1, view2)
  
  def testMissingHydrogens(self):
    ent_view = io.LoadEntity(os.path.join("testfiles","1aho.pdb")).Select("")
    ent_view_missing = ent_view.Select("ele!=H")
    view1, view2 = self.runAtomOrdering(ent_view, ent_view_missing)
    self.assertEqualAtomOrder(view1, view2)
    view1, view2 = self.runAtomOrdering(ent_view_missing, ent_view)
    self.assertEqualAtomOrder(view1, view2)
  
  def testWrongAtomOrder(self):
    ent_view = io.LoadEntity(os.path.join("testfiles","1aho.pdb")).Select("")
    ent_view_wrong = ent_view.CreateEmptyView()
    for c in ent_view.chains:
      ent_view_wrong.AddChain(c)
      for r in c.residues:
        ent_view_wrong.AddResidue(r)
        atoms = list(r.atoms)
        random.shuffle(atoms)
        for a in atoms:
          ent_view_wrong.AddAtom(a)
    view1, view2 = self.runAtomOrdering(ent_view, ent_view_wrong)
    self.assertEqualAtomOrder(view1, view2)
    view1, view2 = self.runAtomOrdering(ent_view_wrong, ent_view)
    self.assertEqualAtomOrder(view1, view2)
  
  def testWrongResidueOrder(self): 
    ent_view = io.LoadEntity(os.path.join("testfiles","1aho.pdb")).Select("")
    ent_view_wrong = ent_view.CreateEmptyView()
    for c in ent_view.chains:
      ent_view_wrong.AddChain(c)
      residues = list(c.residues)
      random.shuffle(residues)
      for r in residues:
        ent_view_wrong.AddResidue(r)
        for a in r.atoms:
          av=ent_view_wrong.AddAtom(a)
    view1, view2 = self.runAtomOrdering(ent_view, ent_view_wrong)
    self.assertEqualAtomOrder(view1, view2)
    view1, view2 = self.runAtomOrdering(ent_view_wrong, ent_view)
    self.assertEqualAtomOrder(view1, view2)
    
if __name__ == "__main__":
  unittest.main()
