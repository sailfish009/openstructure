import traceback

from PyQt4 import QtCore

import ost
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
    if(self.rot):
      transf.ApplyXAxisRotation(rx/self.speed)
      transf.ApplyYAxisRotation(ry/self.speed)
      transf.ApplyZAxisRotation(rz/self.speed)
    if(self.trans or self.rot):
      transf.ApplyZAxisTranslation(-tz/self.speed)
    gfx.Scene().SetTransform(transf)
    gfx.Scene().RequestRedraw()

  def Toggle(self, button):
    if button == 0:
      self.trans = not self.trans
      ost.LogVerbose("SpaceNav: translation %s "%("enabled" if self.trans else "disabled"))
    elif button == 1:
      self.rot = not self.rot
      ost.LogVerbose("SpaceNav: rotation %s"%("enabled" if self.rot else "disabled"))
    elif button == 12:
      if self.speed > 20:
        self.speed *= 0.8
        ost.LogVerbose("SpaceNav: speed increased to "+str(self.speed))
    elif button == 13:
      self.speed /= 0.8
      ost.LogVerbose("SpaceNav: speed reduced to "+str(self.speed))
    else:
      ost.LogDebug("SpaceNav: unmapped button press ["+str(button)+"]")

      
def _InitSpaceNav(app):
  try:
    spnav = gui.SpnavInput.GetQThread()
    if spnav:
      spnav.start()
      parent = app.gl_win.qobject
      SpacenavControl(spnav,parent)
      ost.LogInfo("SpaceNav: device found and connected")
    else:
      ost.LogInfo("SpaceNav: no device found, or could not connect to device socket")
  except:
    ost.LogInfo("SpaceNav: caught exception during initialization: %s"%(traceback.format_exc()))
