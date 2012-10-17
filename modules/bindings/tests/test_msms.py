import unittest
from ost import *
from ost import settings
from ost.bindings import msms

class TestMSMSBindings(unittest.TestCase):
  
  def setUp(self):
    self.protein = io.LoadEntity("testfiles/testprotein.pdb")

    self.num_tri=111546
    self.num_vert=55762
    self.ases=20051.928
    self.asas=21574.324

  def testCalculateSurface(self):
    surf=msms.CalculateSurface(self.protein,
                               msms_env='MSMSSERVER',
                               density=3,
                               radius=1.4)[0]
    assert self.num_vert==len(surf.GetVertexIDList()) \
       and self.num_tri==len(surf.GetTriIDList()), \
       "Number of surface vertices (%i) or triangles (%i) do not match precalculated values (%i/%i)"%(len(surf.GetVertexIDList()),len(surf.GetTriIDList()),self.num_vert,self.num_tri)
  
  def testCalculateSurfaceAres(self):
     (msms_ases, msms_asas)=msms.CalculateSurfaceArea(self.protein, \
                                                      msms_env='MSMSSERVER',
                                                      density=3,
                                                      radius=1.4)
     assert self.ases==msms_ases[0] and self.asas==msms_asas[0], \
     "SASA (%f) or SESA (%f) do not match precalculated values (%f/%f)"%(msms_asas[0],msms_ases[0],self.asas,self.ases)


if __name__ == "__main__":
  # test if msms package is available on system, otherwise ignore tests
  VERSION_REQUIRED = '2.6.1'
  try:
    msms._GetExecutable(msms_exe=None, msms_env='MSMSSERVER')
  except(settings.FileNotFound):
    print "Could not find msms executable: ignoring unit tests"
    exit(0)
  version = msms.GetVersion(msms_exe=None, msms_env='MSMSSERVER')
  if version!=VERSION_REQUIRED:
    print "MSMS version (%s) does not match required version %s: ignoring unit tests"%(version, VERSION_REQUIRED)
  from ost import testutils
  testutils.RunTests()
