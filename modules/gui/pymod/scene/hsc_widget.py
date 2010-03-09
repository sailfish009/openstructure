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
class HSCWidget(RenderModeWidget):
  def __init__(self, parent=None):
    RenderModeWidget.__init__(self, parent)
    
    #Title
    self.text_ = "Helix & Strand Cartoon"
     
    #Set Render Mode
    self.mode_ = gfx.RenderMode.HSC
    
    #Defaults
    min_spline_detail = 1
    max_spline_detail = 20
    
    min_arc_detail = 1
    max_arc_detail = 20
    
    min_width = 0.1
    max_width = 2.5
    max_tube_width= 2.0

    min_tube_ratio = 0.2
    max_tube_ratio = 4
    min_ratio = 0.1
    max_ratio = 2.5
    
    min_ecc = 0.1
    max_ecc = 5
    
    #########UI##########
    
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
    
    #Tube
    tube_label = QtGui.QLabel("Tube")
    font = tube_label.font()
    font.setBold(True)
    
    #Tube Radius
    radius_tube_label = QtGui.QLabel("Radius")
    
    self.width_tube_spinbox_ = QtGui.QDoubleSpinBox()
    self.width_tube_spinbox_.setRange(min_width, max_tube_width)
    self.width_tube_spinbox_.setDecimals(1)
    self.width_tube_spinbox_.setSingleStep(0.1)
    
    self.width_tube_slider_ = QtGui.QSlider(QtCore.Qt.Horizontal, self)
    self.width_tube_slider_.setRange(min_width*10.0, max_tube_width*10.0)
    self.width_tube_slider_.setTickPosition(QtGui.QSlider.NoTicks)
    self.width_tube_slider_.setTickInterval(1)
    
    #Tube Ratio
    ratio_tube_label = QtGui.QLabel("Ratio")
    
    self.thickness_tube_spinbox_ = QtGui.QDoubleSpinBox()
    self.thickness_tube_spinbox_.setRange(min_tube_ratio, max_tube_ratio)
    self.thickness_tube_spinbox_.setDecimals(1)
    self.thickness_tube_spinbox_.setSingleStep(0.1)
    
    self.thickness_tube_slider_ = QtGui.QSlider(QtCore.Qt.Horizontal, self)
    self.thickness_tube_slider_.setRange(min_tube_ratio*10.0, max_tube_ratio*10)
    self.thickness_tube_slider_.setTickPosition(QtGui.QSlider.NoTicks)
    self.thickness_tube_slider_.setTickInterval(1)
    
    #Helifx
    helix_label = QtGui.QLabel("Helix")
    font = helix_label.font()
    font.setBold(True)
    
    #Helix Radius
    radius_helix_label = QtGui.QLabel("Width")
    
    self.width_helix_spinbox_ = QtGui.QDoubleSpinBox()
    self.width_helix_spinbox_.setRange(min_width, max_width)
    self.width_helix_spinbox_.setDecimals(1)
    self.width_helix_spinbox_.setSingleStep(0.1)
    
    self.width_helix_slider_ = QtGui.QSlider(QtCore.Qt.Horizontal, self)
    self.width_helix_slider_.setRange(min_width*10.0, max_width*10.0)
    self.width_helix_slider_.setTickPosition(QtGui.QSlider.NoTicks)
    self.width_helix_slider_.setTickInterval(1)
    
    
    #Helix Ratio
    ratio_helix_label = QtGui.QLabel("Thickness")
    
    self.thickness_helix_spinbox_ = QtGui.QDoubleSpinBox()
    self.thickness_helix_spinbox_.setRange(min_ratio,max_ratio)
    self.thickness_helix_spinbox_.setDecimals(1)
    self.thickness_helix_spinbox_.setSingleStep(0.1)
    
    self.thickness_helix_slider_ = QtGui.QSlider(QtCore.Qt.Horizontal, self)
    self.thickness_helix_slider_.setRange(min_ratio*10.0,max_ratio*10.0)
    self.thickness_helix_slider_.setTickPosition(QtGui.QSlider.NoTicks)
    self.thickness_helix_slider_.setTickInterval(1)
    
    
    #Helix ECC
    ecc_helix_label = QtGui.QLabel("ECC")
    
    self.ecc_helix_spinbox_ = QtGui.QDoubleSpinBox()
    self.ecc_helix_spinbox_.setRange(min_ecc,max_ecc)
    self.ecc_helix_spinbox_.setDecimals(1)
    self.ecc_helix_spinbox_.setSingleStep(0.1)
    
    self.ecc_helix_slider_ = QtGui.QSlider(QtCore.Qt.Horizontal, self)
    self.ecc_helix_slider_.setRange(min_ecc*10,max_ecc*10)
    self.ecc_helix_slider_.setTickPosition(QtGui.QSlider.NoTicks)
    self.ecc_helix_slider_.setTickInterval(1)
    
    
    #Strand
    strand_label = QtGui.QLabel("Strand")
    font = strand_label.font()
    font.setBold(1)
    
    #Strand Radius
    radius_strand_label = QtGui.QLabel("Width")
    
    self.width_strand_spinbox_ = QtGui.QDoubleSpinBox()
    self.width_strand_spinbox_.setRange(min_width, max_width)
    self.width_strand_spinbox_.setDecimals(1)
    self.width_strand_spinbox_.setSingleStep(0.1)
    
    self.width_strand_slider_ = QtGui.QSlider(QtCore.Qt.Horizontal, self)
    self.width_strand_slider_.setRange(min_width*10, max_width*10)
    self.width_strand_slider_.setTickPosition(QtGui.QSlider.NoTicks)
    self.width_strand_slider_.setTickInterval(1)
    
    
    #Strand Ratio
    ratio_strand_label = QtGui.QLabel("Thickness")
    
    self.thickness_strand_spinbox_ = QtGui.QDoubleSpinBox()
    self.thickness_strand_spinbox_.setRange(min_ratio,max_ratio)
    self.thickness_strand_spinbox_.setDecimals(1)
    self.thickness_strand_spinbox_.setSingleStep(0.1)
    
    self.thickness_strand_slider_ = QtGui.QSlider(QtCore.Qt.Horizontal, self)
    self.thickness_strand_slider_.setRange(min_ratio*10,max_ratio*10)
    self.thickness_strand_slider_.setTickPosition(QtGui.QSlider.NoTicks)
    self.thickness_strand_slider_.setTickInterval(1)
    
    
    #Strand ECC
    ecc_strand_label = QtGui.QLabel("ECC")
    
    self.ecc_strand_spinbox_ = QtGui.QDoubleSpinBox()
    self.ecc_strand_spinbox_.setRange(min_ecc,max_ecc)
    self.ecc_strand_spinbox_.setDecimals(1)
    self.ecc_strand_spinbox_.setSingleStep(0.1)
    
    self.ecc_strand_slider_ = QtGui.QSlider(QtCore.Qt.Horizontal, self)
    self.ecc_strand_slider_.setRange(min_ecc*10,max_ecc*10)
    self.ecc_strand_slider_.setTickPosition(QtGui.QSlider.NoTicks)
    self.ecc_strand_slider_.setTickInterval(1)  
    
    grid = QtGui.QGridLayout()
    grid.addWidget(poly_mode_label,1,0,1,1)
    grid.addWidget(self.poly_mode_cb_,1,3,1,2)
    grid.addWidget(spline_label, 2, 0, 1, 3)
    grid.addWidget(self.spline_spinbox_, 2, 4, 1, 1)
    grid.addWidget(arc_label,3,0,1,3)
    grid.addWidget(self.arc_spinbox_,3,4,1,1)
    
    grid.addWidget(tube_label, 4, 0, 1, 3)
    grid.addWidget(radius_tube_label, 5, 0, 1, 1)
    grid.addWidget(self.width_tube_slider_, 5, 1, 1, 3)
    grid.addWidget(self.width_tube_spinbox_, 5, 4, 1, 1)
    grid.addWidget(ratio_tube_label, 6, 0, 1, 1)
    grid.addWidget(self.thickness_tube_slider_, 6, 1, 1, 3)
    grid.addWidget(self.thickness_tube_spinbox_, 6, 4, 1, 1)
    
    grid.addWidget(helix_label, 7, 0, 1, 3)
    grid.addWidget(radius_helix_label, 8, 0, 1, 1)
    grid.addWidget(self.width_helix_slider_, 8, 1, 1, 3)
    grid.addWidget(self.width_helix_spinbox_, 8, 4, 1, 1)
    grid.addWidget(ratio_helix_label, 9, 0, 1, 1)
    grid.addWidget(self.thickness_helix_slider_, 9, 1, 1, 3)
    grid.addWidget(self.thickness_helix_spinbox_, 9, 4, 1, 1)
    grid.addWidget(ecc_helix_label, 10, 0, 1, 1)
    grid.addWidget(self.ecc_helix_slider_, 10, 1, 1, 3)
    grid.addWidget(self.ecc_helix_spinbox_, 10, 4, 1, 1)
   
    grid.addWidget(strand_label, 11, 0, 1, 3)
    grid.addWidget(radius_strand_label, 12, 0, 1, 1)
    grid.addWidget(self.width_strand_slider_, 12, 1, 1, 3)
    grid.addWidget(self.width_strand_spinbox_, 12, 4, 1, 1)
    grid.addWidget(ratio_strand_label, 13, 0, 1, 1)
    grid.addWidget(self.thickness_strand_slider_, 13, 1, 1, 3)
    grid.addWidget(self.thickness_strand_spinbox_, 13, 4, 1, 1)
    grid.addWidget(ecc_strand_label, 14, 0, 1, 1)
    grid.addWidget(self.ecc_strand_slider_, 14, 1, 1, 3)
    grid.addWidget(self.ecc_strand_spinbox_, 14, 4, 1, 1)
    
    grid.setRowStretch(15,1)
    self.setLayout(grid)
    
    QtCore.QObject.connect(self.spline_spinbox_, QtCore.SIGNAL("valueChanged(int)"), self.UpdateSplineDetail)
    QtCore.QObject.connect(self.arc_spinbox_, QtCore.SIGNAL("valueChanged(int)"), self.UpdateArcDetail)
    QtCore.QObject.connect(self.poly_mode_cb_, QtCore.SIGNAL("currentIndexChanged(int)"), self.UpdatePolyMode)
    
    QtCore.QObject.connect(self.width_tube_spinbox_, QtCore.SIGNAL("valueChanged(double)"), self.UpdateTubeRadius)
    QtCore.QObject.connect(self.width_tube_slider_, QtCore.SIGNAL("valueChanged(int)"), self.UpdateSliderTubeRadius)
    QtCore.QObject.connect(self.thickness_tube_spinbox_, QtCore.SIGNAL("valueChanged(double)"), self.UpdateTubeRatio)
    QtCore.QObject.connect(self.thickness_tube_slider_, QtCore.SIGNAL("valueChanged(int)"), self.UpdateSliderTubeRatio)
    
    QtCore.QObject.connect(self.width_helix_spinbox_, QtCore.SIGNAL("valueChanged(double)"), self.UpdateHelixWidth)
    QtCore.QObject.connect(self.width_helix_slider_, QtCore.SIGNAL("valueChanged(int)"), self.UpdateSliderHelixWidth)
    QtCore.QObject.connect(self.thickness_helix_spinbox_, QtCore.SIGNAL("valueChanged(double)"), self.UpdateHelixThickness)
    QtCore.QObject.connect(self.thickness_helix_slider_, QtCore.SIGNAL("valueChanged(int)"), self.UpdateSliderHelixThickness)
    QtCore.QObject.connect(self.ecc_helix_spinbox_, QtCore.SIGNAL("valueChanged(double)"), self.UpdateHelixEcc)
    QtCore.QObject.connect(self.ecc_helix_slider_, QtCore.SIGNAL("valueChanged(int)"), self.UpdateSliderHelixEcc)

    QtCore.QObject.connect(self.width_strand_spinbox_, QtCore.SIGNAL("valueChanged(double)"), self.UpdateStrandWidth)
    QtCore.QObject.connect(self.width_strand_slider_, QtCore.SIGNAL("valueChanged(int)"), self.UpdateSliderStrandWidth)
    QtCore.QObject.connect(self.thickness_strand_spinbox_, QtCore.SIGNAL("valueChanged(double)"), self.UpdateStrandThickness)
    QtCore.QObject.connect(self.thickness_strand_slider_, QtCore.SIGNAL("valueChanged(int)"), self.UpdateSliderStrandThickness)
    QtCore.QObject.connect(self.ecc_strand_spinbox_, QtCore.SIGNAL("valueChanged(double)"), self.UpdateStrandEcc)
    QtCore.QObject.connect(self.ecc_strand_slider_, QtCore.SIGNAL("valueChanged(int)"), self.UpdateSliderStrandEcc)    
    
    self.setMinimumSize(250,420) #14*30
    ########/UI########
  
  def UpdateGui(self,options):
    self.poly_mode_cb_.setCurrentIndex(options.GetPolyMode())
    self.spline_spinbox_.setValue(options.GetSplineDetail())
    self.arc_spinbox_.setValue(options.GetArcDetail())
    
    self.UpdateTubeRadiusGui(options.GetTubeRadius())
    self.UpdateTubeRatioGui(options.GetTubeRatio())
    self.UpdateHelixWidthGui(options.GetHelixWidth())
    self.UpdateHelixThicknessGui(options.GetHelixThickness())
    self.UpdateHelixEccGui(options.GetHelixEcc())
    self.UpdateStrandWidthGui(options.GetStrandWidth())
    self.UpdateStrandThicknessGui(options.GetStrandThickness())
    self.UpdateStrandEccGui(options.GetStrandEcc())
    
  def UpdatePolyMode(self, value):
    self.GetOptions().SetPolyMode(value)
    
  def UpdateSplineDetail(self, value):
    self.GetOptions().SetSplineDetail(value)
    
  def UpdateArcDetail(self, value):
    self.GetOptions().SetArcDetail(value)
    
  def UpdateTubeRadius(self, value):
    self.GetOptions().SetTubeRadius(value)
    
  def UpdateSliderTubeRadius(self, value):
    self.GetOptions().SetTubeRadius(value/10.0)

  def UpdateTubeRatio(self, value):
    self.GetOptions().SetTubeRatio(value)

  def UpdateSliderTubeRatio(self, value):
    self.GetOptions().SetTubeRatio(value/10.0)
    
  def UpdateHelixWidth(self, value):
    self.GetOptions().SetHelixWidth(value) 
    
  def UpdateSliderHelixWidth(self, value):
    self.GetOptions().SetHelixWidth(value/10.0)
    
  def UpdateHelixThickness(self, value):
    self.GetOptions().SetHelixThickness(value)

  def UpdateSliderHelixThickness(self, value):
    self.GetOptions().SetHelixThickness(value/10.0)
    
  def UpdateHelixEcc(self, value):
    self.GetOptions().SetHelixEcc(value)
    
  def UpdateSliderHelixEcc(self, value):
    self.GetOptions().SetHelixEcc(value/10.0)
    
  def UpdateStrandWidth(self, value):
    self.GetOptions().SetStrandWidth(value)

  def UpdateSliderStrandWidth(self, value):
    self.GetOptions().SetStrandWidth(value/10.0)
    
  def UpdateStrandThickness(self, value):
    self.GetOptions().SetStrandThickness(value)
    
  def UpdateSliderStrandThickness(self, value):
    self.GetOptions().SetStrandThickness(value/10.0)
    
  def UpdateStrandEcc(self, value):
    self.GetOptions().SetStrandEcc(value)
    
  def UpdateSliderStrandEcc(self, value):
    self.GetOptions().SetStrandEcc(value/10.0)

  def UpdateTubeRadiusGui(self,value):
    if(abs(value*10.0 - self.width_tube_slider_.value())>=self.width_tube_spinbox_.singleStep()):
      self.width_tube_slider_.setValue(value*10.0)
    self.width_tube_spinbox_.setValue(value)
  
  def UpdateTubeRatioGui(self,value):
    if(abs(value*10.0 - self.thickness_tube_slider_.value())>=self.thickness_tube_spinbox_.singleStep()):
      self.thickness_tube_slider_.setValue(value*10.0)
    self.thickness_tube_spinbox_.setValue(value)

  def UpdateHelixWidthGui(self, value):
    if(abs(value*10.0 - self.width_helix_slider_.value())>=self.width_helix_spinbox_.singleStep()):
      self.width_helix_slider_.setValue(value*10.0)
    self.width_helix_spinbox_.setValue(value)
    
  def UpdateHelixThicknessGui(self, value):
    if(abs(value*10.0 - self.thickness_helix_slider_.value())>=self.thickness_helix_spinbox_.singleStep()):
      self.thickness_helix_slider_.setValue(value*10.0)
    self.thickness_helix_spinbox_.setValue(value)
    
  def UpdateHelixEccGui(self, value):
    if(abs(value*10.0 - self.ecc_helix_slider_.value())>=self.ecc_helix_spinbox_.singleStep()):
      self.ecc_helix_slider_.setValue(value*10.0)
    self.ecc_helix_spinbox_.setValue(value)
    
  def UpdateStrandWidthGui(self, value):
    if(abs(value*10.0 - self.width_strand_slider_.value())>=self.width_strand_spinbox_.singleStep()):
      self.width_strand_slider_.setValue(value*10.0)
    self.width_strand_spinbox_.setValue(value)

  def UpdateStrandThicknessGui(self, value):
    if(abs(value*10.0 - self.thickness_strand_slider_.value())>=self.thickness_strand_spinbox_.singleStep()):
      self.thickness_strand_slider_.setValue(value*10.0)
    self.thickness_strand_spinbox_.setValue(value)
    
  def UpdateStrandEccGui(self, value):
    if(abs(value*10.0 - self.ecc_strand_slider_.value())>=self.ecc_strand_spinbox_.singleStep()):
      self.ecc_strand_slider_.setValue(value*10.0)
    self.ecc_strand_spinbox_.setValue(value)

  def GetText(self):
    return self.text_

  def GetRenderMode(self):
    return self.mode_
