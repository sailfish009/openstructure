import sys,unittest
sys.path.append("../../../../stage/lib/openstructure")
sys.path.append("../../../../stage/lib64/openstructure")
from ost import *
import numpy

def v2v(v):
  return geom.Vec3(float(v[0]),float(v[1]),float(v[2]))

class TestNumpy(unittest.TestCase):
  def setUp(self):
    pass

  def test_(self):
    entity=mol.CreateEntity()
    ed=entity.EditXCS()
    ch=ed.InsertChain("X")
    re=ed.AppendResidue(ch,"ALA")
    a0=ed.InsertAtom(re,"A",geom.Vec3(0,0,0))
    self.assertTrue(a0.GetIndex()==0)
    a1=ed.InsertAtom(re,"B",geom.Vec3(1,0,0))
    self.assertTrue(a1.GetIndex()==1)
    a2=ed.InsertAtom(re,"C",geom.Vec3(2,0,0))
    self.assertTrue(a2.GetIndex()==2)
    a3=ed.InsertAtom(re,"D",geom.Vec3(3,0,0))
    self.assertTrue(a3.GetIndex()==3)

    self.assertTrue(geom.Distance(a0.pos,geom.Vec3(0,0,0))<1e-10)
    self.assertTrue(geom.Distance(a1.pos,geom.Vec3(1,0,0))<1e-10)
    self.assertTrue(geom.Distance(a2.pos,geom.Vec3(2,0,0))<1e-10)
    self.assertTrue(geom.Distance(a3.pos,geom.Vec3(3,0,0))<1e-10)

    ed.SetAtomTransformedPos(entity.GetAtomList(),numpy.array([[0,1,0],[0,2,0],[0,3,0],[0,4,0]], dtype=numpy.float32))

    self.assertTrue(geom.Distance(a0.pos,geom.Vec3(0,1,0))<1e-10)
    self.assertTrue(geom.Distance(a1.pos,geom.Vec3(0,2,0))<1e-10)
    self.assertTrue(geom.Distance(a2.pos,geom.Vec3(0,3,0))<1e-10)
    self.assertTrue(geom.Distance(a3.pos,geom.Vec3(0,4,0))<1e-10)

    na=entity.positions

    self.assertTrue(geom.Distance(v2v(na[0]),geom.Vec3(0,1,0))<1e-10)
    self.assertTrue(geom.Distance(v2v(na[1]),geom.Vec3(0,2,0))<1e-10)
    self.assertTrue(geom.Distance(v2v(na[2]),geom.Vec3(0,3,0))<1e-10)
    self.assertTrue(geom.Distance(v2v(na[3]),geom.Vec3(0,4,0))<1e-10)

if __name__== '__main__':
    unittest.main()

