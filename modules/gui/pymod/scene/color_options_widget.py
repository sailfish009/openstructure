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
    options = {
               "Color by Property":GradientEditor(self),
               "Color by Element":LabelWithMode("Color by Element"),
               "Uniform":UniformColorWidget(self),
               "Presets":PresetWidget(self)
               }
    
    for k, v in options.iteritems():
      ComboOptionsWidget.AddWidget(self, k, v)
  
    self.setMinimumSize(250,200)
    
  def DoSomething(self, item):
    if hasattr(self, "entities_") and isinstance(self.entities_, list):
      item.ChangeColor()
    self.DoResize()
    
    
  def Update(self):
    enable = False
    if hasattr(self, "entities_") and isinstance(self.entities_, list):
      for entity in self.entities_:
        if isinstance(entity, gfx.Entity) or isinstance(entity, gfx.Surface):
          enable = True
        else:
          enable = False
          break
    ComboOptionsWidget.setEnabled(self,enable)  
    
  def GetText(self):
    return self.text_
  
#Helper Class which contains the rendering mode
class LabelWithMode(QtGui.QWidget):
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
   True #Do Nothing
    
  def SetEntities(self,entities):
    self.entities_ = entities

  def ChangeColor(self):
    if self.text_ == "Color by Element":
      for entity in self.entities_:
        if isinstance(entity, gfx.Entity):
          entity.CleanColorOps()
          entity.ColorByElement()
      
  def GetText(self):
    return self.text_
