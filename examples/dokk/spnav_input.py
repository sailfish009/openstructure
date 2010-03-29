from PyQt4 import QtCore, QtGui

from ost import mol, geom, gfx, gui

class SpnavInputDevice(QtCore.QObject):
  def __init__(self, gfx_win, parent=None):
    QtCore.QObject.__init__(self, parent)
    self.spnav = gui.SpnavInput.GetQThread()
    self.spnav.start()
    self.gfx_win = gfx_win
    self.level = None
    QtCore.QObject.connect(self.spnav,QtCore.SIGNAL("deviceTransformed(int,int,int,int,int,int)"), self.InputChanged)
    QtCore.QObject.connect(self.spnav,QtCore.SIGNAL("deviceButtonPressed(int)"), self.ToggleInputMode)     
    
    self.trans = True
    self.rot = True

  def SetLevel(self, level):
    self.level=level

  def InputChanged(self, tx,ty,tz,rx,ry,rz): 
    ligand = self.level.ligand
    transf = mol.Transform()
    if(self.trans):
      transf.ApplyXAxisTranslation(tx/480.0)
      transf.ApplyYAxisTranslation(ty/480.0)
      transf.ApplyZAxisTranslation(-tz/480.0)
    if(self.rot):
      transf.ApplyXAxisRotation(rx/480.0)
      transf.ApplyYAxisRotation(ry/480.0)
      transf.ApplyZAxisRotation(rz/480.0)
    ligand.SetTF(transf)
    gfx.Scene().RequestRedraw()
    self.gfx_win.update()

  def ToggleInputMode(self, button):
    if button == 0:
      self.trans = not self.trans
      print "Translation Enabled:",self.trans
    elif button == 1:
      self.rot = not self.rot
      print "Rotation Enabled:",self.rot

