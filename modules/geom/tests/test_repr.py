import unittest
from ost import geom

class TestRepr(unittest.TestCase):
  def testReprVec2(self):
    v=geom.Vec2(1,2)
    v2=eval(repr(v))
    assert geom.Equal(v, v2)
    
  def testReprVec3(self):
    v=geom.Vec3(1,2,3)
    v2=eval(repr(v))
    assert geom.Equal(v, v2)

  def testReprVec4(self):
    v=geom.Vec4(1,2,3,4)
    v2=eval(repr(v))
    assert geom.Equal(v, v2)

  def testReprMat2(self):
    m=geom.Mat2(1,2,3,4)
    m2=eval(repr(m))
    assert geom.Equal(m, m2)
    
  def testReprMat3(self):
    m=geom.Mat3(1,2,3,4,5,6,7,8,9)
    m2=eval(repr(m))
    assert geom.Equal(m, m2)

  def testReprMat4(self):
    m=geom.Mat4(1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16)
    m2=eval(repr(m))
    assert geom.Equal(m, m2)  

if __name__ == "__main__":
  try:
    unittest.main()
  except Exception, e:
    print e