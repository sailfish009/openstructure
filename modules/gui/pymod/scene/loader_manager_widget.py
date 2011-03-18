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
from datetime import datetime
from PyQt4 import QtCore, QtGui

from loader_list_model import LoaderListModel
from file_loader import GenericLoader

#Gradient Preset Widget
class LoaderManagerWidget(QtGui.QDialog):
  ICONS_DIR = os.path.join(ost.GetSharedDataPath(), "gui", "icons/")
  def __init__(self, parent=None):
    QtGui.QDialog.__init__(self, parent)
    
    #Title
    self.text_ = "Loader Manager"
        
    #Create Ui elements
    self.list_view_ = QtGui.QListView()
    
    #Create Model
    self.list_model_ = LoaderListModel(self)
    self.list_view_.setModel(self.list_model_)
    self.list_view_.setEditTriggers(QtGui.QAbstractItemView.NoEditTriggers)
        
    loader_label = QtGui.QLabel(self.text_)
    font = loader_label.font()
    font.setBold(True)
    
    self.add_action = QtGui.QAction("+",self)
    self.add_action.setIcon(QtGui.QIcon(LoaderManagerWidget.ICONS_DIR+"add_icon.png"))
    
    QtCore.QObject.connect(self.add_action, QtCore.SIGNAL("triggered()"), self.Add)
    
    self.add_button_ = QtGui.QToolButton(self)
    self.add_button_.setIconSize(QtCore.QSize(20,20))
    self.add_button_.setDefaultAction(self.add_action)
        
    grid = QtGui.QGridLayout()
    grid.setContentsMargins(0,5,0,0)
    grid.addWidget(loader_label, 0, 0, 1, 1)
    qhbox = QtGui.QHBoxLayout()
    grid.addWidget(self.list_view_,1,0,3,3)
    grid.addWidget(self.add_button_,4,0,1,1)
    self.setLayout(grid)
    
    self.list_view_.setContextMenuPolicy(QtCore.Qt.CustomContextMenu)
    QtCore.QObject.connect(self.list_view_, QtCore.SIGNAL("customContextMenuRequested(const QPoint)"), self.contextMenuEvent)
    self.CreateContextMenu()

    QtCore.QObject.connect(self.list_view_, QtCore.SIGNAL("doubleClicked(const QModelIndex)"), self.Edit)

  def CreateContextMenu(self):
    self.contextMenu_ = QtGui.QMenu("Context menu", self)
    self.remove_ = QtGui.QAction("Remove", self.list_view_)
    self.rename_ = QtGui.QAction("Rename", self.list_view_)
    self.edit_ = QtGui.QAction("Edit", self.list_view_)
    self.contextMenu_.addAction(self.remove_)
    self.contextMenu_.addAction(self.rename_)
    self.contextMenu_.addAction(self.edit_)
    #Connect Signals with Slots  
    QtCore.QObject.connect(self.remove_, QtCore.SIGNAL("triggered()"), self.Remove)
    QtCore.QObject.connect(self.rename_, QtCore.SIGNAL("triggered()"), self.Rename)
    QtCore.QObject.connect(self.edit_, QtCore.SIGNAL("triggered()"), self.Edit)
  
  def contextMenuEvent(self, pos):
    #ContextMenu
    index = self.list_view_.indexAt(pos)
    if index.isValid(): 
      if self.list_model_.IsEditable(index.row()):
        self.contextMenu_.popup(QtGui.QCursor.pos())
  
  def Add(self):
    row = self.list_model_.GetLastRow()
    edit_widget = LoaderEditWidget()
    
    if edit_widget.exec_() and self.list_model_.AddItem(datetime.now().isoformat(' '), edit_widget.GetLoader(), row, True, True):
      index = self.list_model_.index(row)
      self.list_view_.setCurrentIndex(index)
      self.Rename()
    
  def Edit(self):
    current_index = self.list_view_.currentIndex()
    loader = self.list_model_.GetLoader(current_index)
    edit_widget = LoaderEditWidget()
    edit_widget.SetLoader(loader)
    if edit_widget.exec_():
      self.list_model_.SetItem(current_index, edit_widget.GetLoader())
    
      
  def Remove(self):
    if(self.list_view_.currentIndex().isValid()):
      ret = QtGui.QMessageBox.warning(self, "Delete File Loader",
                   "Delete File Loader?",
                   QtGui.QMessageBox.Yes | QtGui.QMessageBox.No)
      if ret == QtGui.QMessageBox.Yes:
        self.list_model_.RemoveItem(self.list_view_.currentIndex().row())

  def Rename(self):
    if(self.list_view_.currentIndex().isValid()):
      self.list_view_.edit(self.list_view_.currentIndex())
      
  def GetText(self):
    return self.text_
      



