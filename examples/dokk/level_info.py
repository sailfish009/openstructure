import dokk
from PyQt4 import QtCore, QtGui
from hud import *
from ost import mol, geom

class LevelInfo(QtCore.QObject):
  def __init__(self, level, parent=None):
    QtCore.QObject.__init__(self, parent)
    self.level = level
    
    self.timer = QtCore.QTimer()
    self.timer.setSingleShot(True)
    
    if bool(int(level.config.Level["SAVE"])):
      self.highscore = HighScore(level.topten)
    else:
      self.highscore = None
      
    try:
      self.level_descr = LevelDescr(level.config.INFO["TEXT"])
    except:
      self.level_descr = None
    
    self.level_info = LevelDetails(level.config.Level["NAME"],level.config.Level["DIFFICULTY"])
    self.demo_cam = DemoCam(self)
    self.connect(level,QtCore.SIGNAL("Started()"),self.Finish)
    self.connect(level,QtCore.SIGNAL("Stopped()"),self.Start)
    self.connect(level,QtCore.SIGNAL("Closed()"),self.Finish)
    self.connect(self.timer,QtCore.SIGNAL("timeout()"),self.Timeout)
    
    self.start = False
  def Start(self):
    self.start = True
    self.timer.start(1000)
    
  def Finish(self):
    self.start = False
    if self.highscore:
      self.highscore.Finish()
    if self.level_descr:
      self.level_descr.Finish()
    self.demo_cam.Finish()
    self.level_info.Finish()
    
  def Timeout(self):
    if self.start:
      if self.highscore:
        self.highscore.Start()
      if self.level_descr:
        self.level_descr.Start()
      self.level_info.Start()
      self.demo_cam.Start()
   
      
class HighScore(QtCore.QObject):
  def __init__(self, topten, parent=None):
    QtCore.QObject.__init__(self, parent)
    self.topten = topten
    
    self.hud_text = None
    self.hud_bg = None
        
  def Start(self):
    text = "%-2s%5s%-6s%5s%+6s"%("#", "", "Name", "", "Score")
    i=1
    data = self.topten.GetData(i)
    while(data):
       text += "\n%-2s%5s%-6s%5s%+6s"%(str("%-2i"%i), "", data[0],"", str("%3.2f"%float(data[1])))
       i += 1
       data = self.topten.GetData(i)
    rect = QtCore.QRect(QtCore.QPoint(60, 300), QtCore.QSize(24*20, (i+2)*30))
    self.hud_bg = RectHUDObject(-1,rect, bg_color=QtGui.QColor(128,128,128,200))
    dokk.Dokk().gl_win.AddHUDObject(self.hud_bg)
    self.hud_text = RectTextHUDObject(text, rect=rect, time=-1, font=QtGui.QFont("Courier",20))
    dokk.Dokk().gl_win.AddHUDObject(self.hud_text)
                  
  def Finish(self):
    if self.hud_text:
      dokk.Dokk().gl_win.RemoveHUDObject(self.hud_text)
    if self.hud_bg:
      dokk.Dokk().gl_win.RemoveHUDObject(self.hud_bg)
    
class LevelDetails(QtCore.QObject):
  def __init__(self, name, difficulty, parent=None):
    QtCore.QObject.__init__(self, parent)
    self.name = name
    self.difficulty = difficulty
    
    self.hud_name_text = None
    self.hud_diffi_text = None
    self.hud_bg = None
    self.hud_image = None
  def Start(self):
    name_len = max(len(self.name),len(self.difficulty)/2)
    xpos = dokk.Dokk().gl_win.Width()/2 - (name_len*30)/2
    rect = QtCore.QRect(QtCore.QPoint(xpos, 55), QtCore.QSize(name_len*30, 105))
    self.hud_bg = RectHUDObject(-1,rect, bg_color=QtGui.QColor(128,128,128,200))
    dokk.Dokk().gl_win.AddHUDObject(self.hud_bg)
    self.hud_image = ImgHUDObject(-1,QtCore.QPoint(dokk.Dokk().gl_win.Width()-300, dokk.Dokk().gl_win.Height()-100),"images/openstructure.png")
    dokk.Dokk().gl_win.AddHUDObject(self.hud_image)
    text = "%s\n"%self.name
    self.hud_name_text = RectTextHUDObject(text, rect=rect, time=-1, font=QtGui.QFont("Verdana",30))
    text = "%s"%self.difficulty
    self.hud_diffi_text = TextHUDObject(text, QtCore.QPoint(dokk.Dokk().gl_win.Width()/2 - (len(text)*12)/2, 135), time=-1, font=QtGui.QFont("Courier",15))
    dokk.Dokk().gl_win.AddHUDObject(self.hud_name_text)
    dokk.Dokk().gl_win.AddHUDObject(self.hud_diffi_text)
                  
  def Finish(self):
    dokk.Dokk().gl_win.RemoveHUDObject(self.hud_diffi_text)
    dokk.Dokk().gl_win.RemoveHUDObject(self.hud_name_text)
    dokk.Dokk().gl_win.RemoveHUDObject(self.hud_bg)
    dokk.Dokk().gl_win.RemoveHUDObject(self.hud_image)

class LevelDescr(QtCore.QObject):
  def __init__(self, descr, parent=None):
    QtCore.QObject.__init__(self, parent)
    self.descr = descr
  
    self.hud_descr_text = None
    self.hud_bg = None

  def Start(self):
    descr_len = len(self.descr)
    xpos = dokk.Dokk().gl_win.Width()/2 - (descr_len*30)/2
    rect = QtCore.QRect(QtCore.QPoint(xpos, dokk.Dokk().gl_win.Height()-105), QtCore.QSize(descr_len*30, 105))
    self.hud_bg = RectHUDObject(-1,rect, bg_color=QtGui.QColor(128,128,128,200))
    dokk.Dokk().gl_win.AddHUDObject(self.hud_bg)
    self.hud_descr_text = RectTextHUDObject(self.descr, rect=rect, time=-1, font=QtGui.QFont("Verdana",20))
    dokk.Dokk().gl_win.AddHUDObject(self.hud_descr_text)
                  
  def Finish(self):
    dokk.Dokk().gl_win.RemoveHUDObject(self.hud_bg)
    dokk.Dokk().gl_win.RemoveHUDObject(self.hud_descr_text)

class DemoCam(QtCore.QTimer):
  def __init__(self, parent=None):
    QtCore.QTimer.__init__(self)
    QtCore.QObject.connect(self, QtCore.SIGNAL("timeout()"), self.OnTimer)
        
  def Start(self):
    dokk.Dokk().GetLevel().Solve()
    self.start(10)
     
  def Finish(self):
    self.stop()
  
  def OnTimer(self):
    gfx.Scene().Apply(gfx.InputEvent(gfx.INPUT_DEVICE_MOUSE, gfx.INPUT_COMMAND_ROTY, 0.25), False)      
      
