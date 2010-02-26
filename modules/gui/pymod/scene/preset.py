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

from ost import info
from ost import gfx
from PyQt4 import QtGui

#Gradient Editor
class Preset():
  NAME_ATTRIBUTE_NAME = "Name"
  COLOR_OP_GROUP_NAME = "ColorOp"
  CLASS_NAME_ATTRIBUTE_NAME = "ClassName"
  INDEX_ATTRIBUTE_NAME = "Index"
  
  MODULE_NAME = "ost.gfx"
  
  def __init__(self, name, parent=None):    
    self.name_ = name
    self.color_ops_ = list()
    
  def SetName(self, name):
    self.name_ = name
    
  def GetName(self):
    return self.name_
    
  def InsertColorOp(self, index, color_op):
    self.color_ops_.insert(index, color_op)
    
  def RemoveColorOp(self, color_op):
    self.color_ops_.remove(color_op)
  
  def RemoveColorOpAt(self, index):
    del(self.color_ops_[index])
    
  def GetColorOp(self, index):
    return self.color_ops_[index]
  
  def GetColorOpCount(self):
    return len(self.color_ops_)
  
  def SetColorOp(self, index, color_op):
    self.color_ops_[index] = color_op
  
  def AddColorOp(self, color_op):
    self.color_ops_.append(color_op)
  
  def GetColorOps(self):
    return self.color_ops_
  
  def ApplyOn(self, entity):
    if (entity is not None) and isinstance(entity, gfx.Entity):
      for color_op in self.color_ops_:
        entity.Apply(color_op)
  
  def ToInfo(self,group):
    group.SetAttribute(Preset.NAME_ATTRIBUTE_NAME, self.name_)
    for i in range(0,len(self.color_ops_)):
      co_op_group = group.CreateGroup(Preset.COLOR_OP_GROUP_NAME)
      co_op_group.SetAttribute(Preset.INDEX_ATTRIBUTE_NAME, str(i))
      co_op_group.SetAttribute(Preset.CLASS_NAME_ATTRIBUTE_NAME, "%s"%(self.color_ops_[i].__class__.__name__))
      self.color_ops_[i].ToInfo(co_op_group)
    
  @staticmethod
  def FromInfo(group):
    preset = None
    if group.HasAttribute(Preset.NAME_ATTRIBUTE_NAME):
      name = group.GetAttribute(Preset.NAME_ATTRIBUTE_NAME)
      preset = Preset(name)
      group_list = group.GetGroups(Preset.COLOR_OP_GROUP_NAME)
      
      class_order_dict = dict()
      for co_group in group_list:
        if(co_group.HasAttribute(Preset.CLASS_NAME_ATTRIBUTE_NAME) and co_group.HasAttribute(Preset.INDEX_ATTRIBUTE_NAME)):
          class_name = co_group.GetAttribute(Preset.CLASS_NAME_ATTRIBUTE_NAME)
          index = int(co_group.GetAttribute(Preset.INDEX_ATTRIBUTE_NAME))
          color_op_class = Preset.__get_color_op_class("%s.%s"%(Preset.MODULE_NAME,class_name))
          color_op = color_op_class.FromInfo(co_group)
          class_order_dict[index]=color_op
    for i in range(0, len(class_order_dict)):
      if(class_order_dict.has_key(i)):
        preset.AddColorOp(class_order_dict[i])
    return preset
    
  @staticmethod
  def __get_color_op_class( cls ):
    parts = cls.split('.')
    module = ".".join(parts[:-1])
    m = __import__( module )
    for comp in parts[1:]:
      m = getattr(m, comp)
    return m
