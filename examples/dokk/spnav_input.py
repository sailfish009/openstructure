from PyQt4 import QtCore, QtGui

from ost import mol, geom, gfx, gui

DEFAULT_REFRESHRATE = 30

class SpnavInputDevice(QtCore.QObject):
  def __init__(self, gfx_win, parent=None):
    QtCore.QObject.__init__(self, parent)
    self.refresh_rate_ = DEFAULT_REFRESHRATE
    self.spnav = gui.SpnavInput.GetQThread()
    self.spnav.start()
    self.gfx_win = gfx_win
    self.level = None
    QtCore.QObject.connect(self.spnav,QtCore.SIGNAL("deviceTransformed(int,int,int,int,int,int)"), self.InputChanged)
    QtCore.QObject.connect(self.spnav,QtCore.SIGNAL("deviceButtonPressed(int)"), self.ToggleInputMode)     
    
    self.trans = True
    self.rot = True

    self.score_scip = 0
  def SetLevel(self, level):
    self.level=level
    try:
      self.refresh_rate_ = int(level.config.Score["FRAMESKIP"])
    except:
      pass

  def InputChanged(self, tx,ty,tz,rx,ry,rz):
    if self.level is not None:
      ligand = self.level.ligand
      transf = mol.Transform()
      if(self.trans):
        scene_rot = geom.Mat4(gfx.Scene().GetTransform().GetRot())
        center = gfx.Scene().GetCenter()
        delta = geom.Vec3 (tx/480.0, ty/480.0, -tz/480.0)
        transf.SetTrans(delta)
      if(self.rot):
        transf.ApplyXAxisRotation(rx/480.0)
        transf.ApplyYAxisRotation(ry/480.0)
        transf.ApplyZAxisRotation(rz/480.0)
      ligand.SetTF(transf)
      if self.score_scip >= self.refresh_rate_:
        self.level.UpdateScores()
        self.score_scip = 0
      self.score_scip += 1
      gfx.Scene().RequestRedraw()
      self.gfx_win.update()

  def ToggleInputMode(self, button):
    print button
    if button == 0:
      self.trans = not self.trans
      print "Translation Enabled:",self.trans
    elif button == 1:
      self.rot = not self.rot
      print "Rotation Enabled:",self.rot
    elif button == 6:
      QtGui.QApplication.exit()
    elif button == 10:
      self.level.Reset()
      self.gfx_win.update()
    elif button == 11:
      self.level.Solve()
      self.gfx_win.update()
    
