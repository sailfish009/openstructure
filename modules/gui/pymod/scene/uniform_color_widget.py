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

from ost import gui
from ost import gfx
from ost import mol
try: 
  from ost import img
  _img_present=True
except ImportError:
  _img_present=False
  pass
from PyQt4 import QtCore, QtGui
from color_select_widget import ColorSelectWidget

#Uniform Color Widget
class UniformColorWidget(QtGui.QWidget):
  def __init__(self, parent=None):
    QtGui.QWidget.__init__(self, parent)
    
    self.text_ = "Uniform Color"
    
    #Create Ui elements
    uniform_label = QtGui.QLabel(self.text_)
    font = uniform_label.font()
    font.setBold(True)
    
    
    self.color_select_widget_ = ColorSelectWidget(1,1,QtGui.QColor("White"))
    
    top_layout = QtGui.QVBoxLayout()
    
    grid = QtGui.QGridLayout()
    grid.addWidget(self.color_select_widget_, 2, 1, 1, 1)
    grid.setRowStretch(1, 1)
    grid.setRowStretch(3, 1)
    grid.setColumnStretch(0,1)
    grid.setColumnStretch(2,1)
    
    top_layout.addWidget(uniform_label)
    top_layout.addLayout(grid)
    self.setLayout(top_layout)
    
    QtCore.QObject.connect(self.color_select_widget_, QtCore.SIGNAL("colorChanged"), self.Update)
    
    self.setMinimumSize(250,150)

  def Update(self):
    scene_selection = gui.SceneSelection.Instance()
    for i in range(0,scene_selection.GetActiveNodeCount()):
      node = scene_selection.GetActiveNode(i)
      self.ChangeColor(node)
      
    if(scene_selection.GetActiveViewCount() > 0):
      entity = scene_selection.GetViewEntity()
      view = scene_selection.GetViewUnion()
      self.ChangeViewColor(entity,view)
        
  def ChangeColor(self, node):
    gfx_color = self.GetGfxColor()
    if isinstance(node, gfx.Entity) or isinstance(node, gfx.Surface):
      node.CleanColorOps()
      node.SetColor(gfx_color,"")
    elif _img_present and isinstance(node, gfx.MapIso):
      node.SetColor(gfx_color)
  
  def ChangeViewColor(self, entity, view):
    if isinstance(entity, gfx.Entity) and isinstance(view, mol.EntityView):
      gfx_color = self.GetGfxColor()
      ufco=gfx.UniformColorOp(mol.QueryViewWrapper(view),gfx_color)
      entity.Apply(ufco)
  
  def GetGfxColor(self):
    color = self.color_select_widget_.GetColor()
    return gfx.Color(color.redF(), color.greenF(), color.blueF())
  
  def resizeEvent(self, event):
    self.color_select_widget_.SetSize(self.width()/2,self.height()/2)
    
  def GetText(self):
    return self.text_
