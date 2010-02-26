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

import sys
from ost import gui
from ost import gfx
from PyQt4 import QtCore, QtGui

class ToolBarOptionsWidget(QtGui.QWidget):
  """QWidget with a ToolBar and a show area.
    
   This abstract QWidget has a toolbar and a show area. Whenever a button of the tool bar is pressed, the Widget corresponding to the button's value is shown in the show area.  
  """
  def __init__(self, parent=None):
    QtGui.QWidget.__init__(self, parent)
    
    #Setup ui_
    self.parent_=parent
    self.resize(400, 300)
    self.setMinimumSize(QtCore.QSize(250, 200))
    self.gridLayout = QtGui.QGridLayout(self)
    self.gridLayout.setHorizontalSpacing(0)
    self.gridLayout.setVerticalSpacing(0)
    self.gridLayout.setContentsMargins(0,0,0,0)
    self.gridLayout.setMargin(0)
    self.gridLayout.setSpacing(0)    
    self.tool_bar_ = QtGui.QToolBar(self)
    self.tool_bar_.setIconSize(QtCore.QSize(16, 16))
    self.gridLayout.addWidget(self.tool_bar_, 0, 0, 1, 1)
    self.stackedWidget = QtGui.QStackedWidget(self)
    self.gridLayout.addWidget(self.stackedWidget, 1, 0, 1, 1)

    self.current_action_ = None
    self.actions_ = list()
    
    QtCore.QObject.connect(self.tool_bar_, QtCore.SIGNAL("actionTriggered(QAction*)"), self.ChangeSelectedItem)
    
    self.setEnabled(False)
    
    #Entities
    self.entities_ = None
       
    self.Update()
  
  def Update(self):
    """Updates the ToolBarOptionsWidget and the active widget of the show area.
    
     This method calls the Update method of the active widget. 
     If no entity is set, the ToolBarOptionsWidget will be disabled
    """
    if hasattr(self, "entities_") and isinstance(self.entities_, list):
      widget = self.__GetCurrentWidget()
      if hasattr(widget, "Update"):
        self.__GetCurrentWidget().Update()
      self.setEnabled(True)
    else:
      self.setEnabled(False)
    
  def AddWidget(self, ident, widget):
    """Adds a Widget to this Options Widget.
    
     The Widget must have a identifier. If another Widget has the same identifier,
     the old widget will be removed and the new widget gets the identifier.
     Returns True, if widget is added. Otherwise it returns False
    """
    if isinstance(widget, QtGui.QWidget) and ident is not None:
      if hasattr(widget, "GetText"):
        string = QtCore.QString(widget.GetText())
      else:
        string = QtCore.QString(ident)
      
      self.stackedWidget.addWidget(widget)
      action = self.tool_bar_.addAction(ident)
      action.setIcon(QtGui.QIcon(ident))
      pair = ident, widget
      action.setData(QtCore.QVariant(pair))
      action.setCheckable(True);
      if(len(self.actions_) == 0):
        self.ChangeSelectedItem(action)
      self.actions_.append(action)
      return True
    return False
  
  def DoSomething(self, item):
    """This abstract method is called whenever the View is updated.
    
     This abstract method must be implemented by all subclasses. 
     It can be used to do something ;-) whenever the combobox changes its value.
    """
    raise NotImplementedError, "Subclasses must define DoSomething()"
  
  def DoResize(self):
    item = self.__GetCurrentWidget()
    width = 0
    height = 0
    if(hasattr(item,"minimumHeight")):
      height=item.minimumHeight()
    if(hasattr(item,"minimumWidth")):
      width=item.minimumWidth()
    self.setMinimumSize(width,self.tool_bar_.height()+height)
    if(hasattr(self.parent_,"DoResize")):
      self.parent_.DoResize()
  
  def SetEntities(self, entities):
    """Set entities.
    
     This method sets the given entity as current entity. If no entity is set, the Options Widget is disabled.
    """
    self.entities_ = entities
    for a in self.actions_:
      widget = a.data().toPyObject()[1]
      if hasattr(widget, "SetEntities"):
        widget.SetEntities(self.entities_)
  
  def ChangeSelectedItem(self, action):
    """Change Current Selected Item.
    
     Shows the widget which corresponds to the action in the show area.
    """
    if(self.current_action_ != None):
        self.current_action_.setChecked(False)
    else:
       self.current_action_ = action
    widget = action.data().toPyObject()[1]
    self.stackedWidget.setCurrentWidget(widget)
    self.DoSomething(widget)
    if hasattr(widget, "Update"): 
      widget.Update()
    if(self.current_action_ == action):
      self.current_action_.setChecked(True)
    else:
      self.current_action_=action
  #Private Methods
  def __GetCurrentWidget(self):
      return self.stackedWidget.currentWidget();

  #Overwritten Methods
  def setEnabled(self, bool):
    QtGui.QWidget.setEnabled(self, bool)

