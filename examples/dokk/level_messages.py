from PyQt4 import QtCore, QtGui
from dokk import Dokk
from hud import RectHUDObject
from hud import RectTextHUDObject

class LevelMessages(QtCore.QObject):
  def __init__(self, config_map, parent=None):
    QtCore.QObject.__init__(self,parent)
    text_count = int(config_map["TEXT_COUNT"])
    self.text_list = list()
    default_time = int(config_map["TEXTTIME"])
    default_color = QtGui.QColor(int(config_map["TEXTCOLORRED"]),
                                 int(config_map["TEXTCOLORGREEN"]),
                                 int(config_map["TEXTCOLORBLUE"]))
    default_size = int(config_map["TEXTSIZE"])
    self.total_time = 0
    for i in range(1,text_count+1):
      try:
        time = int(config_map["TEXTTIME%s"%i])
      except:
        time = default_time
      
      try:
        color = QtGui.QColor(int(config_map["TEXTCOLORRED%s"%i]),
                                 int(config_map["TEXTCOLORGREEN%s"%i]),
                                 int(config_map["TEXTCOLORBLUE%s"%i]))
      except:
        color = default_color
      
      try:
        size = int(config_map["TEXTSIZE%s"%i])
      except:
        size = default_size
      
      self.text_list.append([config_map["TEXT%s"%i], time, color, size])
      self.total_time += time
        
    self.cur_text = 0

  def Start(self):
    self.Reset()
    dokk = Dokk()
    font = QtGui.QFont("Verdana", 15);
    rect = QtCore.QRect(QtCore.QPoint(60, 60), QtCore.QSize(dokk.gl_win.Width()-120, dokk.gl_win.Height()-120))
    self.text = RectTextHUDObject(text="", rect=rect, time=0, font=font)
    QtCore.QObject.connect(self.text, QtCore.SIGNAL("Finished()"), self.NextMessage)
    self.bg = RectHUDObject(self.total_time,rect, bg_color=QtGui.QColor(128,128,128,200))
    dokk.gl_win.AddHUDObject(self.bg)
    dokk.gl_win.AddHUDObject(self.text)
  
  def Stop(self):
    self.cur_text = -1
  
  def Reset(self):
    self.cur_text = 0
    
  def NextMessage(self):
    if self.cur_text >=0 and self.cur_text < len(self.text_list):
      dokk = Dokk()
      self.text.text=self.text_list[self.cur_text][0]
      self.text.time=self.text_list[self.cur_text][1]
      self.text.color=self.text_list[self.cur_text][2]
      self.text.font.setPointSize(self.text_list[self.cur_text][3])
      self.text.Reset()
    else:
      self.emit(QtCore.SIGNAL('Finished()')) 
      self.cur_text = -1
    self.cur_text+=1
    
class LevelIntro(LevelMessages):
  def __init__(self, config, parent=None):
    LevelMessages.__init__(self,config.INTRO, parent)

class LevelEnd(LevelMessages):
  def __init__(self, messages, parent=None):
    LevelMessages.__init__(self,messages, parent)

  def NextMessage(self):
    if self.cur_text >=0 and self.cur_text < len(self.text_list):
      self.text.text=self.text_list[self.cur_text][0].replace("$SCORE","%.2f"% Dokk().GetLevel().GetScore())
      self.text.text=self.text.text.replace("$TIMEMALUS","%.2f"% Dokk().GetLevel().GetTimeMalus())
      self.text.text=self.text.text.replace("$RMSD","%.2f"% Dokk().GetLevel().GetRMSD())
      self.text.text=self.text.text.replace("$RANK","%i"% Dokk().GetLevel().GetRank())
      self.text.text=self.text.text.replace("$DIFFERENCE","%.2f"% Dokk().GetLevel().GetTTDiff())
      self.text.time=self.text_list[self.cur_text][1]
      self.text.color=self.text_list[self.cur_text][2]
      self.text.font.setPointSize(self.text_list[self.cur_text][3])
      self.text.Reset()
    else:
      self.emit(QtCore.SIGNAL('Finished()')) 
      self.cur_text = -1
    self.cur_text+=1
    
