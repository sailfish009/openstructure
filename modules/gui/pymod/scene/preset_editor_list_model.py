from ost import gui
from ost import gfx
import os
import ost
from PyQt4 import QtCore, QtGui
from preset import Preset

class PresetEditorListModel(QtCore.QAbstractListModel):
  def __init__(self, preset, parent=None, *args): 
    QtCore.QAbstractListModel.__init__(self, parent, *args)
    
    self.preset_ = preset
    
  def AddItem(self, color_op, row):
    self.insertRow(row, QtCore.QModelIndex(),color_op)
    model_index = self.createIndex(row,0)
    end_index = self.createIndex(self.rowCount(),0)
    self.emit(QtCore.SIGNAL("dataChanged"),model_index, end_index)
    return True
  
  def RemoveItem(self, row):
    self.removeRow(row, QtCore.QModelIndex())
    model_index = self.createIndex(row,0)
    self.emit(QtCore.SIGNAL("dataChanged"),model_index, model_index)
    return True
    
  def GetColorOp(self, model_index):
    if model_index.isValid():
      return self.preset_.GetColorOp(model_index.row())
  
  def SetItem(self, model_index, color_op):
    self.preset_.SetColorOp(model_index.row(), color_op)
  
  def GetLastRow(self):
    return self.rowCount()
    
  #Overwritten Methods  
  def rowCount(self, parent=QtCore.QModelIndex()): 
    return self.preset_.GetColorOpCount() 
 
  def data(self, index, role):
    if index.isValid() and index.row()< self.rowCount():
      data = self.preset_.GetColorOp(index.row())
      if role == QtCore.Qt.DisplayRole:
        selection=str(data.GetSelection())
        if(len(selection)>0):
          return QtCore.QVariant(selection)
        else:
          return QtCore.QVariant("all")
    return QtCore.QVariant()

  def setData(self, index, value, role):
    if index.isValid():
      row = index.row()
      data = self.preset_.GetColorOp(row)
      if role == QtCore.Qt.EditRole:
        return True
      elif role == QtCore.Qt.DisplayRole:
        data
      elif role == QtCore.Qt.DecorationRole:
        data
    return False

  def flags(self, index):
    if index.isValid():
      flags = QtCore.QAbstractItemModel.flags(self,index)
      return flags | QtCore.Qt.ItemIsEditable
    return QtCore.Qt.ItemIsEnabled
  
  def insertRow(self, position, index, color_op):
    self.beginInsertRows(index, position, position)
    self.preset_.InsertColorOp(position, color_op)
    self.endInsertRows()
    return True
  
  def removeRow(self, position, index):
    self.beginRemoveRows(index, position, position)
    self.preset_.RemoveColorOpAt(position)
    self.endRemoveRows()
    return True
  