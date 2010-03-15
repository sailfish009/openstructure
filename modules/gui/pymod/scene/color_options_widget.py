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
from gradient_editor_widget import GradientEditor
from uniform_color_widget import UniformColorWidget
from combo_options_widget import ComboOptionsWidget
from preset_widget import PresetWidget

class ColorOptionsWidget(ComboOptionsWidget):
  def __init__(self, parent=None):
    ComboOptionsWidget.__init__(self, parent)
    
    #Title
    self.text_ = "Color Options"
    
    #Add options to menu
    self.AddWidget("Presets", PresetWidget(self))
    self.AddWidget("Color by Element", ByElementWidget("Color by Element"))
    self.AddWidget("Color by Chain", ByChainWidget("Color by Chain"))
    self.AddWidget("Color by Property", GradientEditor(self))
    self.AddWidget("Uniform",UniformColorWidget(self))
  
    self.setMinimumSize(250,200)
    
  def DoSomething(self, item):
    scene_selection = gui.SceneSelection.Instance()
    for i in range(0,scene_selection.GetActiveNodeCount()):
      node = scene_selection.GetActiveNode(i)
      item.ChangeColor(node)
    self.DoResize()
    
    
  def Update(self):
    ComboOptionsWidget.setEnabled(self,True)
    scene_selection = gui.SceneSelection.Instance()
    if scene_selection.GetActiveNodeCount() == 0:
      ComboOptionsWidget.setEnabled(self,False)
      return
    
    for i in range(0,scene_selection.GetActiveNodeCount()):
      node = scene_selection.GetActiveNode(i)
      if not isinstance(node, gfx.Entity) or isinstance(node, gfx.Surface):
        ComboOptionsWidget.setEnabled(self,False)
        return

  def GetText(self):
    return self.text_
  

class ByElementWidget(QtGui.QWidget):
  def __init__(self, text, parent=None):
    QtGui.QLabel.__init__(self, parent)
    
    #Title
    self.text_ = text
    
    #UI
    text_label = QtGui.QLabel(text)
    font = text_label.font()
    font.setBold(True)
    
    grid = QtGui.QGridLayout()
    grid.addWidget(text_label,0,0,1,1)
    grid.addWidget(QtGui.QLabel("No Settings available"), 1, 0, 1, 3)
    grid.setRowStretch(2,1)
    self.setLayout(grid)
    self.setMinimumSize(250,60)
    
  def Update(self):
   pass #Do Nothing
    
  def ChangeColor(self, node):
    if isinstance(node, gfx.Entity):
      node.CleanColorOps()
      node.ColorByElement()
      
  def GetText(self):
    return self.text_
  
  
class ByChainWidget(QtGui.QWidget):
  def __init__(self, text, parent=None):
    QtGui.QLabel.__init__(self, parent)
    
    #Title
    self.text_ = text
    
    #UI
    text_label = QtGui.QLabel(text)
    font = text_label.font()
    font.setBold(True)
    
    grid = QtGui.QGridLayout()
    grid.addWidget(text_label,0,0,1,1)
    grid.addWidget(QtGui.QLabel("No Settings available"), 1, 0, 1, 3)
    grid.setRowStretch(2,1)
    self.setLayout(grid)
    self.setMinimumSize(250,60)
    
  def Update(self):
   pass #Do Nothing
    
  def ChangeColor(self, node):
    if isinstance(node, gfx.Entity):
      node.CleanColorOps()
      node.ColorByChain()
      
  def GetText(self):
    return self.text_
