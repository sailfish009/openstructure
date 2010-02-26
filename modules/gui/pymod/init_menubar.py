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

import sys
from ost import gui
import sip
from ost import gfx
import ost

from PyQt4 import QtCore, QtGui
from ost.gui import FileLoader
from ost.gui.scene.file_loader import GenericLoader

from ost.gui.scene.loader_manager_widget import LoaderManagerWidget

class InitMenuBar(QtCore.QObject):
  def __init__(self, menu_bar=None):
    QtCore.QObject.__init__(self, menu_bar)
    
    persp=gui.GostyApp.Instance().perspective
    file=persp.GetMenu("File")
    options=persp.GetMenu("Options")
    window=persp.GetMenu("Window")
    help=persp.GetMenu("Help")
    
    load = QtGui.QAction(QtGui.QIcon('icons/open.png'), '&Open', self)
    load.setStatusTip('Load a file')
    load.setShortcut('Ctrl+O')
    self.connect(load, QtCore.SIGNAL('triggered()'), self.Load)
    file.addAction(load)

    webpage = QtGui.QAction('&Documentation', self)
    webpage.setStatusTip('Documentation')
    webpage.setShortcut('Ctrl+D')
    self.connect(webpage, QtCore.SIGNAL('triggered()'), self.OpenDocs)
    help.addAction(webpage)
        
    window.addMenu(persp.panels.menu)
    #Options
    #Add file loader to menu
    loader_manager = QtGui.QAction('File &Loader', self)
    loader_manager.setStatusTip('Loader Manager')
    self.connect(loader_manager, QtCore.SIGNAL('triggered()'), self.LoaderManager)
    options.addAction(loader_manager)
    
    self.loader_manager = LoaderManagerWidget()
    
  def Exit(self):
    reply = QtGui.QMessageBox()
    reply.addButton(QtGui.QMessageBox.Yes)
    

  def Load(self):
    filename = QtGui.QFileDialog.getOpenFileName(None, 'Open file','')
    if(QtCore.QFileInfo(filename).isFile()):
      FileLoader.LoadObject(str(filename))

  def LoaderManager(self):
    self.loader_manager.exec_()
  
  def OpenDocs(self):
    QtGui.QDesktopServices.openUrl(QtCore.QUrl("http://www.openstructure.org/docs/index.html"))
    
def _InitMenuBar(menu_bar):
  InitMenuBar(menu_bar)
  
## \example menubar_example.py
#
# Shows how to use PyQt to add a menu from within Python and interact 
# with the currently selected objects in the scene menu.
