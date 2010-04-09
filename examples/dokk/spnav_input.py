import dokk
from PyQt4 import QtCore, QtGui
from hud import TextHUDObject
from ost import mol, geom, gfx, gui

DEFAULT_REFRESHRATE = 30

class SpnavInputDevice(QtCore.QObject):
  def __init__(self, gfx_win, parent=None):
    QtCore.QObject.__init__(self, parent)
    self.refresh_rate_ = DEFAULT_REFRESHRATE
    try:
      self.spnav = gui.SpnavInput.GetQThread()
      self.spnav.start()
      self.gfx_win = gfx_win
      QtCore.QObject.connect(self.spnav,QtCore.SIGNAL("deviceTransformed(int,int,int,int,int,int)"), self.InputChanged)
      QtCore.QObject.connect(self.spnav,QtCore.SIGNAL("deviceButtonPressed(int)"), self.ToggleInputMode)     
    except AttributeError:
      pass #No SpaceNav Support
    
    self.trans = True
    self.rot = True
    
    self._lock_input = False
    
    self.score_scip = 0
  def SetLevel(self, level):
    self.level=level
    self.trans_hud = TextHUDObject("Translation Enabled: %s"%self.trans, QtCore.QPoint(10,50), 0)
    self.rot_hud = TextHUDObject("Rotation Enabled: %s"%self.rot, QtCore.QPoint(10,70), 0)
    dokk.Dokk().gl_win.AddHUDObject(self.trans_hud)
    dokk.Dokk().gl_win.AddHUDObject(self.rot_hud)
    try:
      self.refresh_rate_ = int(level.config.Score["FRAMESKIP"])
    except:
      pass

  def InputChanged(self, tx,ty,tz,rx,ry,rz):
    if (not self._lock_input) and (self.level is not None):
      ligand = self.level.ligand
      transf = mol.Transform()
      if(self.trans):
        delta = geom.Vec3 (tx/480.0, ty/480.0, -tz/480.0)
        transf.SetTrans(delta)
      if(self.rot):
        rot=gfx.Scene().GetTransform().GetRot()
        transf.ApplyAxisRotation(rx/480.0, rot.GetRow(0))
        transf.ApplyAxisRotation(ry/480.0, rot.GetRow(1))
        transf.ApplyAxisRotation(-rz/480.0, rot.GetRow(2))
      ligand.ApplyTF(transf)
      if self.score_scip >= self.refresh_rate_:
        self.level.UpdateScores()
        self.score_scip = 0
      self.score_scip += 1
      gfx.Scene().RequestRedraw()

  def SetLockInput(self, lock):
    self._lock_input = lock

  def ToggleInputMode(self, button):
    if button == 2:
      self.level.Begin()
    elif button == 3:
      dokk.Dokk().PreviousLevel()
    elif button == 4:
      dokk.Dokk().NextLevel()
    elif button == 6:
      QtGui.QApplication.exit()
        
    if (not self._lock_input):
      if button == 0:
        self.trans = not self.trans
        self.trans_hud.text= "Translation Enabled: %s"%self.trans
        self.trans_hud.time = 2000
        self.trans_hud.Reset()
      elif button == 1:
        self.rot = not self.rot
        self.rot_hud.text= "Rotation Enabled: %s"%self.rot
        self.rot_hud.time = 2000
        self.rot_hud.Reset()
      elif button == 7:
        self.level.Reset()
      elif button == 10:
        self.level.Finished()
      elif button == 11:
        self.level.Finished()
      elif button == 12:
        self.level.Solve()
      elif button == 13:
        self.level.ResetPos()
