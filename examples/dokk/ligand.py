from ost import gfx, geom
from ost.mol import alg
from ost import mol
class Ligand:
  def __init__(self, ligand):
    self.handle=ligand
    self.the_solution_=ligand.Copy().CreateFullView()
    self.go=gfx.Entity("Ligand", gfx.CUSTOM, self.handle)
    self.go.SetColor(gfx.GREEN, 'ele=C')
    gfx.Scene().Add(self.go)
    gfx.Scene().SetCenter(self.go.center)
    bbox=self.go.GetBoundingBox()
    self.radius=geom.Length(bbox.GetMax()-bbox.GetMin())+3.0
    self.pivot_=mol.AtomHandle()
  def SetPivot(self, pivot):
    self.pivot_=pivot
  def GetCenter(self):
    return self.handle.GetGeometricCenter()
  def RotateAxis(self, axis, angle):
    edi=self.handle.RequestXCSEditor()
    rot=geom.Mat4(geom.AxisRotation(axis, angle))
    trans=geom.Mat4()
    
    center=self.handle.GetGeometricCenter()
    if self.pivot_.IsValid():
      center=self.pivot_.GetPos()
    trans.PasteTranslation(-center)
    trans2=geom.Mat4()
    trans2.PasteTranslation(center)
    full_tf=trans2*rot*trans
    edi.ApplyTransform(full_tf)
    self.go.UpdatePositions()
  def Shift(self, vec):
    trans=geom.Mat4()
    trans.PasteTranslation(vec)
    edi=self.handle.RequestXCSEditor()    
    edi.ApplyTransform(trans)
    self.go.UpdatePositions()    

  def UpdateScores(self):
    for a in self.b.view.atoms:
      score=qa.ClashScore(a.handle, self.a.view)
      a.SetGenericFloatProperty('clash', score)
  def RMSDToSolution(self):
    return alg.CalculateRMSD(self.handle.CreateFullView(), 
                             self.the_solution_)