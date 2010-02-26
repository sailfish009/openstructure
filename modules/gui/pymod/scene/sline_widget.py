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

#Simple Render Options
class SlineWidget(QtGui.QWidget):
  def __init__(self, parent=None):
    QtGui.QWidget.__init__(self, parent)
    
    #Title
    self.text_ = "Fast Spline"
    
    #Set Render Mode
    self.mode = gfx.RenderMode.SLINE
    
    #Defaults
    min_detail = 1
    max_detail = 20
    
    min_line_width = 0.01
    max_line_width = 20
        
    #Create Ui elements
    self.detail_spinbox_ = QtGui.QSpinBox()
    self.detail_spinbox_.setRange(min_detail, max_detail)
    self.detail_spinbox_.setSingleStep(1)
    
    self.aa_rendering_cb_ = QtGui.QCheckBox()
    
    self.radius_spinbox_ = QtGui.QDoubleSpinBox()
    self.radius_spinbox_.setRange(min_line_width, max_line_width)
    self.radius_spinbox_.setDecimals(2)
    self.radius_spinbox_.setSingleStep(0.1)
    
    sline_label = QtGui.QLabel("Spline Settings")
    font = sline_label.font()
    font.setBold(True)
    
    detail_label = QtGui.QLabel("Spline Detail")
    aa_label = QtGui.QLabel("AA-Lines")
    radius_label = QtGui.QLabel("Line Width")
    grid = QtGui.QGridLayout()
    grid.addWidget(sline_label, 0, 0, 1, 1)
    grid.addWidget(detail_label, 1, 0, 1, 3)
    grid.addWidget(self.detail_spinbox_, 1, 2, 1, 1)
    grid.addWidget(aa_label, 2, 0, 1, 3)
    grid.addWidget(self.aa_rendering_cb_, 2, 2, 1, 1)
    grid.addWidget(radius_label, 3, 0, 1, 3)
    grid.addWidget(self.radius_spinbox_, 3, 2, 1, 1)
    grid.setRowStretch(4,1)
    self.setLayout(grid)

    self.options_ = gfx.SlineRenderOptions()
    QtCore.QObject.connect(self.detail_spinbox_, QtCore.SIGNAL("valueChanged(int)"), self.UpdateDetail)
    QtCore.QObject.connect(self.aa_rendering_cb_, QtCore.SIGNAL("stateChanged(int)"), self.UpdateAA)
    QtCore.QObject.connect(self.radius_spinbox_, QtCore.SIGNAL("valueChanged(double)"), self.UpdateLineWidth)

    self.setMinimumSize(250,120)
  
  def UpdateDetail(self, value):
    self.options_.SetSplineDetail(value)
  
  def UpdateAA(self, value):
    self.options_.SetAALines(value)
    
  def UpdateLineWidth(self, value):
    self.options_.SetLineWidth(value)
    
  def Update(self):
    new_options = self.entities_[0].GetOptions(gfx.SLINE)
    if self.options_ != new_options:
      self.options_ = new_options
    self.detail_spinbox_.setValue(self.options_.GetSplineDetail())
    self.aa_rendering_cb_.setChecked(self.options_.GetAALines())    
    self.radius_spinbox_.setValue(self.options_.GetLineWidth())
    

  def SetEntities(self, entities):
    self.entities_ = entities

  def GetText(self):
    return self.text_
