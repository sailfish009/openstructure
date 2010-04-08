import os
import time
from PyQt4 import QtCore, QtGui
from ost import io, gfx, qa, geom
from ligand import Ligand
from surface import Surface
from protein import Protein
from score_updater import ScoreUpdater
from config import *
from dokk import Dokk
from level_messages import LevelIntro
from level_messages import LevelEnd
from name_enter import HUDNameInput
from highscore import HighScore

class Level(QtCore.QObject):
  def __init__(self, name, parent=None):
    QtCore.QObject.__init__(self,parent)
    self.name_=name
    self.Load()
    self.hud = None
    
    self.endtime = 0
    self.stop_time = 0
    
    self.timer = QtCore.QTimer()
    self.timer.setSingleShot(True)
    self.connect(self.timer,QtCore.SIGNAL("timeout()"),self.Finished)
    self.li = LevelIntro(self.config)
    self.connect(self.li,QtCore.SIGNAL("Finished()"),self._IntroEnd)
    self.le = None
    self.hni = None
    self.highscore = HighScore(self)
    Dokk().gl_win.SetLockInput(True)
    self._started = False
    self.emit(QtCore.SIGNAL("Stopped()"))
    
  def Load(self):
    level_dir=os.path.join('datafiles', self.name_)
    self.config = Config(os.path.join(level_dir, 'level.ini'))
    self.topten = TopTen(os.path.join(level_dir, 'top_ten.ini'))
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

  def GetScore(self):
    rmsd = self.GetRMSD()
    return rmsd + self.GetTimeMalus()
  
  def GetRank(self):
    return self.topten.GetRank(self.GetScore())
  
  def GetTTDiff(self):
    return self.topten.GetDiff(self.GetScore())
      
  def GetStopTime(self):
    time = (self.endtime-self.stop_time)
    if time > 0:
      return time
    return 0
    
  def GetRemainingTime(self):
    if self._started:
      rem_time = self.endtime-time.time()
      if rem_time > 0:
        return rem_time
    return 0
  
  def GetTimeMalus(self):
    rem_time = self.GetStopTime()
    if(rem_time>0):
      return 1- (rem_time / (float(self.config.Level["TIME"])/1000.0))
    return 1
  
  def Reset(self):
    self.endtime = 0
    self.stop_time = 0
    Dokk().gl_win.ClearHUDObjects()
    if self.le is not None:
      self.le.Stop()
    self.li.Stop()
    self.hni = None
    
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
  
  def Finished(self):
    self._started = False
    self.timer.stop()
    self.stop_time = time.time()
    Dokk().gl_win.SetLockInput(True)
    if self.GetRank() > 0:
      self.le = LevelEnd(self.config.TT)
    else:
      self.le = LevelEnd(self.config.NTT)
    self.connect(self.le,QtCore.SIGNAL("Finished()"),self._FinishEnd)
    self.le.Start()
        
  def Begin(self):
    self.Reset()
    self.su.UpdateScores()
    self.li.Start()
    self._started = True
    self.emit(QtCore.SIGNAL("Started()"))
    
  def Close(self):
    if self.le is not None:
      self.le.Stop()
    self.li.Stop()
    self._started = False
    self.surface.Close()
    self.protein.Close()
    self.ligand.Close()

  def _IntroEnd(self):
    if self._started:
      Dokk().gl_win.SetLockInput(False)
      self.timer.start(int(self.config.Level["TIME"]))
      self.endtime = float(time.time()+(int(self.config.Level["TIME"]))/1000.0)
      
  def _FinishEnd(self):
    if bool(int(self.config.Level["SAVE"])) and self.GetRank()>0:
      self.hni = HUDNameInput()
      self.connect(self.hni,QtCore.SIGNAL("Finished()"),self._FinishEnterName)
    else:
      self.Reset()
      self.emit(QtCore.SIGNAL("Stopped()"))
      
  def _FinishEnterName(self):
    self.topten.SetValue(self.hni.GetName(),self.GetScore())
    self.Reset()
    self.emit(QtCore.SIGNAL("Stopped()"))
