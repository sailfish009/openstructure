import unittest
if __name__== '__main__':
  import sys
  sys.path.insert(0,"../../../stage/lib64/openstructure/")

import ost
import ost.gfx
import ost.geom

class TestPrimList(unittest.TestCase):
  def setUp(self):
    pass

  def test_(self):
    pl=ost.gfx.PrimList("foo")
    pl.AddPoint([0,0,0])
    pl.AddPoint(ost.geom.Vec3(1,2,3),col=ost.gfx.RED)
    pl.AddLine([0,0,0],[1,2,3])
    pl.AddLine(ost.geom.Vec3(0,0,0),ost.geom.Vec3(1,2,3),col=ost.gfx.BLUE)
    pl.AddSphere([0,0,0],rad=2.0)
    pl.AddSphere(ost.geom.Vec3(1,2,3),col=ost.gfx.RED,rad=3.0)
    pl.AddCyl([0,0,0],[1,2,3],rad=0.5,col=ost.gfx.YELLOW)
    pl.AddCyl(ost.geom.Vec3(0,0,0),ost.geom.Vec3(1,2,3),rad1=0.5,rad2=0.1,col1=ost.gfx.BLUE,col2=ost.gfx.GREEN)

if __name__== '__main__':
  unittest.main()

