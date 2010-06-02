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
    self.display_modes_.addItem("Highlight properties")
    self.display_modes_.addItem("Secondary structure")
    self.display_modes_.addItem("Highlight conservation")
    QtCore.QObject.connect(self.display_modes_, QtCore.SIGNAL("activated(int)"), self.__ChangeDisplayMode)
    self.setMinimumSize(250,150)
    
    
  def Update(self):
    self.display_modes_.setEnabled(True)
    scene_selection = gui.SceneSelection.Instance()
    
    if scene_selection.GetActiveNodeCount() == 0 and scene_selection.GetActiveViewCount() == 0:
      self.display_modes_.setEnabled(False)
      return
        
    for i in range(0,scene_selection.GetActiveNodeCount()):
      node = scene_selection.GetActiveNode(i)
      if not (isinstance(node, gfx.Entity) or (isinstance(node, seq.AlignmentHandle))) :
        self.display_modes_.setEnabled(self,False)
        return
    
    seq_viewer = gui.GostyApp.Instance().GetSequenceViewerV2()
    
  def __ChangeDisplayMode(self):
    seq_viewer = gui.GostyApp.Instance().GetSequenceViewerV2()
    scene_selection = gui.SceneSelection.Instance()
    for i in range(0,scene_selection.GetActiveNodeCount()):
      node = scene_selection.GetActiveNode(i)
      seq_viewer.ChangeDisplayMode(node, str(self.display_modes_.currentText()))
    