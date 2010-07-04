from PyQt4 import QtCore, QtGui
from dokk import Dokk
from hud import TextHUDObject
from hud import RectHUDObject
from hud import RectTextHUDObject
class LevelIntro(QtCore.QObject):
  def __init__(self, level, parent=None):
    QtCore.QObject.__init__(self,parent)
    self.level = level
    text_count = int(level.config.Intro["TEXT_COUNT"])
    self.text_list = list()
    default_time = int(level.config.Intro["TEXTTIME"])
    default_color = QtGui.QColor(int(level.config.Intro["TEXTCOLORRED"]),
                                 int(level.config.Intro["TEXTCOLORGREEN"]),
                                 int(level.config.Intro["TEXTCOLORBLUE"]))
    default_size = int(level.config.Intro["TEXTSIZE"])
    self.total_time = 0
    for i in range(1,text_count+1):
      try:
        time = int(level.config.Intro["TEXTTIME%s"%i])
      except:
        time = default_time
      try:
        color = QtGui.QColor(int(level.config.Intro["TEXTCOLORRED%s"%i]),
                                 int(level.config.Intro["TEXTCOLORGREEN%s"%i]),
                                 int(level.config.Intro["TEXTCOLORBLUE%s"%i]))
      except:
        color = default_color
      try:
        size = int(level.config.Intro["TEXTSIZE%s"%i])
      except:
        size = default_size
      self.text_list.append([level.config.Intro["TEXT%s"%i], time, color, size])
      self.total_time += time
        
    self.cur_text = 0

  def Start(self):
    dokk = Dokk()
    font = QtGui.QFont("Verdana", 15);
    rect = QtCore.QRect(QtCore.QPoint(60, 60), QtCore.QSize(dokk.gl_win.Width()-120, dokk.gl_win.Height()-120))
    self.text = RectTextHUDObject(text="", rect=rect, time=0, font=font)
    QtCore.QObject.connect(self.text, QtCore.SIGNAL("Finished()"), self.NextMessage)
    self.bg = RectHUDObject(self.total_time,rect, bg_color=QtGui.QColor(128,128,128,200))
    self.level.AddHUDObject(self.bg)
    self.level.AddHUDObject(self.text)
    
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
      self.cur_text = -1
    self.cur_text+=1
