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
from render_mode_widget import RenderModeWidget

#Tube Render Options
class TubeWidget(RenderModeWidget):
  def __init__(self, parent=None):
    RenderModeWidget.__init__(self, parent)
    
    #Title
    self.text_ = "Smooth Tube"
    
    #Set Render Mode
    self.mode_ = gfx.RenderMode.TUBE
        
    #Defaults
    min_spline_detail = 1
    max_spline_detail = 20
    
    min_arc_detail = 1
    max_arc_detail = 20
    
    min_radius = 0.1
    max_radius = 2

    min_ratio = 0.2
    max_ratio = 4
    
    #UI
    tube_label = QtGui.QLabel("Tube Settings")
    font = tube_label.font()
    font.setBold(True)
    
    #Poly Mode
    poly_mode_label = QtGui.QLabel("Poly Mode")
    
    self.poly_mode_cb_ = QtGui.QComboBox()
    self.poly_mode_cb_.addItem("Points")
    self.poly_mode_cb_.addItem("Wireframe")
    self.poly_mode_cb_.addItem("Surface")
    
    #Sphere Label
    spline_label = QtGui.QLabel("Spline Detail")
    
    self.spline_spinbox_ = QtGui.QSpinBox()
    self.spline_spinbox_.setRange(min_spline_detail, max_spline_detail)
    
    #Arc Label
    arc_label = QtGui.QLabel("Arc Detail")
    
    self.arc_spinbox_ = QtGui.QSpinBox()
    self.arc_spinbox_.setRange(min_arc_detail, max_arc_detail)
        
    #Radius
    radius_label = QtGui.QLabel("Tube radius")

    
    self.radius_spinbox_ = QtGui.QDoubleSpinBox()
    self.radius_spinbox_.setRange(min_radius, max_radius)
    self.radius_spinbox_.setDecimals(1)
    self.radius_spinbox_.setSingleStep(0.1)

    self.radius_slider_ = QtGui.QSlider(QtCore.Qt.Horizontal, self)
    self.radius_slider_.setRange(min_radius*10.0, max_radius*10.0)
    self.radius_slider_.setTickPosition(QtGui.QSlider.NoTicks)
    self.radius_slider_.setTickInterval(1)
    
    #Ratio
    ratio_label = QtGui.QLabel("Tube ratio")
    
    self.ratio_spinbox_ = QtGui.QDoubleSpinBox()
    self.ratio_spinbox_.setRange(min_ratio, max_ratio)
    self.ratio_spinbox_.setDecimals(1)
    self.ratio_spinbox_.setSingleStep(0.1)
    self.ratio_slider_ = QtGui.QSlider(QtCore.Qt.Horizontal, self)
    self.ratio_slider_.setRange(min_ratio*10.0, max_ratio*10.0)
    self.ratio_slider_.setTickPosition(QtGui.QSlider.NoTicks)
    self.ratio_slider_.setTickInterval(2)
    
    grid = QtGui.QGridLayout()
    grid.addWidget(tube_label,0,0,1,3)
    grid.addWidget(poly_mode_label,1,0,1,3)
    grid.addWidget(self.poly_mode_cb_,1,3,1,2)
    grid.addWidget(spline_label, 2, 0, 1, 3)
    grid.addWidget(self.spline_spinbox_, 2, 4, 1, 1)
    grid.addWidget(arc_label,3,0,1,3)
    grid.addWidget(self.arc_spinbox_,3,4,1,1)
    grid.addWidget(radius_label,4,0,1,1)
    grid.addWidget(self.radius_slider_,4,1,1,3)
    grid.addWidget(self.radius_spinbox_,4,4,1,1)
    grid.addWidget(ratio_label,5,0,1,1)
    grid.addWidget(self.ratio_slider_,5,1,1,3)
    grid.addWidget(self.ratio_spinbox_,5,4,1,1)
    grid.setRowStretch(6,1)
    self.setLayout(grid)
    
    QtCore.QObject.connect(self.spline_spinbox_, QtCore.SIGNAL("valueChanged(int)"), self.UpdateSplineDetail)
    QtCore.QObject.connect(self.arc_spinbox_, QtCore.SIGNAL("valueChanged(int)"), self.UpdateArcDetail)
    QtCore.QObject.connect(self.poly_mode_cb_, QtCore.SIGNAL("currentIndexChanged(int)"), self.UpdatePolyMode)
    QtCore.QObject.connect(self.radius_spinbox_, QtCore.SIGNAL("valueChanged(double)"), self.UpdateRadius)
    QtCore.QObject.connect(self.radius_slider_, QtCore.SIGNAL("valueChanged(int)"), self.UpdateSliderRadius)
    QtCore.QObject.connect(self.ratio_spinbox_, QtCore.SIGNAL("valueChanged(double)"), self.UpdateRatio)
    QtCore.QObject.connect(self.ratio_slider_, QtCore.SIGNAL("valueChanged(int)"), self.UpdateSliderRatio)
  
    self.setMinimumSize(250,200)
   
  def UpdateGui(self,options):
    self.poly_mode_cb_.setCurrentIndex(options.GetPolyMode())
    self.spline_spinbox_.setValue(options.GetSplineDetail())
    self.arc_spinbox_.setValue(options.GetArcDetail())
    self.UpdateRadiusGui(options.GetTubeRadius())
    self.UpdateRatioGui(options.GetTubeRatio())
   
  def UpdatePolyMode(self, value):
    self.GetOptions().SetPolyMode(value)
    self.ApplyOptions()
    
  def UpdateSplineDetail(self, value):
    self.GetOptions().SetSplineDetail(value)
    self.ApplyOptions()
        
  def UpdateArcDetail(self, value):
    self.GetOptions().SetArcDetail(value)
    self.ApplyOptions()
    
  def UpdateRadius(self, value):
    self.GetOptions().SetTubeRadius(value)
    self.ApplyOptions()
    
  def UpdateSliderRadius(self, value):
    self.GetOptions().SetTubeRadius(value/10.0)

  def UpdateRatio(self, value):
    self.GetOptions().SetTubeRatio(value)
    
  def UpdateSliderRatio(self, value):
    self.GetOptions().SetTubeRatio(value/10.0)

  def UpdateRadiusGui(self,value):
    if(abs(value*10.0 - self.radius_slider_.value())>=self.radius_spinbox_.singleStep()):
      self.radius_slider_.setValue(value*10.0)
    self.radius_spinbox_.setValue(value)
  
  def UpdateRatioGui(self,value):
    if(abs(value*10.0 - self.ratio_slider_.value())>=self.ratio_spinbox_.singleStep()):
      self.ratio_slider_.setValue(value*10.0)
    self.ratio_spinbox_.setValue(value)

  def GetText(self):
    return self.text_

  def GetRenderMode(self):
    return self.mode_
