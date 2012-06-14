from PyQt4 import QtCore

from ost import gfx, gui

class SpacenavControl(QtCore.QObject):
  def __init__(self, spnav,
               parent=None):
    QtCore.QObject.__init__(self, parent)
    QtCore.QObject.connect(spnav,QtCore.SIGNAL("deviceTransformed(int,int,int,int,int,int)"), self.Changed)
    QtCore.QObject.connect(spnav,QtCore.SIGNAL("deviceButtonPressed(int)"), self.Toggle)     
    
    self.trans = True
    self.rot = True
    self.speed = 480.0
    
  def Changed(self, tx,ty,tz,rx,ry,rz): 
    transf = gfx.Scene().GetTransform()
    if(self.trans):
      transf.ApplyXAxisTranslation(tx/self.speed)
      transf.ApplyYAxisTranslation(ty/self.speed)
      transf.ApplyZAxisTranslation(-tz/self.speed)
    if(self.rot):
      transf.ApplyXAxisRotation(rx/self.speed)
      transf.ApplyYAxisRotation(ry/self.speed)
      transf.ApplyZAxisRotation(rz/self.speed)
    gfx.Scene().SetTransform(transf)
    gfx.Scene().RequestRedraw()

  def Toggle(self, button):
    if button == 0:
      self.trans = not self.trans
      print "Translation Enabled:",self.trans
    elif button == 1:
      self.rot = not self.rot
      print "Rotation Enabled:",self.rot
    elif button == 12:
      if self.speed > 20:
        self.speed *= 0.8
        print "Speed Increased:",self.speed
    elif button == 13:
      self.speed /= 0.8
      print "Speed Reduced:",self.speed
    else:
      print "other:",button

      
def _InitSpaceNav(app):
  try:
    spnav = gui.SpnavInput.GetQThread()
    spnav.start()
    parent = app.gl_win.qobject
    SpacenavControl(spnav,parent)
  except AttributeError:
    pass
