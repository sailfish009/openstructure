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

import ost

from ost import gui
from ost import gfx
from ost import seq

from PyQt4 import QtCore, QtGui


class SequenceWidget(QtGui.QWidget):
  def __init__(self, parent=None):   
    QtGui.QWidget.__init__(self, parent)
    
    self.text_ = "Sequences"
    
    self.display_modes_ = QtGui.QComboBox(self);
    QtCore.QObject.connect(self.display_modes_, QtCore.SIGNAL("activated(int)"), self.__ChangeDisplayMode)
    self.setMinimumSize(250,150)
    
    
  def Update(self):
    self.display_modes_.setEnabled(True)
    scene_selection = gui.SceneSelection.Instance()
    seq_viewer = gui.GostyApp.Instance().GetSequenceViewerV2()
    self.display_modes_.clear()
          
    if scene_selection.GetActiveNodeCount() == 0 and scene_selection.GetActiveViewCount() == 0:
      self.display_modes_.setEnabled(False)
      return
    elif scene_selection.GetActiveNodeCount() == 1 and scene_selection.GetActiveViewCount() == 0:
      node = scene_selection.GetActiveNode(0) 
      if (isinstance(node, gfx.Entity)) or (isinstance(node, seq.AlignmentHandle)):
        selected = seq_viewer.GetCurrentDisplayMode(node)
        modes = seq_viewer.GetDisplayModes(node)
      elif (isinstance(node,gfx.GfxNode)) and (node.GetName() == "Scene"):
        selected = seq_viewer.GetCurrentDisplayMode()
        modes = seq_viewer.GetDisplayModes()
      else:
        self.display_modes_.setEnabled(self,False)
        selected = None
        modes = list()
      for m in modes:
        self.display_modes_.addItem(m)
      for i in range(0,self.display_modes_.count()):
        if(selected == self.display_modes_.itemText(i)):
          self.display_modes_.setCurrentIndex(i)
    elif scene_selection.GetActiveNodeCount() > 1 and scene_selection.GetActiveViewCount() == 0:
      for i in range(0,scene_selection.GetActiveNodeCount()):
        node = scene_selection.GetActiveNode(i)
        if not (isinstance(node, gfx.Entity) or (isinstance(node, seq.AlignmentHandle))) :
          self.display_modes_.setEnabled(self,False)
          return
      selected = seq_viewer.GetCurrentDisplayMode()
      modes = seq_viewer.GetDisplayModes()
      for m in modes:
        self.display_modes_.addItem(m)
      for i in range(0,self.display_modes_.count()):
        if(selected == self.display_modes_.itemText(i)):
          self.display_modes_.setCurrentIndex(i)
    else:
      self.display_modes_.setEnabled(self,False)      
    
    
  def __ChangeDisplayMode(self):
    seq_viewer = gui.GostyApp.Instance().GetSequenceViewerV2()
    scene_selection = gui.SceneSelection.Instance()
    for i in range(0,scene_selection.GetActiveNodeCount()):
      node = scene_selection.GetActiveNode(i)
      if (isinstance(node,gfx.GfxNode)) and (node.GetName() == "Scene"):
        seq_viewer.ChangeDisplayMode(str(self.display_modes_.currentText()))
      else:
        seq_viewer.ChangeDisplayMode(node, str(self.display_modes_.currentText()))
    