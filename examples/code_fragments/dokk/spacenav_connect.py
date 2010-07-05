from PyQt4 import QtCore, QtGui

from ost import geom, gfx, gui

import sys, random

class SpnavWidget(QtCore.QObject):
  def __init__(self, spnav,
               parent=None):
    QtCore.QObject.__init__(self, parent)
    QtCore.QObject.connect(spnav,QtCore.SIGNAL("deviceTransformed(int,int,int,int,int,int)"), self.Changed)
    QtCore.QObject.connect(spnav,QtCore.SIGNAL("deviceButtonPressed(int)"), self.Toggle)     
    
    self.e=io.LoadEntity("../entity/sdh.pdb")
    self.go=gfx.Entity("sdh",self.e)
    scene.Add(self.go)
    self.trans = True
    self.rot = True
  def Changed(self, tx,ty,tz,rx,ry,rz): 
    transf = self.go.GetTF()
    if(self.trans):
      transf.ApplyXAxisTranslation(tx/480.0)
      transf.ApplyYAxisTranslation(ty/480.0)
      transf.ApplyZAxisTranslation(-tz/480.0)
    if(self.rot):
      transf.ApplyXAxisRotation(rx/480.0)
      transf.ApplyYAxisRotation(ry/480.0)
      transf.ApplyZAxisRotation(rz/480.0) 
    self.go.SetTF(transf)
    gfx.Scene().RequestRedraw()

  def Toggle(self, button):
    if button == 0:
      self.trans = not self.trans
      print "Translation Enabled:",self.trans
    elif button == 1:
      self.rot = not self.rot
      print "Rotation Enabled:",self.rot
      
app=gui.GostyApp.Instance()

spnav = gui.SpnavInput.GetQThread()
spnav.start()
pts=SpnavWidget(spnav)
gfx.Scene().AutoAutoslab(False)
gfx.Scene().SetNearFar(1,10000)
