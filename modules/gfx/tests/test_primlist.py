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
    pl.AddPoint(ost.geom.Vec3(1,2,3),color=ost.gfx.RED)
    pl.AddLine([0,0,0],[1,2,3])
    pl.AddLine(ost.geom.Vec3(0,0,0),ost.geom.Vec3(1,2,3),color=ost.gfx.BLUE)
    pl.AddSphere([0,0,0],radius=2.0)
    pl.AddSphere(ost.geom.Vec3(1,2,3),color=ost.gfx.RED,radius=3.0)
    pl.AddCyl([0,0,0],[1,2,3],radius=0.5,color=ost.gfx.YELLOW)
    pl.AddCyl(ost.geom.Vec3(0,0,0),ost.geom.Vec3(1,2,3),radius1=0.5,radius2=0.1,color1=ost.gfx.BLUE,color2=ost.gfx.GREEN)
    pl.AddText("foo",[0,2,3])
    pl.AddText("bar",[-2,0,0],color=gfx.WHITE,point_size=8)

if __name__== '__main__':
  unittest.main()

