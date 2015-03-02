#------------------------------------------------------------------------------
# This file is part of the OpenStructure project <www.openstructure.org>
#
# Copyright (C) 2008-2015 by the OpenStructure authors
#
# This library is free software; you can redistribute it and/or modify it under
# the terms of the GNU Lesser General Public License as published by the Free
# Software Foundation; either version 3.0 of the License, or (at your option)
# any later version.
# This library is distributed in the hope that it will be useful, but WITHOUT
# ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
# FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public License for more
# details.
#
# You should have received a copy of the GNU Lesser General Public License
# along with this library; if not, write to the Free Software Foundation, Inc.,
# 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
#------------------------------------------------------------------------------

import unittest
if __name__== '__main__':
  import sys
  sys.path.insert(0,"../../../stage/lib64/openstructure/")
  sys.path.insert(0,"../../../stage/lib/openstructure/")

import ost
import ost.geom as geom

class TestGeom(unittest.TestCase):
  def runTest(self):
    self.test_repr()
    self.test_data()

  def test_repr(self):
    v=geom.Vec2(1,2)
    v2=eval(repr(v))
    self.assertTrue(geom.Equal(v, v2))
                     
    v=geom.Vec3(1,2,3)
    v2=eval(repr(v))
    self.assertTrue(geom.Equal(v, v2))

    v=geom.Vec4(1,2,3,4)
    v2=eval(repr(v))
    self.assertTrue(geom.Equal(v, v2))

    m=geom.Mat2(1,2,3,4)
    m2=eval(repr(m))
    self.assertTrue(geom.Equal(m, m2))

    m=geom.Mat3(1,2,3,4,5,6,7,8,9)
    m2=eval(repr(m))
    self.assertTrue(geom.Equal(m, m2))

    m=geom.Mat4(1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16)
    m2=eval(repr(m))
    self.assertTrue(geom.Equal(m, m2))

  def test_data(self):
    self.assertEqual(geom.Vec2(1,2).data,[1,2])
    self.assertEqual(geom.Vec3(1,2,3).data,[1,2,3])
    self.assertEqual(geom.Vec4(1,2,3,4).data,[1,2,3,4])
    self.assertEqual(geom.Mat2(1,2,
                               3,4).data,
                     [1,2,
                      3,4])
    self.assertEqual(geom.Mat3(1,2,3,
                               4,5,6,
                               7,8,9).data,
                     [1,2,3,
                      4,5,6,
                      7,8,9])
    self.assertEqual(geom.Mat4(1,2,3,4,
                               5,6,7,8,
                               9,10,11,12,
                               13,14,15,16).data,
                     [1,2,3,4,
                      5,6,7,8,
                      9,10,11,12,
                      13,14,15,16])
    
if __name__== '__main__':
  from ost import testutils
  testutils.RunTests()

