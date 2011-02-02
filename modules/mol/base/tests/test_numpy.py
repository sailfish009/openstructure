import sys,unittest
sys.path.append("../../../../stage/lib/openstructure")
sys.path.append("../../../../stage/lib64/openstructure")
from ost import *
import numpy

class TestNumpy(unittest.TestCase):
  def setUp(self):
    pass

  def test_(self):
    entity=mol.CreateEntity()
    ed=entity.EditXCS()
    ch=ed.InsertChain("X")
    re=ed.AppendResidue(ch,"ALA")
    a0=ed.InsertAtom(re,"A",geom.Vec3(0,0,0))
    a1=ed.InsertAtom(re,"B",geom.Vec3(1,0,0))
    a2=ed.InsertAtom(re,"C",geom.Vec3(2,0,0))
    a3=ed.InsertAtom(re,"D",geom.Vec3(3,0,0))

    self.assertTrue(geom.Distance(a0.pos,geom.Vec3(0,0,0))<1e-10)
    self.assertTrue(geom.Distance(a1.pos,geom.Vec3(1,0,0))<1e-10)
    self.assertTrue(geom.Distance(a2.pos,geom.Vec3(2,0,0))<1e-10)
    self.assertTrue(geom.Distance(a3.pos,geom.Vec3(3,0,0))<1e-10)

    entity.positions=numpy.array([[0,1,0],[0,2,0],[0,3,0],[0,4,0]])

    self.assertTrue(geom.Distance(a0.pos,geom.Vec3(0,1,0))<1e-10)
    self.assertTrue(geom.Distance(a1.pos,geom.Vec3(0,2,0))<1e-10)
    self.assertTrue(geom.Distance(a2.pos,geom.Vec3(0,3,0))<1e-10)
    self.assertTrue(geom.Distance(a3.pos,geom.Vec3(0,4,0))<1e-10)

    na=entity.positions

    self.assertTrue(geom.Distance(geom.Vec3(na[0][0],na[0][1],na[0][2]),geom.Vec3(0,1,0))<1e-10)
    self.assertTrue(geom.Distance(geom.Vec3(na[1][0],na[1][1],na[1][2]),geom.Vec3(0,2,0))<1e-10)
    self.assertTrue(geom.Distance(geom.Vec3(na[2][0],na[2][1],na[2][2]),geom.Vec3(0,3,0))<1e-10)
    self.assertTrue(geom.Distance(geom.Vec3(na[3][0],na[3][1],na[3][2]),geom.Vec3(0,4,0))<1e-10)

if __name__== '__main__':
    unittest.main()

