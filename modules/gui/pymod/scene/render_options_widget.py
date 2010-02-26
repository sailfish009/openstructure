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
    
    #Add options to menu    
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
    if hasattr(self, "entities_") and isinstance(self.entities_, list):
      for entity in self.entities_:
        if isinstance(entity, gfx.Entity):
          entity.SetRenderMode(item.mode)
    self.DoResize()
  
  def Update(self):
    enable = False
    if hasattr(self, "entities_") and isinstance(self.entities_, list):
      entity = self.entities_[0];
      if isinstance(entity, gfx.Entity):
        mode = entity.GetRenderMode()
        ComboOptionsWidget.ChangeSelectedItem(self,mode)
        self.GetCurrentWidget().Update()
      for entity in self.entities_:
        if isinstance(entity, gfx.Entity):
          enable = True
        else:
          enable = False
          break
    ComboOptionsWidget.setEnabled(self,enable)  
  
  def GetText(self):
    return self.text_
        
#Helper Class which contains the rendering mode and a beautiful label
class LabelWithMode(QtGui.QWidget):
  def __init__(self, text, mode, parent=None):
    QtGui.QLabel.__init__(self, parent)
    
    #Title
    self.text_ = text
    
    #Mode
    self.mode = mode
    
    #UI
    textLabel = QtGui.QLabel(text)
    font = textLabel.font()
    font.setBold(True)
    
    grid = QtGui.QGridLayout()
    grid.addWidget(textLabel,0,0,1,1)
    grid.addWidget(QtGui.QLabel("No Settings available"), 1, 0, 1, 3)
    grid.setRowStretch(2,1)
    self.setLayout(grid)

  def SetEntities(self, entities):
    self.entities_ = entities
    
  def Update(self):
   True #Do Nothing

  def GetText(self):
    return self.text_

