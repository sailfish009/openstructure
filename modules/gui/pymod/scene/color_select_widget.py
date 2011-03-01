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

#Gradient Stop  
class ColorSelectWidget(QtGui.QWidget):
  def __init__(self, width, height, color, parent=None):
    QtGui.QWidget.__init__(self, parent)
    
    #Membervars
    self.width_ = width
    self.height_ = height

    if(color is None):
      self.color_ = QtGui.QColor("White")
    else:
      self.color_ = color
        
    self.show()
    
    #ContextMenu
    self.change_color_ = QtGui.QAction('ChangeColor', self)
    
    QtCore.QObject.connect(self.change_color_, QtCore.SIGNAL('triggered()'), self.ChangeColor)
    
    self.addAction(self.change_color_)
    self.setContextMenuPolicy(QtCore.Qt.ActionsContextMenu)
        
    self.Resize()
    
  def ChangeColor(self):
    color = QtGui.QColorDialog.getColor(self.color_, self)
    
    if(color != self.color_ and color.isValid()):
      self.color_ = color
      self.emit(QtCore.SIGNAL("colorChanged"))
      self.update()
    
  def GetColor(self):
    return self.color_
  
  def GetGfxColor(self):
    color = self.GetColor()
    return gfx.Color(color.redF(), color.greenF(), color.blueF())
  
  def SetColor(self, color):
    if(self.color_ != color):
      self.color_ = color
      self.emit(QtCore.SIGNAL("colorChanged"))
      self.update()
  
  def SetGfxColor(self, color):
    qcolor= QtGui.QColor(color.Red()*255,color.Green()*255,color.Blue()*255,color.Alpha()*255)
    self.SetColor(qcolor)
            
  def paintEvent(self, event):
    if self.isEnabled():
      size = self.size()
      paint = QtGui.QPainter()
      if paint.begin(self):
        brush = QtGui.QBrush(self.color_)
        paint.setBrush(brush)
        paint.drawRect(0,
                       0,
                       self.width() - 1,
                       self.height() - 1)
        paint.end()
      
  def mouseDoubleClickEvent(self, event):    
    self.ChangeColor()
   
  def SetSize(self, width, height):
    self.width_ = width
    self.height_ = height
    self.Resize()
    
  def Resize(self):
    self.setMinimumSize(self.width_, self.height_)
    self.setMaximumSize(self.width_, self.height_)  
    self.resize(self.width_, self.height_)
