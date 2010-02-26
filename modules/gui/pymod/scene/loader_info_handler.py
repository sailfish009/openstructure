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
from ost import info
from PyQt4 import QtCore, QtGui
from immutable_loader_info_handler import ImmutableLoaderInfoHandler

#Generic Loader Info Handler
class LoaderInfoHandler(ImmutableLoaderInfoHandler):
  
  def __init__(self, file_name):    
    ImmutableLoaderInfoHandler.__init__(self, file_name)
  
  def StoreLoader(self,name, loader):
    group = self.loaders_.CreateGroup(ImmutableLoaderInfoHandler.LOADER_GROUP_NAME)
    group.SetAttribute(ImmutableLoaderInfoHandler.NAME_ATTRIBUTE_NAME, str(name))    
    loader.ToInfo(group)
    self.handle_.Export(self.file_name_)
    
  def RemoveLoader(self, name):
    group_list = self.loaders_.GetGroups(ImmutableLoaderInfoHandler.LOADER_GROUP_NAME)
    group_to_del = None
    for group in group_list:
      if group.HasAttribute(ImmutableLoaderInfoHandler.NAME_ATTRIBUTE_NAME):
        groupname = group.GetAttribute(ImmutableLoaderInfoHandler.NAME_ATTRIBUTE_NAME)
        if name == groupname:
          group_to_del = group
          break
    
    if group_to_del != None:
      self.loaders_.Remove(group_to_del)      
      self.handle_.Export(self.file_name_)
      
  def RenameLoader(self, old, new):
    group_list = self.loaders_.GetGroups(ImmutableLoaderInfoHandler.LOADER_GROUP_NAME)
    for group in group_list:
      if group.HasAttribute(ImmutableLoaderInfoHandler.NAME_ATTRIBUTE_NAME):
        groupname = group.GetAttribute(ImmutableLoaderInfoHandler.NAME_ATTRIBUTE_NAME)
        if old == groupname:
          group.SetAttribute(ImmutableLoaderInfoHandler.NAME_ATTRIBUTE_NAME, new)
          self.handle_.Export(self.file_name_)