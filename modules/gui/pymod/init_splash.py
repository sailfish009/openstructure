from PyQt4 import QtCore, QtGui

import os
import ost
from ost import gui

LOGO_PATH = os.path.join(ost.GetSharedDataPath(), "gui", "images", "logo-small.png")

class SplashDialog(QtGui.QDialog):
  def __init__(self, parent=None):
    QtGui.QDialog.__init__(self, parent)
    layout = QtGui.QHBoxLayout(self)
    self.setLayout(layout)
    imageLabel = QtGui.QLabel();
    self.pix_map = QtGui.QPixmap(LOGO_PATH);
    imageLabel.setPixmap(self.pix_map);
    layout.addWidget(imageLabel)
    self.label = QtGui.QTextEdit()
    self.label.setReadOnly(True)
    self.label.setHtml("Welcome to <b>Openstructure</b>!<br /><br />You are running version 1.0.0a<br /><br />If you are new to OpenStructure, we invite you to run the demos from the examples directory. Scripts can be displayed by right clicking on the file and selecting 'Show source'.<br /><br />Feel free visit our website at:<br /> http://www.openstructure.org")
    layout.addWidget(self.label)
    
def _InitSplash(app):
  splash = SplashDialog(app.perspective.main_area.qobject)
  #print dir(splash)

  splash.showNormal()

  #QtCore.QTimer.singleShot(30000, splash.close);
