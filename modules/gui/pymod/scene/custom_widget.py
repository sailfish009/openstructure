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
from PyQt4 import QtCore, QtGui

#Custom Render Options
class CustomWidget(QtGui.QWidget):
  def __init__(self, parent=None):
    QtGui.QWidget.__init__(self, parent)
    
    #Title
    self.text_ = "Ball & Stick"
    
    #Defaults
    min_sphere_detail = 1
    max_sphere_detail = 20
    
    min_arc_detail = 1
    max_arc_detail = 20
    
    #Set Render Mode
    self.mode = gfx.RenderMode.CUSTOM
        
    #Create Ui elements
    
    self.sphere_spinbox_ = QtGui.QSpinBox()
    self.sphere_spinbox_.setRange(min_sphere_detail, max_sphere_detail)
    
    self.arc_spinbox_ = QtGui.QSpinBox()
    self.arc_spinbox_.setRange(min_arc_detail, max_arc_detail)
    
    custom_label = QtGui.QLabel(self.text_)
    font = custom_label.font()
    font.setBold(True)
    
    sphere_label = QtGui.QLabel("Sphere Detail")
    arc_label = QtGui.QLabel("Arc Detail")
    grid = QtGui.QGridLayout()
    grid.addWidget(custom_label,0,0,1,1)
    grid.addWidget(sphere_label, 1, 0, 1, 3)
    grid.addWidget(self.sphere_spinbox_, 1, 2, 1, 1)
    grid.addWidget(arc_label,2,0,1,3)
    grid.addWidget(self.arc_spinbox_,2,2,1,1)
    grid.setRowStretch(3,1)
    self.setLayout(grid)
  
    self.options_ = gfx.CustomRenderOptions()
    QtCore.QObject.connect(self.sphere_spinbox_, QtCore.SIGNAL("valueChanged(int)"), self.UpdateSphereDetail)
    QtCore.QObject.connect(self.arc_spinbox_, QtCore.SIGNAL("valueChanged(int)"), self.UpdateArcDetail)
    
    self.setMinimumSize(250,90)
    
  def UpdateSphereDetail(self, value):
    self.options_.SetSphereDetail(value)
    
  def UpdateArcDetail(self, value):
    self.options_.SetArcDetail(value)

  def Update(self):
    new_options = self.entities_[0].GetOptions(gfx.CUSTOM)
    if self.options_ != new_options:
      self.options_ = new_options
    self.sphere_spinbox_.setValue(self.options_.GetSphereDetail())    
    self.arc_spinbox_.setValue(self.options_.GetArcDetail())

  def SetEntities(self, entities):
    self.entities_ = entities

  def GetText(self):
    return self.text_
