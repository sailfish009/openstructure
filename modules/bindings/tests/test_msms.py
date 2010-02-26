import unittest
from ost import *
from ost import settings
from ost.bindings import msms

class TestMSMSBindings(unittest.TestCase):
  
  def setUp(self):
    self.protein = io.LoadEntity("testfiles/testprotein.pdb")
    self.num_tri=45360
    self.num_vert=22670
    self.ases=19336.730
    self.asas=21179.932

  def testCalculateSurface(self):
    surf=msms.CalculateSurface(self.protein, msms_env='MSMSSERVER')[0]
    assert self.num_vert==len(surf.GetVertexIDList()) \
       and self.num_tri==len(surf.GetTriIDList()), \
       "Number of surface vertices or triangles do not match precalculated values"
  
  def testCalculateSurfaceAres(self):
     (msms_ases, msms_asas)=msms.CalculateSurfaceArea(self.protein, \
                                                      msms_env='MSMSSERVER')
     assert self.ases==msms_ases[0] and self.asas==msms_asas[0], \
     "SASA or SESA do not match precalculated values"


if __name__ == "__main__":
  # test if msms package is available on system, otherwise ignore tests
  try:
    msms._GetExecutable(msms_exe=None, msms_env='MSMSSERVER')
  except(settings.FileNotFound):
    print "Could not find msms executable: ignoring unit tests"
    exit(0)
  unittest.main()
