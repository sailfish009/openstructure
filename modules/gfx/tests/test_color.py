import unittest
if __name__== '__main__':
  import sys
  sys.path.insert(0,"../../../stage/lib64/openstructure/")

import ost
#import ost.gfx
#import ost.geom

class TestColor(unittest.TestCase):
  def setUp(self):
    pass

  def test_(self):
    c=ost.gfx.Color(0.5,0.3,0.2)
    self.assertAlmostEqual(c.r,0.5)
    self.assertAlmostEqual(c.g,0.3)
    self.assertAlmostEqual(c.b,0.2)
    self.assertAlmostEqual(c.a,1.0)
    self.assertAlmostEqual(c.red,0.5)
    self.assertAlmostEqual(c.green,0.3)
    self.assertAlmostEqual(c.blue,0.2)
    self.assertAlmostEqual(c.alpha,1.0)
    self.assertAlmostEqual(c[0],0.5)
    self.assertAlmostEqual(c[1],0.3)
    self.assertAlmostEqual(c[2],0.2)
    self.assertAlmostEqual(c[3],1.0)
    c.r=0.9
    self.assertAlmostEqual(c.r,0.9)
    self.assertAlmostEqual(c.red,0.9)
    self.assertAlmostEqual(c[0],0.9)

if __name__== '__main__':
  unittest.main()

