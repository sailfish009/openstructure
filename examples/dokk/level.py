import os
import time
from PyQt4 import QtCore, QtGui
from ost import io, gfx, geom
from ligand import Ligand
from surface import Surface
from protein import Protein
from score_updater import ScoreUpdater
from config import Config, TopTen
from dokk import Dokk
from level_messages import LevelIntro
from level_messages import LevelEnd
from name_enter import HUDNameInput
from level_info import LevelInfo

class Level(QtCore.QObject):
  def __init__(self, name, parent=None):
    QtCore.QObject.__init__(self,parent)
    self.name_=name
    
    self.endtime = 0
    self.stop_time = 0

    self.Load()    
    self.LoadHUD()
    
    self.timer = QtCore.QTimer()
    self.timer.setSingleShot(True)
    self.connect(self.timer,QtCore.SIGNAL("timeout()"),self.Finished)
        
    Dokk().gl_win.SetLockInput(True)
    self._started = False
    self._intro_done = False
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
    
  def LoadHUD(self):
    self.hud_level_intro = LevelIntro(self.config)
    self.connect(self.hud_level_intro,QtCore.SIGNAL("Finished()"),self._IntroEnd)
    self.hud_level_end_tt = LevelEnd(self.config.TT)
    self.connect(self.hud_level_end_tt,QtCore.SIGNAL("Finished()"),self._FinishEnd)
    self.hud_level_end_ntt = LevelEnd(self.config.NTT)
    self.connect(self.hud_level_end_ntt,QtCore.SIGNAL("Finished()"),self._FinishEnd)
    self.hud_name_input = HUDNameInput()
    self.connect(self.hud_name_input,QtCore.SIGNAL("Finished()"),self._FinishEnterName)
    self.level_info = LevelInfo(self)
    
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
    score = self.GetScore()
    if self.topten.IsTopTen(self.GetScore()):
      return self.topten.GetRank(self.GetScore())
    return -1
  
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
    Dokk().gl_win.SetLockInput(True)
    self.endtime = 0
    self.stop_time = 0
    self._started = False
    self._intro_done = False
    self.timer.stop()
    self.CleanHUD()
    self.ResetPos()
    self.emit(QtCore.SIGNAL("Stopped()"))
  
  def CleanHUD(self):
    self.hud_level_end_tt.Stop()
    self.hud_level_end_ntt.Stop()
    self.hud_level_intro.Stop()
    self.hud_name_input.Stop()
    Dokk().gl_win.ClearHUDObjects()
  
  def ResetPos(self):
    self.ligand.Reset()
    gfx.Scene().SetTransform(self.transform_)
    gfx.Scene().SetCenter(self.surface.go.GetCenter())
    gfx.Scene().RequestRedraw()
    self.UpdateScores()
    
  def Solve(self):
    self.ligand.Solve()
    gfx.Scene().SetTransform(self.transform_)
    gfx.Scene().SetCenter(self.surface.go.GetCenter())
    gfx.Scene().RequestRedraw()
    self.UpdateScores()
  
  def Finished(self):
    self.timer.stop()
    self.stop_time = time.time()
    Dokk().gl_win.SetLockInput(True)
    if self._started:
      if self.GetRank() > 0 or self.GetRMSD()<= float(self.config.Level["GOAL"]):
        self.hud_level_end_tt.Start()
      else:
        self.hud_level_end_ntt.Start()
    self._started = False
    self._intro_done = False

  def Begin(self):
    self.Reset()
    self.UpdateScores()
    self.hud_level_intro.Start()
    self._started = True
    self.emit(QtCore.SIGNAL("Started()"))
    
  def QuickBegin(self):
    self.Reset()
    self.UpdateScores()
    self.hud_level_intro.QuickStart()
    self._started = True
    self.emit(QtCore.SIGNAL("Started()"))
    
  def Close(self):
    gfx.Scene().SetTransform(self.transform_)
    self.emit(QtCore.SIGNAL("Closed()"))
    self._started = False
    self.CleanHUD()
    self.surface.Close()
    self.protein.Close()
    self.ligand.Close()

  def CheckSolved(self):
    goal = float(self.config.Level["GOAL"])
    if self.GetRMSD()< goal:
      self.Finished()
    
  def IsStarted(self):
    return self._started

  def IsIntroDone(self):
    return self._intro_done
  
  def _IntroEnd(self):
    if self._started:
      Dokk().gl_win.SetLockInput(False)
      self.timer.start(int(self.config.Level["TIME"]))
      self.endtime = float(time.time()+(int(self.config.Level["TIME"]))/1000.0)
      self._intro_done = True
      self.emit(QtCore.SIGNAL("Started()"))
      
  def _FinishEnd(self):
    if bool(int(self.config.Level["SAVE"])) and self.GetRank()>0:
      self.hud_name_input.Start()
    else:
      self.Reset()
      self.emit(QtCore.SIGNAL("Stopped()"))
      
  def _FinishEnterName(self):
    self.topten.SetValue(self.hud_name_input.GetName(),self.GetScore())
    self.Reset()
