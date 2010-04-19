from PyQt4 import QtCore, QtGui

from ost import geom, gfx, gui
from ost import settings
from ost.bindings import tmtools

class SelectRefDialog(QtGui.QDialog):
  def __init__(self, ent_list, parent=None):
    QtGui.QDialog.__init__(self, parent)
    self.ent_list_ = ent_list
    vb = QtGui.QVBoxLayout()
    self.setLayout(vb)
    self.setWindowTitle("Select Reference Object")
    self.label = QtGui.QLabel("Please Select the Reference Object")
    self.list = QtGui.QTableWidget(self)
    self.list.horizontalHeader().setStretchLastSection(True)
    self.list.setColumnCount(2)
    self.list.verticalHeader().setVisible(False)
    self.list.horizontalHeader().setVisible(False)
    self.list.setSelectionBehavior(QtGui.QAbstractItemView.SelectRows)
    self.list.setSelectionMode(QtGui.QAbstractItemView.SingleSelection)
    vb.addWidget(self.label)
    vb.addWidget(self.list)
    hb = QtGui.QHBoxLayout()
    hb.setDirection(QtGui.QBoxLayout.LeftToRight)
    cancel_btn = QtGui.QPushButton("Cancel", self)
    load_btn = QtGui.QPushButton("Select", self)
    hb.addStretch(1)
    hb.addWidget(cancel_btn)
    hb.addWidget(load_btn)
    vb.addItem(hb)
    load_btn.setDefault(True)
    QtCore.QObject.connect(load_btn, QtCore.SIGNAL("clicked()"), self.Select)
    QtCore.QObject.connect(cancel_btn, QtCore.SIGNAL("clicked()"), self.reject)

    row = 0
    for ent in self.ent_list_:
      variant = QtCore.QVariant(ent)
      self.list.insertRow(row)
      new_item = QtGui.QTableWidgetItem("%i"%(row+1))
      new_item.setFlags(QtCore.Qt.ItemIsSelectable|QtCore.Qt.ItemIsEnabled)
      new_item.setData(QtCore.Qt.UserRole,variant)
      self.list.setItem(row, 0, new_item)
      new_item = QtGui.QTableWidgetItem(ent.GetName())
      new_item.setFlags(QtCore.Qt.ItemIsSelectable|QtCore.Qt.ItemIsEnabled)
      self.list.setItem(row, 1, new_item)
      row += 1  

    self.list.resizeColumnsToContents()
    
  def Select(self):
    items = self.list.selectedItems()
    for item in items:
      if(item.column()==0):
        ent = item.data(QtCore.Qt.UserRole).toPyObject()
        self.ent_list_.remove(ent)
        self.ent_list_.insert(0,ent)
    self.accept()
      
  def GetEntities(self):
    return self.ent_list_

class ShowResultDialog(QtGui.QDialog):
  def __init__(self, ent_list, res_list, parent=None):
    QtGui.QDialog.__init__(self, parent)
    self.ent_list_ = ent_list
    vb = QtGui.QVBoxLayout()
    self.setLayout(vb)
    self.setWindowTitle("Alignment result")
    self.label = QtGui.QLabel("Alignment results with %s as reference"%ent_list[0].GetName())
    self.list = QtGui.QTableWidget(self)
    self.list.horizontalHeader().setStretchLastSection(True)
    self.list.setColumnCount(4)
    self.list.verticalHeader().setVisible(False)
    self.list.setHorizontalHeaderLabels (["Name","RMSD","TMScore",""])
    self.list.setSelectionBehavior(QtGui.QAbstractItemView.SelectRows)
    vb.addWidget(self.label)
    vb.addWidget(self.list)
    hb = QtGui.QHBoxLayout()
    hb.setDirection(QtGui.QBoxLayout.LeftToRight)
    ok_btn = QtGui.QPushButton("OK", self)
    hb.addStretch(1)
    hb.addWidget(ok_btn)
    vb.addItem(hb)
    ok_btn.setDefault(True)
    QtCore.QObject.connect(ok_btn, QtCore.SIGNAL("clicked()"), self.accept)

    for i in range(0, len(res_list)):
      self.list.insertRow(i)
      new_item = QtGui.QTableWidgetItem(ent_list[i+1].GetName())
      new_item.setFlags(QtCore.Qt.ItemIsEnabled)
      self.list.setItem(i, 0, new_item)
      new_item = QtGui.QTableWidgetItem("%.2f"%res_list[i].rmsd)
      new_item.setFlags(QtCore.Qt.ItemIsEnabled)
      self.list.setItem(i, 1, new_item)
      new_item = QtGui.QTableWidgetItem("%i"%res_list[i].tm_score)
      new_item.setFlags(QtCore.Qt.ItemIsEnabled)
      self.list.setItem(i, 2, new_item)
      new_item = QtGui.QTableWidgetItem()
      new_item.setFlags(QtCore.Qt.ItemIsEnabled)
      self.list.setItem(i, 3, new_item)
          
    self.list.resizeColumnsToContents()

class AlignmentContextMenu(QtCore.QObject):
  def __init__(self, context_menu):
    try:
      settings.Locate('tmalign')
      QtCore.QObject.__init__(self, context_menu.qobject)

      self.action = QtGui.QAction("Align", self)
      QtCore.QObject.connect(self.action,QtCore.SIGNAL("triggered()"), self.Align)
      context_menu.AddAction(self.action, gui.ContextActionType.ENTITY | gui.ContextActionType.MULTI)
    except settings.FileNotFound:
      return
    
  def Align(self):
    scene_selection = gui.SceneSelection.Instance()
    ent_list = list()
    for i in range(0,scene_selection.GetActiveNodeCount()):
      ent_list.append(scene_selection.GetActiveNode(i))
    if len(ent_list) == 2:
      self.__Align(ent_list)
    elif len(ent_list) > 2:
      sd = SelectRefDialog(ent_list)
      if(sd.exec_()):
        self.__Align(sd.GetEntities())
        
  def __Align(self, ent_list):
    node = ent_list[0]
    res_list = list()
    if isinstance(node, gfx.Entity):
      ref = node.view.handle
      for i in range(1,len(ent_list)):
        node = ent_list[i]
        if isinstance(node, gfx.Entity):
          res_list.append(tmtools.TMAlign(node.view.handle, ref))
          node.UpdatePositions()
    self.__ShowScore(ent_list, res_list)
    
  def __ShowScore(self, ent_list, res_list):
    if(len(res_list)==1):
      res = res_list[0]
      string = "RMSD: %.2f, TMScore: %i"%(res.rmsd, res.tm_score)
      gui.GostyApp.Instance().perspective.StatusMessage(string)
    elif(len(res_list)>1):
      ShowResultDialog(ent_list, res_list).exec_()
      
    
def _InitContextMenu():
  cm=gui.GostyApp.Instance().GetSceneWin().GetContextMenu()
  AlignmentContextMenu(cm)