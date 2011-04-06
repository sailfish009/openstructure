#------------------------------------------------------------------------------
# This file is part of the OpenStructure project <www.openstructure.org>
#
# Copyright (C) 2008-2011 by the OpenStructure authors
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

import sys
from ost import gui
import sip
from ost import gfx
import ost
import os
from PyQt4 import QtCore, QtGui
from toolbar_options_widget import ToolBarOptionsWidget 
from render_options_widget import RenderOptionsWidget
from color_options_widget import ColorOptionsWidget
from ost.gui.scene.scene_observer_impl import SceneObserverImpl
from map_level_widget import AdditionalSettingsWidget
from scene_selection_helper import SelHelper

class InspectorWidget(ToolBarOptionsWidget):
  ICONS_PATH = os.path.join(ost.GetSharedDataPath(), "scene", "icons/")
  def __init__(self, parent=None):
    ToolBarOptionsWidget.__init__(self, parent)
    app=gui.GostyApp.Instance()
    options = [
                [InspectorWidget.ICONS_PATH+"render_icon.png",RenderOptionsWidget(self),None], 
                [InspectorWidget.ICONS_PATH+"color_icon.png",ColorOptionsWidget(self),None],
                [InspectorWidget.ICONS_PATH+"preset_icon.png", AdditionalSettingsWidget(self),"Additional Node Settings"],
                [InspectorWidget.ICONS_PATH+"tool_icon.png",app.tool_options_win.qobject,"Tool Options"]
              ]
    for o in options:
      ToolBarOptionsWidget.AddWidget(self,o[0], o[1], o[2])
    
    self.obs = SceneObserverImpl()
    self.obs.AttachObserver(self)
    ost.scene.AttachObserver(self.obs)
    QtCore.QObject.connect(app.scene_win.qobject,QtCore.SIGNAL("ActiveNodesChanged()"),
                           self.ActiveNodesChanged)     
    
    self.setMinimumSize(250,215)
  #ToolBarOptionsWidget Method
  def OnComboChange(self, item):
    self.DoResize()
        
  #Observer Methods    
  def NodeRemoved(self, node):
    SelHelper().Update()
    ToolBarOptionsWidget.Update(self)  
  
  def RenderModeChanged(self, node):
    SelHelper().Update()
    ToolBarOptionsWidget.Update(self)
   
  def NodeChanged(self, node):
    SelHelper().Update()
    ToolBarOptionsWidget.Update(self)

  def ActiveNodesChanged(self):
    SelHelper().Update()
    ToolBarOptionsWidget.Update(self)

class InspectorDialog(QtGui.QDialog):
  def __init__(self, parent=None):
    QtGui.QDialog.__init__(self, parent)
    self.setWindowTitle("Inspector Gadget")
    self.setAttribute(QtCore.Qt.WA_MacSmallSize)    
    self.layout=QtGui.QHBoxLayout()
    self.layout.setMargin(0)
    self.layout.setSpacing(0)
    self.setLayout(self.layout)
    self.mywidget_ = InspectorWidget(self)
    self.layout.addWidget(self.mywidget_)
    size_pol = QtGui.QSizePolicy(QtGui.QSizePolicy.Expanding,
                                 QtGui.QSizePolicy.Expanding)
    self.setSizePolicy(size_pol)
    self.DoResize()
    
  def DoResize(self):
    if(hasattr(self, "mywidget_")):
      self.setMinimumSize(self.mywidget_.minimumWidth(),
                          self.mywidget_.minimumHeight())
      self.resize(self.mywidget_.minimumWidth(),
                  self.mywidget_.minimumHeight())
    
  def ToggleHide(self,checked):
    self.setHidden(not self.isHidden())

  def hideEvent(self, event):
    self.emit(QtCore.SIGNAL("visible"),False)
    QtGui.QDialog.hideEvent(self,event)
    
  def showEvent(self, event):
    self.emit(QtCore.SIGNAL("visible"),True)
    QtGui.QDialog.showEvent(self,event)