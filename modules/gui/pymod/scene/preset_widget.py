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

from ost import gui
from ost import gfx
import ost
import os
from datetime import datetime
from PyQt4 import QtCore, QtGui
from preset_list_model import PresetListModel
from preset_editor_widget import PresetEditor
from preset import Preset

class PresetWidget(QtGui.QWidget):
  PRESET_XML_FILE = os.path.join(ost.GetSharedDataPath(), "scene", "presets.xml")
  ICONS_DIR = os.path.join(ost.GetSharedDataPath(), "gui", "icons/")
  def __init__(self, parent=None):   
    QtGui.QWidget.__init__(self, parent)
    
    self.text_ = "Presets"
        
    #Create Ui elements
    self.list_view_ = QtGui.QListView()
        
    #Create Model
    self.list_model_ = PresetListModel(self)
    self.list_view_.setModel(self.list_model_)
    self.list_view_.setEditTriggers(QtGui.QAbstractItemView.NoEditTriggers)
    
    self.add_action = QtGui.QAction("+",self)
    self.add_action.setIcon(QtGui.QIcon(PresetWidget.ICONS_DIR+"add_icon.png"))
    
    QtCore.QObject.connect(self.add_action, QtCore.SIGNAL("triggered()"), self.Add)
    
    self.add_button_ = QtGui.QToolButton(self)
    self.add_button_.setIconSize(QtCore.QSize(20,20))
    self.add_button_.setDefaultAction(self.add_action)
    
    self.preset_editor_ = PresetEditor(self)
    
    grid = QtGui.QGridLayout()
    grid.setContentsMargins(0,5,0,0)
    grid.addWidget(self.list_view_,0,0,3,3)
    grid.addWidget(self.add_button_,3,0,1,1)
    self.setLayout(grid)
    
    self.list_view_.setContextMenuPolicy(QtCore.Qt.CustomContextMenu)
    QtCore.QObject.connect(self.list_view_, QtCore.SIGNAL("customContextMenuRequested(const QPoint)"), self.contextMenuEvent)
    self.CreateImmutableContextMenu()
    self.CreateContextMenu()
  
    QtCore.QObject.connect(self.list_view_, QtCore.SIGNAL("doubleClicked(const QModelIndex)"), self.Load)
    
    self.setMinimumSize(250,150)
    
  def CreateImmutableContextMenu(self):  
    self.immucontext_menu_ = QtGui.QMenu("Context menu", self)
    self.load_ = QtGui.QAction("Load", self.list_view_)  
    self.immucontext_menu_.addAction(self.load_)
    #Connect Signal with Slot  
    QtCore.QObject.connect(self.load_, QtCore.SIGNAL("triggered()"), self.LoadCurrentIndex)

  def CreateContextMenu(self):
    self.context_menu_ = QtGui.QMenu("Context menu", self)
    self.remove_ = QtGui.QAction("Remove", self.list_view_)
    self.rename_ = QtGui.QAction("Rename", self.list_view_)
    self.edit_ = QtGui.QAction("Edit", self.list_view_)
    self.context_menu_.addAction(self.load_)
    self.context_menu_.addAction(self.remove_)
    self.context_menu_.addAction(self.rename_)
    self.context_menu_.addAction(self.edit_)
    #Connect Signals with Slots  
    QtCore.QObject.connect(self.remove_, QtCore.SIGNAL("triggered()"), self.Remove)
    QtCore.QObject.connect(self.rename_, QtCore.SIGNAL("triggered()"), self.Rename)
    QtCore.QObject.connect(self.edit_, QtCore.SIGNAL("triggered()"), self.Edit)
  
  def contextMenuEvent(self, pos):
    #ContextMenu
    index = self.list_view_.indexAt(pos)
    if index.isValid(): 
      if self.list_model_.IsEditable(index.row()):
        self.context_menu_.popup(QtGui.QCursor.pos())
      else:
        self.immucontext_menu_.popup(QtGui.QCursor.pos())
  
  def Add(self):
    row = self.list_model_.GetLastRow()
    preset = Preset(datetime.now().isoformat(' '))
    self.preset_editor_.SetPreset(preset)
    if(self.preset_editor_.exec_()):
      if self.list_model_.AddItem(preset, row, True, True):
        index = self.list_model_.index(row)
        self.list_view_.setCurrentIndex(index)
        self.Rename()
      
  def Remove(self):
    if(self.list_view_.currentIndex().isValid()):
      ret = QtGui.QMessageBox.warning(self, "Delete Preset",
                   "Delete Preset?",
                   QtGui.QMessageBox.Yes | QtGui.QMessageBox.No)
      if ret == QtGui.QMessageBox.Yes:
        self.list_model_.RemoveItem(self.list_view_.currentIndex().row())

  def Edit(self):
    if(self.list_view_.currentIndex().isValid()):
        preset = self.list_model_.GetPreset(self.list_view_.currentIndex())
        self.preset_editor_.SetPreset(preset)
        if(self.preset_editor_.exec_()):
            row = self.list_view_.currentIndex().row()
            self.list_model_.RemoveItem(row)
            self.list_model_.AddItem(preset, row, True, True)
        
  def LoadCurrentIndex(self):
    if(self.list_view_.currentIndex().isValid()):
      self.Load(self.list_view_.currentIndex())

  def Load(self, index):
    if(index.isValid()):
      scene_selection = gui.SceneSelection.Instance()
      preset=self.list_model_.GetPreset(index)
      for i in range(0,scene_selection.GetActiveNodeCount()):
        node = scene_selection.GetActiveNode(i)
        if isinstance(node, gfx.Entity):
          node.CleanColorOps()
          preset.ApplyOn(node)
          
  def ChangeColor(self,node):
    self.LoadCurrentIndex()
  
    
  def Rename(self):
    if(self.list_view_.currentIndex().isValid()):
      self.list_view_.edit(self.list_view_.currentIndex())
    
  def GetText(self):
    return self.text_
