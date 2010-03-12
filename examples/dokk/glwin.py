import math
from ost import gfx
from PyQt4.QtGui import *
from PyQt4.QtCore import *
from PyQt4.QtOpenGL import *

class DokkGLCanvas(QGLWidget):
  def __init__(self, format, parent=None):
    QGLWidget.__init__(self, format, parent)
  def initializeGL(self):
    gfx.Scene().InitGL()

  def paintGL(self):
    gfx.Scene().RenderGL()

  def resizeGL(self, w, h):
    gfx.Scene().Resize(w, h)

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

    def Show(self):
      self.canvas_.show()
    def SetStereo():
      pass