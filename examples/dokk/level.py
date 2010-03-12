import os
from ost import io, gfx, qa, geom
from ligand import Ligand
from surface import Surface
from protein import Protein

class Level:
  def __init__(self, name):
    self.name_=name
    self.Load()
  def Load(self):
    print 'Loading %s' % self.name_
    level_dir=os.path.join('datafiles', self.name_)
    ligand_ent=io.LoadSDF(os.path.join(level_dir, 'ligand.sdf'))
    self.ligand=Ligand(ligand_ent)
    protein_ent=io.LoadPDB(os.path.join(level_dir, 'protein.pdb'))
    self.protein=Protein(protein_ent)
    surface=io.LoadSurface(os.path.join(level_dir, 'surface'), 'msms')
    self.surface=Surface(surface)
    self.surface.handle.Attach(self.protein.handle, 5.0)
    gfx.Scene().SetCenter(self.ligand.GetCenter())
    print 'Done Loading'
    self.UpdateScores()
  def RotateAxis(self, axis, angle):
    self.ligand.RotateAxis(axis, angle)
  def SetPivot(self, x, y):
    v1=gfx.Scene().UnProject(geom.Vec3(x, y, 0.0));
    v2=gfx.Scene().UnProject(geom.Vec3(x, y, 1.0));    
    atom=self.ligand.go.PickAtom(geom.Line3(v1, v2), 0.1)
    if atom.IsValid():
      self.ligand.SetPivot(atom)
    
  def Shift(self, vec):
    self.ligand.Shift(vec)
  def UpdateScores(self):
    prot_within=self.protein.handle.FindWithin(self.ligand.GetCenter(), 
                                               self.ligand.radius)
    lig_view=self.ligand.handle.CreateFullView()
    for a in prot_within:
      score=qa.ClashScore(a, lig_view)
      a.SetGenericFloatProperty('clash', score)
    self.surface.go.ReapplyColorOps()
  def GetRMSD(self):
    return self.ligand.RMSDToSolution()