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

#Trace Render Options
class TraceWidget(RenderModeWidget):
  def __init__(self, parent=None):
    RenderModeWidget.__init__(self, parent)
    
    #Title
    self.text_ = "Trace"
     
    #Set Render Mode
    self.mode_ = gfx.RenderMode.TRACE
        
    min_arc_detail = 1
    max_arc_detail = 20
    
    min_width = 0.1
    max_width = 2.5
    max_tube_width= 2.0
    
    #########UI##########
            
    #Arc Label
    arc_label = QtGui.QLabel("Arc Detail")
    
    self.arc_spinbox_ = QtGui.QSpinBox()
    self.arc_spinbox_.setRange(min_arc_detail, max_arc_detail)
        
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

    grid = QtGui.QGridLayout()
    grid.addWidget(arc_label,3,0,1,3)
    grid.addWidget(self.arc_spinbox_,3,4,1,1)
    
    grid.addWidget(radius_tube_label, 5, 0, 1, 1)
    grid.addWidget(self.width_tube_slider_, 5, 1, 1, 3)
    grid.addWidget(self.width_tube_spinbox_, 5, 4, 1, 1)
        
    grid.setRowStretch(15,1)
    self.setLayout(grid)
    
    QtCore.QObject.connect(self.arc_spinbox_, QtCore.SIGNAL("valueChanged(int)"), 
                           self.UpdateArcDetail)
    
    QtCore.QObject.connect(self.width_tube_spinbox_, 
                           QtCore.SIGNAL("valueChanged(double)"), 
                           self.UpdateTubeRadius)
    QtCore.QObject.connect(self.width_tube_slider_, 
                           QtCore.SIGNAL("valueChanged(int)"), 
                           self.UpdateSliderTubeRadius)
    
    self.setMinimumSize(250,60) #2*30
    ########/UI########
  def UpdateGui(self,options):
    self.arc_spinbox_.setValue(options.GetArcDetail())
    
    self.UpdateTubeRadiusGui(options.GetTubeRadius())
    
  def UpdatePolyMode(self, value):
    self.GetOptions().SetPolyMode(value)
    self.ApplyOptions()
        
  def UpdateArcDetail(self, value):
    self.GetOptions().SetArcDetail(value)
    self.ApplyOptions()
    
  def UpdateTubeRadius(self, value):
    self.GetOptions().SetTubeRadius(value)
    
  def UpdateSliderTubeRadius(self, value):
    self.GetOptions().SetTubeRadius(value/10.0)

  def UpdateTubeRadiusGui(self,value):
    if(abs(value*10.0 - self.width_tube_slider_.value())>=self.width_tube_spinbox_.singleStep()):
      self.width_tube_slider_.setValue(value*10.0)
    self.width_tube_spinbox_.setValue(value)
  
  def GetText(self):
    return self.text_

  def GetRenderMode(self):
    return self.mode_
