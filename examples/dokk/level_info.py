import dokk
from PyQt4 import QtCore, QtGui
from hud import *
from ost import gui

class LevelInfo(QtCore.QObject):
  def __init__(self, level, parent=None):
    QtCore.QObject.__init__(self, parent)
    self.level = level
    
    self.timer = QtCore.QTimer()
    self.timer.setSingleShot(True)
    
    self.highscore = HighScore(level.topten)
    self.level_info = LevelDetails(level.config.Level["NAME"],level.config.Level["DIFFICULTY"])
    self.connect(level,QtCore.SIGNAL("Start()"),self.Finish)
    self.connect(level,QtCore.SIGNAL("Stopped()"),self.Start)
    self.connect(level,QtCore.SIGNAL("Closed()"),self.Close)
    self.connect(self.timer,QtCore.SIGNAL("timeout()"),self.Timeout)
    
    self.closed = False
  def Start(self):
    self.timer.start(1000)
    
  def Finish(self):
    self.highscore.Finish()
    self.level_info.Finish()
    
  def Timeout(self):
    if not self.closed:
      self.highscore.Start()
      self.level_info.Start()
    
  def Close(self):
    self.closed = True
    self.Finish()
    
      
class HighScore(QtCore.QObject):
  def __init__(self, topten, parent=None):
    QtCore.QObject.__init__(self, parent)
    self.topten = topten
      
  def Start(self):
    text = "%-2s%5s%-6s%5s%+6s"%("#", "", "Name", "", "Score")
    i=1
    data = self.topten.GetData(i)
    while(data):
       text += "\n%-2s%5s%-6s%5s%+6s"%(str("%-2i"%i), "", data[0],"", str("%3.2f"%float(data[1])))
       i += 1
       data = self.topten.GetData(i)
    rect = QtCore.QRect(QtCore.QPoint(60, 120), QtCore.QSize(24*20, (i+2)*30))
    self.bg = RectHUDObject(-1,rect, bg_color=QtGui.QColor(128,128,128,200))
    dokk.Dokk().gl_win.AddHUDObject(self.bg)
    self.hud_text = RectTextHUDObject(text, rect=rect, time=-1, font=QtGui.QFont("Courier",20))
    dokk.Dokk().gl_win.AddHUDObject(self.hud_text)
                  
  def Finish(self):
    dokk.Dokk().gl_win.RemoveHUDObject(self.hud_text)
    dokk.Dokk().gl_win.RemoveHUDObject(self.bg)
    
class LevelDetails(QtCore.QObject):
  def __init__(self, name, difficulty, parent=None):
    QtCore.QObject.__init__(self, parent)
    self.name = name
    self.difficulty = difficulty
      
  def Start(self):
    name_len = len(self.name)
    xpos = dokk.Dokk().gl_win.Width()/2 - (name_len*30)/2
    rect = QtCore.QRect(QtCore.QPoint(xpos, 55), QtCore.QSize(name_len*30, 105))
    self.bg = RectHUDObject(-1,rect, bg_color=QtGui.QColor(128,128,128,200))
    dokk.Dokk().gl_win.AddHUDObject(self.bg)
    text = "%s\n"%self.name
    self.name_text = RectTextHUDObject(text, rect=rect, time=-1, font=QtGui.QFont("Verdana",30))
    text = "%s"%self.difficulty
    self.diffi_text = TextHUDObject(text, QtCore.QPoint(dokk.Dokk().gl_win.Width()/2 - (len(text)*12)/2, 135), time=-1, font=QtGui.QFont("Courier",15))
    dokk.Dokk().gl_win.AddHUDObject(self.name_text)
    dokk.Dokk().gl_win.AddHUDObject(self.diffi_text)
                  
  def Finish(self):
    dokk.Dokk().gl_win.RemoveHUDObject(self.diffi_text)
    dokk.Dokk().gl_win.RemoveHUDObject(self.name_text)
    dokk.Dokk().gl_win.RemoveHUDObject(self.bg)