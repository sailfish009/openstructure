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
# -*- coding: utf-8 -*-

import sys
from ost import gui
from ost import gfx
from PyQt4 import QtCore, QtGui
from combo_options_widget import ComboOptionsWidget
from custom_widget import CustomWidget
from cpk_widget import CPKWidget
from tube_widget import TubeWidget
from simple_widget import SimpleWidget
from sline_widget import SlineWidget
from hsc_widget import HSCWidget
from trace_widget import TraceWidget
from line_trace_widget import LineTraceWidget

class RenderOptionsWidget(ComboOptionsWidget):
  def __init__(self, parent=None):
    ComboOptionsWidget.__init__(self, parent)
    
    #Title
    self.text_ = "Render Options"
    
    self.keep_action_ = QtGui.QAction("K",self)
    self.keep_action_.setCheckable(True);
    self.keep_action_.setChecked(False)
    self.keep_action_.setToolTip("Keep rendermodes and add current")
    self.keep_button_ = QtGui.QToolButton(self)
    self.keep_button_.setDefaultAction(self.keep_action_)
    
    self.grid_layout_.addWidget(self.keep_button_, 0, 1, 1, 1)
    self.grid_layout_.addWidget(self.stacked_widget_, 1, 0, 1, 2)
    
    #Add options to menu    
    ComboOptionsWidget.AddWidget(self, "", EmptyMode())
    ComboOptionsWidget.AddWidget(self, gfx.RenderMode.SIMPLE, SimpleWidget(self))
    ComboOptionsWidget.AddWidget(self, gfx.RenderMode.CUSTOM, CustomWidget(self))
    ComboOptionsWidget.AddWidget(self, gfx.RenderMode.CPK, CPKWidget(self))
    ComboOptionsWidget.AddWidget(self, gfx.RenderMode.LINE_TRACE, LineTraceWidget(self))    
    ComboOptionsWidget.AddWidget(self, gfx.RenderMode.TRACE, TraceWidget(self))
    ComboOptionsWidget.AddWidget(self, gfx.RenderMode.SLINE, SlineWidget(self))
    ComboOptionsWidget.AddWidget(self, gfx.RenderMode.TUBE, TubeWidget(self))
    ComboOptionsWidget.AddWidget(self, gfx.RenderMode.HSC, HSCWidget(self))

    self.setMinimumSize(250,200)
    
  def DoSomething(self, item):
    scene_selection = gui.SceneSelection.Instance()
    for i in range(0,scene_selection.GetActiveNodeCount()):
      node = scene_selection.GetActiveNode(i)
      if isinstance(node, gfx.Entity):
        render_mode = item.GetRenderMode()
        if render_mode is not None:
          node.SetRenderMode(render_mode)     
    
    if(scene_selection.GetActiveViewCount() > 0):
      entity = scene_selection.GetViewEntity()
      for i in range(0,scene_selection.GetActiveViewCount()):
        view = scene_selection.GetActiveView(i)
        render_mode = item.GetRenderMode()
        if render_mode is not None:
          entity.SetRenderMode(item.GetRenderMode(),view,self.keep_action_.isChecked())
        
    item.Update()
    self.DoResize()
  
  def Update(self):
    if hasattr(self, "keep_button_"):
      self.keep_button_.setEnabled(True)
    scene_selection = gui.SceneSelection.Instance()
    if scene_selection.GetActiveNodeCount() == 0 and scene_selection.GetActiveViewCount() == 0:
      ComboOptionsWidget.setEnabled(self,False)
      return
    
    if scene_selection.GetActiveNodeCount() > 0 :
      if hasattr(self, "keep_button_"):
        self.keep_button_.setEnabled(False)
      render_mode_valid = True
      render_mode = None
      for i in range(0,scene_selection.GetActiveNodeCount()):
        entity = scene_selection.GetActiveNode(i)
        if isinstance(scene_selection.GetActiveNode(i), gfx.Entity):
          if render_mode is None:
            render_mode = entity.GetRenderMode()
          elif render_mode != entity.GetRenderMode():
            render_mode_valid = False
            break
        else:
          ComboOptionsWidget.setEnabled(self,False)
          return
      if(render_mode_valid):
        ComboOptionsWidget.ChangeSelectedItem(self,render_mode)
      else:
        ComboOptionsWidget.ChangeSelectedItem(self,"")
    
    self.GetCurrentWidget().Update()
    
    ComboOptionsWidget.setEnabled(self,True)
  
  def GetText(self):
    return self.text_
        
class EmptyMode(QtGui.QWidget):
  def __init__(self, parent=None):
    QtGui.QLabel.__init__(self, parent)
    self.setMinimumSize(250,30)
  
  def Update(self):
   True #Do Nothing

  def GetText(self):
    return ""

  def GetRenderMode(self):
    return None
