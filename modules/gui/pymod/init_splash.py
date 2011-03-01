from PyQt4 import QtCore, QtGui

import os
import ost
from ost import gui

LOGO_PATH = os.path.join(ost.GetSharedDataPath(), "gui", "images", "logo-small.png")


SPLASH_TEXT=""""Welcome to <b>Openstructure</b>!<br/><br/>
You are running version %s<br /><br />If you are new to OpenStructure, we 
invite you to run the demos from the examples directory. Scripts can be 
displayed by right clicking on the file and selecting 'Show source'.<br/><br/>
Feel free visit our website at:<br /> 
<a href='http://www.openstructure.org'>http://www.openstructure.org</a>
""" % ost.VERSION

class SplashDialog(QtGui.QDialog):
  def __init__(self, parent=None):
    QtGui.QDialog.__init__(self, parent)
    layout = QtGui.QHBoxLayout(self)
    self.setLayout(layout)
    imageLabel = QtGui.QLabel();
    self.pix_map = QtGui.QPixmap(LOGO_PATH);
    imageLabel.setPixmap(self.pix_map);
    layout.addWidget(imageLabel)
    self.label = QtGui.QTextBrowser()
    self.label.setReadOnly(True)
    self.label.setOpenExternalLinks(True)
    self.label.setHtml(SPLASH_TEXT)
    layout.addWidget(self.label)
    
def _InitSplash():
  splash = SplashDialog(gui.GostyApp.Instance().perspective.main_area.qobject)
  splash.exec_()

  #QtCore.QTimer.singleShot(30000, splash.close);


__all__=('SplashDialog',)
