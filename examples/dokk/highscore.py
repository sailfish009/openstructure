import dokk
from PyQt4 import QtCore, QtGui
from hud import *
from ost import gui

class HighScore(QtCore.QObject):
  def __init__(self, level):
    QtCore.QObject.__init__(self, level)
    self.level = level
    self.connect(level,QtCore.SIGNAL("Start()"),self.Finish)
    self.connect(level,QtCore.SIGNAL("Stopped()"),self.Start)
    
  def GetName(self):
    return self.ne.GetName()
  
  def Start(self):
    topten=self.level.topten
    text = "%2s %4s %5s\n"%("#", "Name","Score")
    i=1
    while(topten.GetData(i)):
       data = topten.GetData(i)
       text += "%2i %4s %1.2f\n"%(i, data[0],float(data[1]))
       i += 1
    rect = QtCore.QRect(QtCore.QPoint(60, 60), QtCore.QSize(dokk.Dokk().gl_win.Width()/5, (i+1)*30))
    self.bg = RectHUDObject(-1,rect, bg_color=QtGui.QColor(128,128,128,200))
    dokk.Dokk().gl_win.AddHUDObject(self.bg)
    self.hud_text = RectTextHUDObject(text, rect=rect, time=-1, font=QtGui.QFont("Verdana",20))
    dokk.Dokk().gl_win.AddHUDObject(self.hud_text)
                  
  def Finish(self):
    dokk.Dokk().gl_win.RemoveHUDObject(self.hud_text)
    dokk.Dokk().gl_win.RemoveHUDObject(self.bg)