class LoaderEditWidget(QtGui.QDialog):
  def __init__(self, parent=None):
    QtGui.QDialog.__init__(self, parent)
    name_ext_label = QtGui.QLabel("Name Extension")
    self.name_ext_edit_ = QtGui.QLineEdit()
    
    url_label = QtGui.QLabel("Url")
    self.url_edit_ = QtGui.QLineEdit()
    self.add_id_button_ = QtGui.QToolButton(self)
    self.add_id_button_.setDefaultAction(QtGui.QAction("ID",self))
     
    file_ext_label = QtGui.QLabel("File Type")
    self.file_ext_auto_ = QtGui.QCheckBox("Auto")    
    self.file_ext_edit_ = QtGui.QLineEdit()
    
    self.hbox_ = QtGui.QHBoxLayout()
    self.ok_button_ = QtGui.QPushButton("OK")
    self.cancel_button_ = QtGui.QPushButton("Cancel")
    self.hbox_.addWidget(self.ok_button_)
    self.hbox_.addStretch()
    self.hbox_.addWidget(self.cancel_button_)
    
    grid = QtGui.QGridLayout()
    grid.setContentsMargins(0,5,0,0)
    grid.addWidget(name_ext_label, 0, 0, 1, 1)
    grid.addWidget(self.name_ext_edit_, 0, 1, 1, 4)
    grid.addWidget(url_label, 1, 0, 1, 1)
    grid.addWidget(self.url_edit_, 1, 1, 1, 3)
    grid.addWidget(self.add_id_button_, 1, 4, 1, 1)
    grid.addWidget(file_ext_label, 2, 0, 1, 1)
    grid.addWidget(self.file_ext_auto_, 2, 1, 1, 1)
    grid.addWidget(self.file_ext_edit_, 2, 2, 1, 3)
    grid.addLayout(self.hbox_,3,0,1,2)
    grid.setRowStretch(2, 1)
    self.setLayout(grid)
    
    QtCore.QObject.connect(self.add_id_button_, QtCore.SIGNAL("clicked()"), self.AppendId)
    QtCore.QObject.connect(self.file_ext_auto_, QtCore.SIGNAL("stateChanged(int)"), self.CbStateChanged)
    QtCore.QObject.connect(self.ok_button_, QtCore.SIGNAL("clicked()"), self.Ok)
    QtCore.QObject.connect(self.cancel_button_, QtCore.SIGNAL("clicked()"), self.Cancel)
        
  def GetLoader(self):
    name_ext = str(self.name_ext_edit_.text())
    url = str(self.url_edit_.text())
    file_ext = None
    if not self.file_ext_auto_.isChecked():
      file_ext = str(self.file_ext_edit_.text())
    
    return GenericLoader(name_ext, url, False, file_ext)

  def SetLoader(self, loader):
    self.name_ext_edit_.setText(loader.name_)
    self.url_edit_.setText(loader.url_)
    if loader.file_type_ is not None:
      self.file_ext_edit_.setText(loader.file_type_)
      self.file_ext_auto_.setChecked(False)
    else:
      self.file_ext_auto_.setChecked(True)
  def AppendId(self):
    self.url_edit_.setText(self.url_edit_.text()+"${ID}")

  def CbStateChanged(self, state):
      self.file_ext_edit_.setEnabled(not self.file_ext_auto_.isChecked())
      
  def Ok(self):
    self.accept()
    
  def Cancel(self):
    self.reject()
