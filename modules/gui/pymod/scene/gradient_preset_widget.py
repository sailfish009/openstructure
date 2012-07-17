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

import os
import ost
from ost import gui
from ost import gfx
from ost import info
from datetime import datetime

from datetime import datetime
from PyQt4 import QtCore, QtGui
from gradient_info_handler import GradientInfoHandler
from gradient_list_model import GradientListModel


#Gradient Preset Widget
class GradientPresetWidget(QtGui.QWidget):
  ICONS_DIR = os.path.join(ost.GetSharedDataPath(), "gui", "icons/")
  def __init__(self, gradient_edit, parent=None):
    QtGui.QWidget.__init__(self, parent)
    
    #Title
    self.text_ = "Gradient Presets"
    
    #Refrences
    self.gradient_edit_ = gradient_edit
    
    #Create Ui elements
    self.list_view_ = QtGui.QListView()
    
    #Create Model
    self.list_model_ = GradientListModel(self)
    self.list_view_.setModel(self.list_model_)
    self.list_view_.setEditTriggers(QtGui.QAbstractItemView.NoEditTriggers)
        
    preset_label = QtGui.QLabel(self.text_)
    font = preset_label.font()
    font.setBold(True)
    
    self.add_action = QtGui.QAction("+",self)
    self.add_action.setIcon(QtGui.QIcon(GradientPresetWidget.ICONS_DIR+"add_icon.png"))
    
    QtCore.QObject.connect(self.add_action, QtCore.SIGNAL("triggered()"), self.Add)
    
    self.add_button_ = QtGui.QToolButton(self)
    self.add_button_.setIconSize(QtCore.QSize(20,20))
    self.add_button_.setDefaultAction(self.add_action)
        
    grid = QtGui.QGridLayout()
    grid.setContentsMargins(0,5,0,0)
    grid.addWidget(preset_label, 0, 0, 1, 1)
    qhbox = QtGui.QHBoxLayout()
    grid.addWidget(self.list_view_,1,0,3,3)
    grid.addWidget(self.add_button_,4,0,1,1)
    self.setLayout(grid)
    
    self.list_view_.setContextMenuPolicy(QtCore.Qt.CustomContextMenu)
    QtCore.QObject.connect(self.list_view_, QtCore.SIGNAL("customContextMenuRequested(const QPoint)"), self.contextMenuEvent)
    self.CreateImmutableContextMenu()
    self.CreateContextMenu()

    QtCore.QObject.connect(self.list_view_, QtCore.SIGNAL("doubleClicked(const QModelIndex)"), self.Load)

  def CreateImmutableContextMenu(self):  
    self.immucontextMenu_ = QtGui.QMenu("Context menu", self)
    self.load_ = QtGui.QAction("Load", self.list_view_)  
    self.immucontextMenu_.addAction(self.load_)
    #Connect Signal with Slot  
    QtCore.QObject.connect(self.load_, QtCore.SIGNAL("triggered()"), self.LoadCurrentIndex)

  def CreateContextMenu(self):
    self.contextMenu_ = QtGui.QMenu("Context menu", self)
    self.remove_ = QtGui.QAction("Remove", self.list_view_)
    self.rename_ = QtGui.QAction("Rename", self.list_view_)
    self.contextMenu_.addAction(self.load_)
    self.contextMenu_.addAction(self.remove_)
    self.contextMenu_.addAction(self.rename_)
    #Connect Signals with Slots  
    QtCore.QObject.connect(self.remove_, QtCore.SIGNAL("triggered()"), self.Remove)
    QtCore.QObject.connect(self.rename_, QtCore.SIGNAL("triggered()"), self.Rename)
  
  def contextMenuEvent(self, pos):
    #ContextMenu
    index = self.list_view_.indexAt(pos)
    if index.isValid(): 
      if self.list_model_.IsEditable(index.row()):
        self.contextMenu_.popup(QtGui.QCursor.pos())
      else:
        self.immucontextMenu_.popup(QtGui.QCursor.pos())
  
  def Add(self):
    if(self.list_view_.currentIndex().isValid()):
      self.list_view_.closePersistentEditor(self.list_view_.currentIndex())
    row = self.list_model_.GetLastRow()
    if self.list_model_.AddItem(datetime.now().isoformat(' '), self.gradient_edit_.GetGradient(), row, True, True):
      index = self.list_model_.index(row)
      self.list_view_.setCurrentIndex(index)
      self.Rename()
    else:
      QtGui.QMessageBox.information(self, "Gradient not added", "The gradient could not be added!")
      
  def Remove(self):
    if(self.list_view_.currentIndex().isValid()):
      ret = QtGui.QMessageBox.warning(self, "Delete Gradient",
                   "Delete Gradient?",
                   QtGui.QMessageBox.Yes | QtGui.QMessageBox.No)
      if ret == QtGui.QMessageBox.Yes:
        self.list_model_.RemoveItem(self.list_view_.currentIndex().row())

  def LoadCurrentIndex(self):
    if(self.list_view_.currentIndex().isValid()):
      self.Load(self.list_view_.currentIndex())

  def Load(self, index):
    if(index.isValid()):
      self.emit(QtCore.SIGNAL("gradientSelected"),self.list_model_.GetGradient(index))

  def Rename(self):
    if(self.list_view_.currentIndex().isValid()):
      self.list_view_.edit(self.list_view_.currentIndex())
      
  def GetText(self):
    return self.text_
      
