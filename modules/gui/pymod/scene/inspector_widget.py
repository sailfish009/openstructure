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

class InspectorWidget(ToolBarOptionsWidget):
  ICONS_PATH = os.path.join(ost.GetSharedDataPath(), "scene", "icons/")
  def __init__(self, parent=None):
    ToolBarOptionsWidget.__init__(self, parent)
    app=gui.GostyApp.Instance()
    options = [
                [InspectorWidget.ICONS_PATH+"render_icon.png",RenderOptionsWidget(self)], 
                [InspectorWidget.ICONS_PATH+"color_icon.png",ColorOptionsWidget(self)],
                [InspectorWidget.ICONS_PATH+"tool_icon.png",sip.wrapinstance(app.GetToolOptionsWin().GetSipHandle(),QtGui.QWidget)]
              ]
    for o in options:
      ToolBarOptionsWidget.AddWidget(self,o[0], o[1])
    
    self.obs = SceneObserverImpl()
    self.obs.AttachObserver(self)
    ost.scene.AttachObserver(self.obs)    
    app=gui.GostyApp.Instance()
    self.scene_selection_ = gui.SceneSelection.Instance()
    scenewin = sip.wrapinstance(app.GetSceneWin().GetSipHandle(),QtGui.QWidget)
    QtCore.QObject.connect(scenewin,QtCore.SIGNAL("ActiveNodesChanged(gfx::NodePtrList,gfx::EntityP,mol::QueryViewWrapperList)"),
                           self.EntitySelectionChanged)     
    
    self.setMinimumSize(250,215)
  #ToolBarOptionsWidget Method
  def DoSomething(self, item):
    self.DoResize()
        
  #Observer Methods    
  def NodeRemoved(self, node):
    self.SetEntities(None)
    ToolBarOptionsWidget.Update(self)  
  
  def RenderModeChanged(self, node):
    ToolBarOptionsWidget.Update(self)
   
  def NodeChanged(self, node):
    ToolBarOptionsWidget.Update(self)

  def EntitySelectionChanged(self):
    node_count = self.scene_selection_.GetActiveNodeCount()
    if(node_count > 0):
      entities = list();
      for i in range(node_count):
        entities.append(self.scene_selection_.GetActiveNode(i))
      self.SetEntities(entities)
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
    size_pol = QtGui.QSizePolicy(QtGui.QSizePolicy.Expanding,QtGui.QSizePolicy.Expanding)
    self.setSizePolicy(size_pol)
    self.DoResize()
    
  def DoResize(self):
    if(hasattr(self, "mywidget_")):
      self.setMinimumSize(self.mywidget_.minimumWidth(),self.mywidget_.minimumHeight())
      self.resize(self.mywidget_.minimumWidth(),self.mywidget_.minimumHeight())
    
  def ToggleHide(self,checked):
    self.setHidden(not self.isHidden())
