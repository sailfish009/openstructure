#------------------------------------------------------------------------------
# This file is part of the OpenStructure project <www.openstructure.org>
#
# Copyright (C) 2008-2011 by the OpenStructure authors
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

    min_profile=0
    max_profile=4
    
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

    # Tube Profile Type
    tube_profile_label = QtGui.QLabel("Tube Profile Type")
    self.tube_profile_spinbox_ = QtGui.QSpinBox()
    self.tube_profile_spinbox_.setRange(min_profile, max_profile)
    
    # Helix
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
    
    # Helix Profile Type
    helix_profile_label = QtGui.QLabel("Helix Profile Type")
    self.helix_profile_spinbox_ = QtGui.QSpinBox()
    self.helix_profile_spinbox_.setRange(min_profile, max_profile)
    
    # Helix Rendering Mode
    helix_mode_label = QtGui.QLabel("Helix Render Mode")
    self.helix_mode_spinbox_ = QtGui.QSpinBox()
    self.helix_mode_spinbox_.setRange(0, 1)
    
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
    
    # Strand Profile Type
    strand_profile_label = QtGui.QLabel("Strand Profile Type")
    self.strand_profile_spinbox_ = QtGui.QSpinBox()
    self.strand_profile_spinbox_.setRange(min_profile, max_profile)

    # Strand Rendering Mode
    strand_mode_label = QtGui.QLabel("Strand Render Mode")
    self.strand_mode_spinbox_ = QtGui.QSpinBox()
    self.strand_mode_spinbox_.setRange(0, 1)

    #
    row=1
    grid = QtGui.QGridLayout()
    grid.addWidget(poly_mode_label,row,0,1,1)
    grid.addWidget(self.poly_mode_cb_,row,3,1,2)
    row+=1
    grid.addWidget(spline_label, row, 0, 1, 3)
    grid.addWidget(self.spline_spinbox_, row, 4, 1, 1)
    row+=1
    grid.addWidget(arc_label,row,0,1,3)
    grid.addWidget(self.arc_spinbox_,row,4,1,1)
    row+=1
    grid.addWidget(tube_label, row, 0, 1, 3)
    row+=1
    grid.addWidget(radius_tube_label, row, 0, 1, 1)
    grid.addWidget(self.width_tube_slider_, row, 1, 1, 3)
    grid.addWidget(self.width_tube_spinbox_, row, 4, 1, 1)
    row+=1
    grid.addWidget(ratio_tube_label, row, 0, 1, 1)
    grid.addWidget(self.thickness_tube_slider_, row, 1, 1, 3)
    grid.addWidget(self.thickness_tube_spinbox_, row, 4, 1, 1)
    row+=1
    grid.addWidget(tube_profile_label, row, 0, 1, 3)
    grid.addWidget(self.tube_profile_spinbox_, row, 4, 1, 1)
    row+=1
    grid.addWidget(helix_label, row, 0, 1, 3)
    row+=1
    grid.addWidget(radius_helix_label, row, 0, 1, 1)
    grid.addWidget(self.width_helix_slider_, row, 1, 1, 3)
    grid.addWidget(self.width_helix_spinbox_, row, 4, 1, 1)
    row+=1
    grid.addWidget(ratio_helix_label, row, 0, 1, 1)
    grid.addWidget(self.thickness_helix_slider_, row, 1, 1, 3)
    grid.addWidget(self.thickness_helix_spinbox_, row, 4, 1, 1)
    row+=1
    grid.addWidget(ecc_helix_label, row, 0, 1, 1)
    grid.addWidget(self.ecc_helix_slider_, row, 1, 1, 3)
    grid.addWidget(self.ecc_helix_spinbox_, row, 4, 1, 1)
    row+=1
    grid.addWidget(helix_profile_label, row, 0, 1, 3)
    grid.addWidget(self.helix_profile_spinbox_, row, 4, 1, 1)
    row+=1
    grid.addWidget(helix_mode_label, row, 0, 1, 3)
    grid.addWidget(self.helix_mode_spinbox_, row, 4, 1, 1)
    row+=1
    grid.addWidget(strand_label, row, 0, 1, 3)
    row+=1
    grid.addWidget(radius_strand_label, row, 0, 1, 1)
    grid.addWidget(self.width_strand_slider_, row, 1, 1, 3)
    grid.addWidget(self.width_strand_spinbox_, row, 4, 1, 1)
    row+=1
    grid.addWidget(ratio_strand_label, row, 0, 1, 1)
    grid.addWidget(self.thickness_strand_slider_, row, 1, 1, 3)
    grid.addWidget(self.thickness_strand_spinbox_, row, 4, 1, 1)
    row+=1
    grid.addWidget(ecc_strand_label, row, 0, 1, 1)
    grid.addWidget(self.ecc_strand_slider_, row, 1, 1, 3)
    grid.addWidget(self.ecc_strand_spinbox_, row, 4, 1, 1)
    row+=1
    grid.addWidget(strand_profile_label, row, 0, 1, 3)
    grid.addWidget(self.strand_profile_spinbox_, row, 4, 1, 1)
    row+=1
    grid.addWidget(strand_mode_label, row, 0, 1, 3)
    grid.addWidget(self.strand_mode_spinbox_, row, 4, 1, 1)
    
    grid.setRowStretch(row+1,1)
    self.setLayout(grid)
    
    QtCore.QObject.connect(self.spline_spinbox_, QtCore.SIGNAL("valueChanged(int)"), self.UpdateSplineDetail)
    QtCore.QObject.connect(self.arc_spinbox_, QtCore.SIGNAL("valueChanged(int)"), self.UpdateArcDetail)
    QtCore.QObject.connect(self.poly_mode_cb_, QtCore.SIGNAL("currentIndexChanged(int)"), self.UpdatePolyMode)
    
    QtCore.QObject.connect(self.width_tube_spinbox_, QtCore.SIGNAL("valueChanged(double)"), self.UpdateTubeRadius)
    QtCore.QObject.connect(self.width_tube_slider_, QtCore.SIGNAL("valueChanged(int)"), self.UpdateSliderTubeRadius)
    QtCore.QObject.connect(self.thickness_tube_spinbox_, QtCore.SIGNAL("valueChanged(double)"), self.UpdateTubeRatio)
    QtCore.QObject.connect(self.thickness_tube_slider_, QtCore.SIGNAL("valueChanged(int)"), self.UpdateSliderTubeRatio)
    QtCore.QObject.connect(self.tube_profile_spinbox_, QtCore.SIGNAL("valueChanged(int)"), self.UpdateTubeProfileType)
    
    QtCore.QObject.connect(self.width_helix_spinbox_, QtCore.SIGNAL("valueChanged(double)"), self.UpdateHelixWidth)
    QtCore.QObject.connect(self.width_helix_slider_, QtCore.SIGNAL("valueChanged(int)"), self.UpdateSliderHelixWidth)
    QtCore.QObject.connect(self.thickness_helix_spinbox_, QtCore.SIGNAL("valueChanged(double)"), self.UpdateHelixThickness)
    QtCore.QObject.connect(self.thickness_helix_slider_, QtCore.SIGNAL("valueChanged(int)"), self.UpdateSliderHelixThickness)
    QtCore.QObject.connect(self.ecc_helix_spinbox_, QtCore.SIGNAL("valueChanged(double)"), self.UpdateHelixEcc)
    QtCore.QObject.connect(self.ecc_helix_slider_, QtCore.SIGNAL("valueChanged(int)"), self.UpdateSliderHelixEcc)
    QtCore.QObject.connect(self.helix_profile_spinbox_, QtCore.SIGNAL("valueChanged(int)"), self.UpdateHelixProfileType)
    QtCore.QObject.connect(self.helix_mode_spinbox_, QtCore.SIGNAL("valueChanged(int)"), self.UpdateHelixMode)

    QtCore.QObject.connect(self.width_strand_spinbox_, QtCore.SIGNAL("valueChanged(double)"), self.UpdateStrandWidth)
    QtCore.QObject.connect(self.width_strand_slider_, QtCore.SIGNAL("valueChanged(int)"), self.UpdateSliderStrandWidth)
    QtCore.QObject.connect(self.thickness_strand_spinbox_, QtCore.SIGNAL("valueChanged(double)"), self.UpdateStrandThickness)
    QtCore.QObject.connect(self.thickness_strand_slider_, QtCore.SIGNAL("valueChanged(int)"), self.UpdateSliderStrandThickness)
    QtCore.QObject.connect(self.ecc_strand_spinbox_, QtCore.SIGNAL("valueChanged(double)"), self.UpdateStrandEcc)
    QtCore.QObject.connect(self.ecc_strand_slider_, QtCore.SIGNAL("valueChanged(int)"), self.UpdateSliderStrandEcc)    
    QtCore.QObject.connect(self.strand_profile_spinbox_, QtCore.SIGNAL("valueChanged(int)"), self.UpdateStrandProfileType)
    QtCore.QObject.connect(self.strand_mode_spinbox_, QtCore.SIGNAL("valueChanged(int)"), self.UpdateStrandMode)
    
    self.setMinimumSize(250,550)
    ########/UI########
  
  def UpdateGui(self,options):
    self.poly_mode_cb_.setCurrentIndex(options.GetPolyMode())
    self.spline_spinbox_.setValue(options.GetSplineDetail())
    self.arc_spinbox_.setValue(options.GetArcDetail())
    
    self.UpdateTubeRadiusGui(options.GetTubeRadius())
    self.UpdateTubeRatioGui(options.GetTubeRatio())
    self.tube_profile_spinbox_.setValue(options.GetTubeProfileType())
    self.UpdateHelixWidthGui(options.GetHelixWidth())
    self.UpdateHelixThicknessGui(options.GetHelixThickness())
    self.UpdateHelixEccGui(options.GetHelixEcc())
    self.helix_profile_spinbox_.setValue(options.GetHelixProfileType())
    self.helix_mode_spinbox_.setValue(options.GetHelixMode())
    self.UpdateStrandWidthGui(options.GetStrandWidth())
    self.UpdateStrandThicknessGui(options.GetStrandThickness())
    self.UpdateStrandEccGui(options.GetStrandEcc())
    self.strand_profile_spinbox_.setValue(options.GetStrandProfileType())
    self.strand_mode_spinbox_.setValue(options.GetStrandMode())
    
  def UpdatePolyMode(self, value):
    self.GetOptions().SetPolyMode(value)
    self.ApplyOptions()
    
  def UpdateSplineDetail(self, value):
    self.GetOptions().SetSplineDetail(value)
    self.ApplyOptions()
    
  def UpdateArcDetail(self, value):
    self.GetOptions().SetArcDetail(value)
    self.ApplyOptions()
    
  def UpdateTubeRadius(self, value):
    self.GetOptions().SetTubeRadius(value)
    self.ApplyOptions()
    
  def UpdateSliderTubeRadius(self, value):
    self.GetOptions().SetTubeRadius(value/10.0)
    self.ApplyOptions()
    
  def UpdateTubeRatio(self, value):
    self.GetOptions().SetTubeRatio(value)
    self.ApplyOptions()
    
  def UpdateSliderTubeRatio(self, value):
    self.GetOptions().SetTubeRatio(value/10.0)
    self.ApplyOptions()
    
  def UpdateTubeProfileType(self, value):
    self.GetOptions().SetTubeProfileType(value)
    self.ApplyOptions()
    
  def UpdateHelixWidth(self, value):
    self.GetOptions().SetHelixWidth(value) 
    self.ApplyOptions()
    
  def UpdateSliderHelixWidth(self, value):
    self.GetOptions().SetHelixWidth(value/10.0)
    self.ApplyOptions()
    
  def UpdateHelixThickness(self, value):
    self.GetOptions().SetHelixThickness(value)
    self.ApplyOptions()
    
  def UpdateSliderHelixThickness(self, value):
    self.GetOptions().SetHelixThickness(value/10.0)
    self.ApplyOptions()
    
  def UpdateHelixEcc(self, value):
    self.GetOptions().SetHelixEcc(value)
    self.ApplyOptions()
    
  def UpdateHelixProfileType(self, value):
    self.GetOptions().SetHelixProfileType(value)
    self.ApplyOptions()

  def UpdateHelixMode(self, value):
    self.GetOptions().SetHelixMode(value)
    self.ApplyOptions()
    
  def UpdateSliderHelixEcc(self, value):
    self.GetOptions().SetHelixEcc(value/10.0)
    self.ApplyOptions()
    
  def UpdateStrandWidth(self, value):
    self.GetOptions().SetStrandWidth(value)
    self.ApplyOptions()
    
  def UpdateSliderStrandWidth(self, value):
    self.GetOptions().SetStrandWidth(value/10.0)
    self.ApplyOptions()
    
  def UpdateStrandThickness(self, value):
    self.GetOptions().SetStrandThickness(value)
    self.ApplyOptions()
    
  def UpdateSliderStrandThickness(self, value):
    self.GetOptions().SetStrandThickness(value/10.0)
    self.ApplyOptions()
    
  def UpdateStrandEcc(self, value):
    self.GetOptions().SetStrandEcc(value)
    self.ApplyOptions()
    
  def UpdateSliderStrandEcc(self, value):
    self.GetOptions().SetStrandEcc(value/10.0)
    self.ApplyOptions()
    
  def UpdateStrandProfileType(self, value):
    self.GetOptions().SetStrandProfileType(value)
    self.ApplyOptions()
    
  def UpdateStrandMode(self, value):
    self.GetOptions().SetStrandMode(value)
    self.ApplyOptions()
    
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
