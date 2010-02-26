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

#CPK Render Options
class CPKWidget(QtGui.QWidget):
  def __init__(self, parent=None):
    QtGui.QWidget.__init__(self, parent)
    
    #Title
    self.text_ = "Spheres"
    
    #Defaults
    min_sphere_detail = 1
    max_sphere_detail = 20
    
    #Set Render Mode
    self.mode = gfx.RenderMode.CPK
        
    #Create Ui elements
    
    self.sphere_spinbox_ = QtGui.QSpinBox()
    self.sphere_spinbox_.setRange(min_sphere_detail, max_sphere_detail)
    
    cpk_label = QtGui.QLabel(self.text_)
    font = cpk_label.font()
    font.setBold(True)
    
    sphere_label = QtGui.QLabel("Sphere Detail")
    grid = QtGui.QGridLayout()
    grid.addWidget(cpk_label,0,0,1,1)
    grid.addWidget(sphere_label, 1, 0, 1, 3)
    grid.addWidget(self.sphere_spinbox_, 1, 2, 1, 1)
    grid.setRowStretch(2,1)
    self.setLayout(grid)
  
    self.options_ = gfx.CPKRenderOptions()
    QtCore.QObject.connect(self.sphere_spinbox_, QtCore.SIGNAL("valueChanged(int)"), self.UpdateSphereDetail)
    
    self.setMinimumSize(250,60)
    
  def UpdateSphereDetail(self, value):
    self.options_.SetSphereDetail(value)
    
  def Update(self):
    new_options = self.entities_[0].GetOptions(gfx.CPK)
    if self.options_ != new_options:
      self.options_ = new_options
    self.sphere_spinbox_.setValue(self.options_.GetSphereDetail())

  def SetEntities(self, entities):
    self.entities_ = entities

  def GetText(self):
    return self.text_
