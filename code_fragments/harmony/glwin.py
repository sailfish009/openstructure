import math
from ost import gfx, geom, mol
from PyQt4.QtGui import *
from PyQt4.QtCore import *
from PyQt4.QtOpenGL import *

TRANS_VAL = 20

class DokkGLCanvas(QGLWidget):

  def __init__(self, format, parent=None):
    QGLWidget.__init__(self, format, parent)
    self.last_pos_=QPoint()
    self.setAutoFillBackground(False)
    #self.setAttribute(Qt.WA_KeyCompression,True)
    self.resize(800, 800)
    self.atom=mol.AtomHandle()
  def initializeGL(self):
    gfx.Scene().InitGL()
    self.startTimer(10)
  def paintGL(self):
    gfx.Scene().RenderGL()
  def paintEvent(self, event):
    self.makeCurrent()
    self.paintGL()
    #painter=QPainter(self)
    #self.RenderHUD(painter)
    self.swapBuffers()

  def RenderHUD(self, painter):
    painter.setPen(QColor(100, 100, 100, 50))
    painter.setBrush(QColor(200, 200, 200, 50))
    painter.drawRect(QRect(QPoint(0, 0), QSize(self.width(), 25)))
    painter.setPen(QPen(QColor(255,255,255), Qt.SolidLine))
    painter.setFont(QFont("Verdana"))
  def resizeGL(self, w, h):
    gfx.Scene().Resize(w, h)

  def mousePressEvent(self, event):
    self.last_point_=QPoint(event.x(), event.y())
    v1=gfx.Scene().UnProject(geom.Vec3(event.x(), self.height()-event.y(), 0.0));
    v2=gfx.Scene().UnProject(geom.Vec3(event.x(), self.height()-event.y(), 1.0));
    self.atom=self.world.go.PickAtom(geom.Line3(v1, v2), 0.1)
    self.world.go.SetColor(gfx.WHITE, '')
    if self.atom.IsValid():
      self.world.go.SetColorForAtom(gfx.RED, self.atom)
  def mouseReleaseEvent(self, event):
    self.world.go.SetColor(gfx.WHITE, '')
    self.atom=mol.AtomHandle()
  def mouseMoveEvent(self, event):
    delta=QPoint(event.x(), event.y())-self.last_point_
    self.last_point_=QPoint(event.x(), event.y())
    if event.buttons() & Qt.LeftButton:
      if self.atom.IsValid():
        rot=gfx.Scene().GetTransform().GetRot()
        force=-rot.GetRow(1)*delta.y()+rot.GetRow(0)*delta.x()
        self.world.AddForce(self.atom, force*2)
  def wheelEvent(self, event):
    self.OnTransform(gfx.INPUT_COMMAND_TRANSZ,0,gfx.TRANSFORM_VIEW,
              0.1*(-event.delta()))

  def timerEvent(self, event):
    self.world.Update()
    self.update()
  def OnTransform(self,com, indx, trg, val):
    gfx.Scene().Apply(gfx.InputEvent(gfx.INPUT_DEVICE_MOUSE,
                                     com, indx,trg,val*0.5),False)

  def keyReleaseEvent(self, event):
    if event.key() == Qt.Key_Escape:
      QApplication.exit()

class DokkGLWin(gfx.GLWinBase):
    def _CreateFormat(self):
      fmt=QGLFormat()
      fmt.setAlpha(True)
      return fmt
    def __init__(self):
        gfx.GLWinBase.__init__(self)
        self.canvas_=DokkGLCanvas(self._CreateFormat())
    def DoRefresh(self):
      self.refresh_=True
    def SetLevel(self, level):
      self.canvas_.SetLevel(level)
    def Show(self, fullscreen):
      if fullscreen:
        self.canvas_.showFullScreen()
      else:
        self.canvas_.show()
    def SetStereo():
      pass
