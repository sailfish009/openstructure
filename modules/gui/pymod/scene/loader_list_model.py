from ost import gui
from ost import gfx
import os
import ost
from PyQt4 import QtCore, QtGui
from immutable_loader_info_handler import ImmutableLoaderInfoHandler
from loader_info_handler import LoaderInfoHandler
from ost.gui import LoaderManager

class LoaderListModel(QtCore.QAbstractListModel):
  
  IMMUTABLE_LOADERS_PATH = os.path.join(ost.GetSharedDataPath(),"scene", 
                                          "loaders.xml")
  MUTABLE_LOADERS_PATH = "user_loaders.xml"
  
  def __init__(self, parent=None, *args): 
    QtCore.QAbstractListModel.__init__(self, parent, *args)
    
    self.data_ = list()
    
    #Info Handler
    self.immutable_infoh_ = ImmutableLoaderInfoHandler(LoaderListModel.IMMUTABLE_LOADERS_PATH)
    self.infoh_ = LoaderInfoHandler(LoaderListModel.MUTABLE_LOADERS_PATH)
    self.LoadLoaderFromInfo()
    
    self.loader_manager_ = gui.FileLoader.GetLoaderManager()
    
  def AddItem(self, name, loader, row, editable, save):
    if self.NameIsValid(name):
      self.insertRow(row, QtCore.QModelIndex())
      self.data_[row] = [name, loader, editable]
      model_index = self.createIndex(row,0)
      index = self.index(row)
      end_index = self.createIndex(self.rowCount(),0)
      if save:
        self.AddLoaderToInfo(name, loader)
      self.emit(QtCore.SIGNAL("dataChanged"),model_index, end_index)
      gui.FileLoader.GetLoaderManager().AddRemoteSiteLoader(name, loader)
      return True
    return False
  
  def IsEditable(self, row):
    return self.data_[row][2]
  
  def RemoveItem(self, row):
    if self.IsEditable(row):
      name = self.data_[row][0]
      self.removeRow(row, QtCore.QModelIndex())
      model_index = self.createIndex(row,0)
      self.infoh_.RemoveLoader(name)
      self.emit(QtCore.SIGNAL("dataChanged"),model_index, model_index)
      self.loader_manager_.RemoveRemoteSiteLoader(name)
      return True
    return False

  def AddLoaderToInfo(self, name, loader):
    self.infoh_.StoreLoader(name, loader)
  
  def RemoveLoaderFromInfo(self, name):
    self.infoh_.RemoveLoader(name)
  
  def SetItem(self, model_index, loader):
    row = model_index.row()
    name = self.data_[row][0]
    self.data_[row][1] = loader
    model_index = self.createIndex(row,0)
    index = self.index(row)
    end_index = self.createIndex(self.rowCount(),0)
    self.RemoveLoaderFromInfo(self.data_[row][0])
    self.AddLoaderToInfo(self.data_[row][0], loader)
    self.emit(QtCore.SIGNAL("dataChanged"),model_index, end_index)
    self.loader_manager_.RemoveRemoteSiteLoader(name)
    self.loader_manager_.AddRemoteSiteLoader(name, loader)
    return True
    
  def LoadLoaderFromInfo(self):
    if self.immutable_infoh_:
      loaders = self.immutable_infoh_.GetLoaders()
      for k, v in loaders.iteritems():
        self.AddItem(k, v, self.GetLastRow(), False, False)
    
    loaders = self.infoh_.GetLoaders()
    for k, v in loaders.iteritems():
      self.AddItem(k, v, self.GetLastRow(), True, False)

  def GetLoader(self, model_index):
    if model_index.isValid():
      return self.data_[model_index.row()][1]
  
  def GetLastRow(self):
    return self.rowCount()
  
  #Helper
  def NameIsValid(self, string):
    if len(string)==0:
      return False
    for values in self.data_:
      if string == values[0]:
        return False
    return True
  
  #Overwritten Methods  
  def rowCount(self, parent=QtCore.QModelIndex()): 
    return len(self.data_) 
 
  def data(self, index, role):
    if index.isValid() and index.row()< self.rowCount():
      data = self.data_[index.row()]
      if role == QtCore.Qt.DisplayRole:
        return QtCore.QVariant(data[0])
    return QtCore.QVariant()

  def setData(self, index, value, role):
    if index.isValid():
      row = index.row()
      if not self.data_[row]:
        self.data_[row] = list()
      if role == QtCore.Qt.EditRole and self.NameIsValid(value.toString()):
        old_name = str(self.data_[row][0])
        new_name = value.toString()
        self.data_[row][0] = new_name
        self.infoh_.RenameLoader(old_name,str(new_name))
        self.emit(QtCore.SIGNAL("dataChanged"),index, index)
        self.loader_manager_.RemoveRemoteSiteLoader(old_name)
        self.loader_manager_.AddRemoteSiteLoader(str(new_name), self.data_[row][1])
        return True
      elif role == QtCore.Qt.DisplayRole:
        self.data_[row][0] = value.toString()
    return False

  def flags(self, index):
    if index.isValid():
      flags = QtCore.QAbstractItemModel.flags(self,index)
      if self.IsEditable(index.row()):
        return flags | QtCore.Qt.ItemIsEditable
      else:
        return flags
    return QtCore.Qt.ItemIsEnabled
  
  def insertRow(self, position, index):
    self.beginInsertRows(index, position, position)
    self.data_.insert(position,list())
    self.endInsertRows()
    return True
  
  def removeRow(self, position, index):
    self.beginRemoveRows(index, position, position)
    del self.data_[position]
    self.endRemoveRows()
    return True

  
  
  
