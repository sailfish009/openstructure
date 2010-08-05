from ost import mol
from PyQt4 import QtCore, QtGui

class QueryEditorWidget(QtGui.QWidget):
   
  def __init__(self, parent=None): 
    QtGui.QWidget.__init__(self, parent)
    self.default_font_= QtGui.QTextCharFormat()
    self.default_font_.setForeground(QtGui.QBrush(QtGui.QColor(0,0,0)))
    self.error_font_= QtGui.QTextCharFormat()
    self.error_font_.setForeground(QtGui.QBrush(QtGui.QColor(255,0,0))) 
    self.selection_edit_ = QtGui.QTextEdit(self)
    self.selection_edit_.setMinimumHeight(50)
    self.selection_edit_.setSizePolicy(QtGui.QSizePolicy(QtGui.QSizePolicy.Expanding,QtGui.QSizePolicy.MinimumExpanding))
    self.selection_edit_.updateGeometry()
    selection_label = QtGui.QLabel("Selection",self)
        
    self.status_=QtGui.QLabel("",self);
    self.status_.setWordWrap(True)
    self.status_.setMargin(0)
    self.status_.setHidden(True)
    self.status_.setAlignment(QtCore.Qt.AlignRight)
    self.status_.setSizePolicy(QtGui.QSizePolicy(QtGui.QSizePolicy.Minimum,QtGui.QSizePolicy.Expanding))
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
    grid.addWidget(selection_label,0,0,1,1,QtCore.Qt.AlignTop|QtCore.Qt.AlignLeft)
    grid.addWidget(self.selection_edit_,0,1,1,1)
    grid.addWidget(self.status_,1,0,1,2,QtCore.Qt.AlignRight)
    grid.addWidget(flag_label,2,0,3,1,QtCore.Qt.AlignTop|QtCore.Qt.AlignLeft)
    grid.addLayout(v_checkbox_layout,2,1,3,1)
    grid.setRowStretch(0,1)
    self.changing_text_=False;
    self.connect(self.selection_edit_,QtCore.SIGNAL("textChanged()"),self.UpdateMessage)
    
  def GetQueryFlags(self):
    flag = 0
    for k,v in self.checkboxes_.iteritems():
      if k.isChecked():
        flag += v
    return flag
  
  def GetQuery(self):
    return mol.Query(str(self.selection_edit_.toPlainText()))

  def GetQueryText(self):
    return str(self.selection_edit_.toPlainText())
  
  def SetQueryFlags(self,flags):
    for k,v in self.checkboxes_.iteritems():
      if (flags & v)>0:
        k.setChecked(True)
      else:
        k.setChecked(False)
  
  def SetQuery(self,query):
    self.selection_edit_.setText(query)
    
  def UpdateMessage(self):
    if self.changing_text_:
      return
    self.changing_text_ = True
    query=self.GetQuery()
    
    cursor=self.selection_edit_.textCursor()
    cursor.select(QtGui.QTextCursor.Document)
    cursor.setCharFormat(self.default_font_)  
    
    if query.IsValid():
      self.status_.setText("")
      self.status_.setHidden(True)
    else:      
      d=query.GetErrorDescription()
      self.status_.setText("<font color='red'>%s</font>"%d.msg)
      self.status_.setVisible(True)
      self.status_.setFixedSize(self.width(),self.status_.height())
      
      cursor.movePosition(QtGui.QTextCursor.Start)
      cursor.movePosition(QtGui.QTextCursor.NextCharacter, QtGui.QTextCursor.MoveAnchor, d.range.Loc)
      cursor.movePosition(QtGui.QTextCursor.NextCharacter, QtGui.QTextCursor.KeepAnchor, d.range.Length)
      cursor.setCharFormat(self.error_font_)
    self.changing_text_ = False