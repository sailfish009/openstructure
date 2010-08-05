from ost import mol
from PyQt4 import QtCore, QtGui

class QueryEditorWidget(QtGui.QWidget):
   
  def __init__(self, parent=None): 
    QtGui.QWidget.__init__(self, parent)
    self.selection_edit_ = QtGui.QLineEdit()
    selection_label = QtGui.QLabel("Selection",self)    
    self.status_=QtGui.QLabel("",self);
    self.status_.setWordWrap(True)
    self.status_.setMargin(0)
    self.status_.setHidden(True)
    
    self.checkboxes_ = dict()
    for k,v in mol.QueryFlag.__dict__["values"].iteritems():
      checkbox = QtGui.QCheckBox(str(v),self)
      self.checkboxes_[checkbox]=k
      
    v_checkbox_layout = QtGui.QVBoxLayout()
    v_checkbox_layout.setMargin(0)
    v_checkbox_layout.setSpacing(0)
    for k,v in self.checkboxes_.iteritems():
      v_checkbox_layout.addWidget(k)
    
    flag_label = QtGui.QLabel("Query flags")       
    grid = QtGui.QGridLayout(self)
    grid.setContentsMargins(0,5,0,0)
    grid.addWidget(selection_label,0,0,1,1)
    grid.addWidget(self.selection_edit_,0,1,1,1)
    grid.setRowMinimumHeight(1,0)
    grid.addWidget(self.status_,1,0,1,2)
    grid.addWidget(flag_label,2,0,1,1)
    grid.addLayout(v_checkbox_layout,2,1,1,1)
    grid.setRowStretch(2, 1)
    #group_box = QtGui.QGroupBox("Selection",self)
    #group_box.setLayout(h_layout)
        
    self.connect(self.selection_edit_,QtCore.SIGNAL("textChanged(const QString& )"),self.UpdateMessage)
    
  def GetQueryFlags(self):
    flag = 0
    for k,v in self.checkboxes_.iteritems():
      if k.isChecked():
        flag += v
    return flag
  
  def GetQuery(self):
    return mol.Query(str(self.selection_edit_.text()))

  def GetQueryText(self):
    return str(self.selection_edit_.text())
  
  def SetQueryFlags(self,flags):
    for k,v in self.checkboxes_.iteritems():
      if (flags & v)>0:
        k.setChecked(True)
      else:
        k.setChecked(False)
  
  def SetQuery(self,query):
    self.selection_edit_.setText(query)
    
  def UpdateMessage(self):
    q=self.GetQuery()
    if q.IsValid():
      self.status_.setText("")
      self.status_.setHidden(True)
      
    else:
      d=q.GetErrorDescription()
      self.status_.setText("<font color='red'>%s</font>"%d.msg)
      self.status_.setVisible(True)