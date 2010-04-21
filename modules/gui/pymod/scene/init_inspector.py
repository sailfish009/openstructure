#------------------------------------------------------------------------------
# This file is part of the OpenStructure project <www.openstructure.org>
#
# Copyright (C) 2008-2010 by the OpenStructure authors
#
# This library is free software; you can redistribute it and/or modify it under
# the terms of the GNU Lesser General Public License as published by the Free
# Software Foundation; either version 3.0 of the License, or (at your option)
# any later version.
# This library is distributed in the hope that it will be useful, but WITHOUT
# ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
# FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public License for more
# details.
#
# You should have received a copy of the GNU Lesser General Public License
# along with this library; if not, write to the Free Software Foundation, Inc.,
# 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
#------------------------------------------------------------------------------

import sip
from ost import gui
from ost import gfx
from PyQt4 import QtCore, QtGui
from ost.gui.scene.inspector_widget import InspectorDialog

class InitInspectorMenu(QtCore.QObject):
  def __init__(self, inspectordialog, menu_bar=None):
    QtCore.QObject.__init__(self, menu_bar)
    
    self.inspector_ = inspectordialog
    persp=gui.GostyApp.Instance().perspective
    options=persp.GetMenu("Options")
    
    self.show_ = options.addAction("&Inspector gadget")
    self.show_.setShortcut('Ctrl+I')
    self.show_.setCheckable(True)
    self.show_.setChecked(not self.inspector_.isHidden())
    self.connect(self.show_, QtCore.SIGNAL('triggered()'), self.Toggle)
    
  def Toggle(self):
    self.inspector_.setVisible(self.show_.isChecked())
    
def _InitInspector():
  app=gui.GostyApp.Instance()
  mywidget = InspectorDialog(app.gl_win.qobject)
  mywidget.setWindowFlags(QtCore.Qt.Dialog | QtCore.Qt.Tool)
  mywidget.show()
  unwrapped = gui.BPQtHandle(sip.unwrapinstance(mywidget))
  app.AddWidgetToApp("InspectorDialog", unwrapped)  
  menu_bar=app.perspective.GetMenuBar()
  InitInspectorMenu(mywidget,menu_bar)

