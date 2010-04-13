import math
from ost import gfx
from PyQt4.QtGui import *
from PyQt4.QtCore import *
from PyQt4.QtOpenGL import *

from spnav_input import SpnavInputDevice
from hud import HUD, HUDObject


UPDATE_INTERVAL = 20
TRANS_VAL = 20

class DokkGLCanvas(QGLWidget):
    
  def __init__(self, format, dokk, parent=None):
    QGLWidget.__init__(self, format, parent)
    self.last_pos_=QPoint()    
    self.setAutoFillBackground(False)
    #self.setAttribute(Qt.WA_KeyCompression,True)
    self.resize(800, 800)
    self.dokk = dokk
    self.spnav_input = SpnavInputDevice(self)
    self.update_timer = QTimer()
    self.update_timer.start(UPDATE_INTERVAL)
    
    self.hud = HUD()
        
    self._lock_input = False
    QObject.connect(self.update_timer, SIGNAL("timeout()"), self.update)
        
  def initializeGL(self):
    gfx.Scene().InitGL()

  def paintGL(self):
    gfx.Scene().RenderGL()

  def SetLevel(self, level):
    self.spnav_input.SetLevel(level)
    
  def paintEvent(self, event):
    self.makeCurrent()
    self.paintGL()
    painter=QPainter(self)
    painter.setRenderHint(QPainter.Antialiasing);
    self.RenderHUD(painter)


  def RenderHUD(self, painter):
    painter.setPen(QColor(100, 100, 100, 50))
    painter.setBrush(QColor(200, 200, 200, 50))
    painter.drawRect(QRect(QPoint(0, 0), QSize(self.width(), 35)))
    painter.setPen(QPen(QColor(255,255,255), Qt.SolidLine))
    painter.setFont(QFont("Verdana",20))
    painter.drawText(QPoint(10, 25), "You are %.1f away from the solution" % self.dokk.GetLevel().GetRMSD())
    painter.drawText(QPoint(self.width()-200, 25), "%.0f seconds left" % self.dokk.GetLevel().GetRemainingTime())
    if self.hud is not None:
      self.hud.Paint(painter)
    self.dokk.GetLevel().CheckSolved()
      
  def resizeGL(self, w, h):
    gfx.Scene().Resize(w, h)

  def mousePressEvent(self, event):
    self.last_point_=QPoint(event.x(), event.y())

  def mouseMoveEvent(self, event):
    delta=QPoint(event.x(), event.y())-self.last_point_
    self.last_point_=QPoint(event.x(), event.y())
    if not self._lock_input:
      if event.buttons() & Qt.LeftButton:
        tf=gfx.Scene().GetTransform()      
        if event.modifiers() & Qt.ShiftModifier:
          if event.modifiers() & Qt.AltModifier:
            self.dokk.GetLevel().Shift(tf.GetRot().GetRow(2)*delta.y()*0.1)          
          else:
            self.dokk.GetLevel().Shift((tf.GetRot().GetRow(0)*delta.x()
                             -tf.GetRot().GetRow(1)*delta.y())*0.1)
        else:
          if delta.y()!=0:
            self.dokk.GetLevel().RotateAxis(tf.GetRot().GetRow(0), delta.y()*0.005)
          if delta.x()!=0:
            self.dokk.GetLevel().RotateAxis(tf.GetRot().GetRow(1), delta.x()*0.005)
        self.dokk.GetLevel().UpdateScores()
        self.update()
    if event.buttons() & Qt.RightButton:
      if delta.y()!=0:
        gfx.Scene().Apply(gfx.InputEvent(gfx.INPUT_DEVICE_MOUSE,
                                         gfx.INPUT_COMMAND_ROTX, delta.y()*0.5),
                          False)
      if delta.x()!=0:
        gfx.Scene().Apply(gfx.InputEvent(gfx.INPUT_DEVICE_MOUSE,
                                         gfx.INPUT_COMMAND_ROTY, delta.x()*0.5),
                          False)
      self.update()
  def mouseDoubleClickEvent(self, event):
    if not self._lock_input:
      self.dokk.GetLevel().SetPivot(event.x(), self.height()-event.y())
  def wheelEvent(self, event):
    self.OnTransform(gfx.INPUT_COMMAND_TRANSZ,0,gfx.TRANSFORM_VIEW,
              0.1*(-event.delta()))


  def OnTransform(self,com, indx, trg, val):
    gfx.Scene().Apply(gfx.InputEvent(gfx.INPUT_DEVICE_MOUSE,
                                     com, indx,trg,val*0.5),False)
    self.update()
    
  def keyReleaseEvent(self, event):
    self.emit(SIGNAL("KeyPressed"),event)
    if event.key() == Qt.Key_Escape:
      QApplication.exit()
    elif event.key() == Qt.Key_P:
      self.dokk.PreviousLevel()
    elif event.key() == Qt.Key_N:
      self.dokk.NextLevel()
    elif event.key() == Qt.Key_Space:
      self.dokk.GetLevel().Begin()
    
    if not self._lock_input:
      if event.key() == Qt.Key_Left or event.key() == Qt.Key_A:
        self.OnTransform(gfx.INPUT_COMMAND_TRANSX,0, gfx.TRANSFORM_VIEW, -TRANS_VAL)
        
      elif event.key() == Qt.Key_Right or event.key() == Qt.Key_D:
        self.OnTransform(gfx.INPUT_COMMAND_TRANSX,0, gfx.TRANSFORM_VIEW, TRANS_VAL)
        
      elif event.key() == Qt.Key_Down or event.key() == Qt.Key_S:
        self.OnTransform(gfx.INPUT_COMMAND_TRANSZ,0, gfx.TRANSFORM_VIEW, TRANS_VAL)
        
      elif event.key() == Qt.Key_Up or event.key() == Qt.Key_W:
        self.OnTransform(gfx.INPUT_COMMAND_TRANSZ,0, gfx.TRANSFORM_VIEW, -TRANS_VAL)

      elif event.key() == Qt.Key_Backspace:
        self.dokk.GetLevel().Reset()
    
      elif event.key() == Qt.Key_Enter or event.key() == Qt.Key_Return:
        self.dokk.GetLevel().Finished()
        
      elif event.key() == Qt.Key_E:
        self.dokk.GetLevel().Solve()
        
      elif event.key() == Qt.Key_R:
        self.dokk.GetLevel().ResetPos()
    
  def SetLockInput(self, lock):
    self._lock_input = lock
    self.spnav_input.SetLockInput(lock)

class DokkGLWin(gfx.GLWinBase):
    def _CreateFormat(self):
      fmt=QGLFormat()
      fmt.setAlpha(True)
      fmt.setStereo(True)
      return fmt
    
    def __init__(self,dokk):
      gfx.GLWinBase.__init__(self)
      self.canvas=DokkGLCanvas(self._CreateFormat(),dokk)
        
    def DoRefresh(self):
      self.refresh_=True
      
    def SetLevel(self, level):
      self.canvas.SetLevel(level)
      
    def Show(self, fullscreen):
      if fullscreen:
        self.canvas.showFullScreen()
      else:
        self.canvas.show()
        
    def SetStereo(self, mode):
      gfx.Scene().Stereo(mode)
    
    def SetLockInput(self, lock):
      self.canvas.SetLockInput(lock)
    
    def Width(self):
      return self.canvas.width()
    
    def Height(self):
      return self.canvas.height()
    
    def ClearHUDObjects(self):
      self.canvas.hud.Clear()
    
    def RemoveHUDObject(self, hud_object):
      self.canvas.hud.Remove(hud_object)
      
    def AddHUDObject(self, hud_object):
      self.canvas.hud.Add(hud_object)
