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

from file_loader import GenericLoader

class ImmutableLoaderInfoHandler():
  LOADERS_GROUP_NAME = "GenericLoaders"
  LOADER_GROUP_NAME = "GenericLoader"
  NAME_ATTRIBUTE_NAME = "Name"
  
  def __init__(self, file_name):
    self.file_name_ = file_name
    self.handle_ = info.LoadOrCreateInfo(self.file_name_)
    
    self.root_ = self.handle_.Root()
    
    self.loaders_ = self.root_.RetrieveGroup(ImmutableLoaderInfoHandler.LOADERS_GROUP_NAME)

    
  def GetLoader(self, name):
    group_list = self.loaders_.GetGroups(ImmutableLoaderInfoHandler.LOADER_GROUP_NAME)
    for group in group_list:
      if group.HasAttribute(ImmutableLoaderInfoHandler.NAME_ATTRIBUTE_NAME):
        groupname = group.GetAttribute(ImmutableLoaderInfoHandler.NAME_ATTRIBUTE_NAME)
        if name == groupname:
          loader = GenericLoader.FromInfo(group)
          return loader
    return None
  
  def GetLoaders(self):
    group_list = self.loaders_.GetGroups(ImmutableLoaderInfoHandler.LOADER_GROUP_NAME)
    loaders = dict()
    
    for group in group_list:
      if group.HasAttribute(ImmutableLoaderInfoHandler.NAME_ATTRIBUTE_NAME):
        loader = GenericLoader.FromInfo(group)
        loaders[group.GetAttribute(self.NAME_ATTRIBUTE_NAME)] = loader
    return loaders
