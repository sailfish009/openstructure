import os
from ost import io, gfx, qa, geom
from ligand import Ligand
from surface import Surface
from protein import Protein
from score_updater import ScoreUpdater
from config import Config
from dokk import Dokk
from level_intro import LevelIntro

class Level:
  def __init__(self, name):
    self.name_=name
    self.Load()
    self.hud = None
    dokk = Dokk()
    self.li = LevelIntro(self)
        
  def Load(self):
    level_dir=os.path.join('datafiles', self.name_)
    self.config = Config(os.path.join(level_dir, 'level.ini'))
    print 'Loading %s' % self.config.Level["NAME"]
    ligand_ent=io.LoadSDF(os.path.join(level_dir, 'ligand.sdf'))
    self.ligand=Ligand(ligand_ent,self.config)
    protein_ent=io.LoadPDB(os.path.join(level_dir, 'protein.pdb'))
    self.protein=Protein(protein_ent)
    surface=io.LoadSurface(os.path.join(level_dir, 'surface'), 'msms')
    self.surface=Surface(surface)
    self.surface.handle.Attach(self.protein.handle, 5.0)
    gfx.Scene().SetCenter(self.surface.go.GetCenter())
    print 'Done Loading'
    self.su = ScoreUpdater(self)
    self.transform_ = gfx.Scene().GetTransform()
    
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
    self.su.UpdateScores()
    
  def GetRMSD(self):
    return self.ligand.RMSDToSolution()

  def Reset(self):
    self.ligand.Reset()
    gfx.Scene().SetTransform(self.transform_)
    gfx.Scene().SetCenter(self.surface.go.GetCenter())
    gfx.Scene().RequestRedraw()
    self.su.UpdateScores()
    
  def Solve(self):
    self.ligand.Solve()
    gfx.Scene().SetTransform(self.transform_)
    gfx.Scene().SetCenter(self.surface.go.GetCenter())
    gfx.Scene().RequestRedraw()
    self.su.UpdateScores()
    
  def AddHUDObject(self, hud_object):
    self.hud.Add(hud_object)
    
  def Begin(self):
    self.li.Start()
    
  def Close(self):
    self.surface.Close()
    self.protein.Close()
    self.ligand.Close()