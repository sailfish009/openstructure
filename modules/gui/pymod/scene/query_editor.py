from ost import mol
from PyQt4 import QtCore, QtGui

class QueryEditorWidget(QtGui.QWidget):
  def __init__(self, parent=None): 
    QtGui.QWidget.__init__(self, parent)
    self.default_font_=QtGui.QTextCharFormat()
    self.default_font_.setForeground(QtGui.QBrush(QtGui.QColor(0,0,0)))
    self.error_font_=QtGui.QTextCharFormat()
    self.error_font_.setForeground(QtGui.QBrush(QtGui.QColor(255,0,0))) 
    self.selection_edit_=QtGui.QTextEdit(self)
    self.selection_edit_.setFixedHeight(40)
    self.selection_edit_.updateGeometry()
    self.status_=QtGui.QLabel(" ",self);
    self.status_.setWordWrap(True)
    self.status_.setMargin(0)
    self.status_.setAlignment(QtCore.Qt.AlignRight)
    self.status_.setSizePolicy(QtGui.QSizePolicy(QtGui.QSizePolicy.Minimum,
                                                 QtGui.QSizePolicy.Expanding))
    vl=QtGui.QVBoxLayout()
    vl.addWidget(self.selection_edit_)
    self.no_bonds_=QtGui.QRadioButton('none')
    self.ex_bonds_=QtGui.QRadioButton('exclusive')
    self.in_bonds_=QtGui.QRadioButton('inclusive')
    self.in_bonds_.setChecked(True)
    self.match_res_=QtGui.QCheckBox('match residues')
    
    vl.setMargin(0)
    vl.setSpacing(0)
    self.setLayout(vl)
    vl.addWidget(self.status_)
    hl=QtGui.QHBoxLayout()
    l=QtGui.QLabel("bonds:")

    hl.addWidget(l)
    hl.addSpacing(5)
    hl.addWidget(self.no_bonds_)
    hl.addWidget(self.ex_bonds_)
    hl.addWidget(self.in_bonds_)
    hl.addStretch(1)
    vl.addLayout(hl)
    vl.addWidget(self.match_res_)

    self.changing_text_=False;
    self.connect(self.selection_edit_,QtCore.SIGNAL("textChanged()"),
                 self._StartTimer)
    self.timer_=QtCore.QTimer()
    QtCore.QObject.connect(self.timer_, QtCore.SIGNAL('timeout()'),
                           self._UpdateMessage)
    
  def GetQueryFlags(self):
    flags=0
    if self.no_bonds_.isChecked():
      flags|=mol.NO_BONDS
    if self.ex_bonds_.isChecked():
      flags|=mol.EXCLUSIVE_BONDS
    if self.match_res_.isChecked():
      flags|=mol.MATCH_RESIDUES
    return flags
  
  def GetQuery(self):
    return mol.Query(str(self.selection_edit_.toPlainText()))

  def GetQueryText(self):
    return str(self.selection_edit_.toPlainText())
  
  def SetQueryFlags(self,flags):
    self.no_bonds_.setChecked(flags & mol.NO_BONDS)
    self.ex_bonds_.setChecked(flags & mol.EXCLUSIVE_BONDS)
    self.in_bonds_.setChecked(not (flags & mol.NO_BONDS|mol.EXCLUSIVE_BONDS))
    self.match_res_.setChecked(flags & mol.MATCH_RESIDUES)
  
  def SetQuery(self,query):
    self.selection_edit_.setText(query)
    
  def _StartTimer(self):
    self.timer_.stop()
    self.timer_.start(500)

  def _UpdateMessage(self):
    if self.changing_text_:
      return
    self.changing_text_ = True
    query=self.GetQuery()
    
    cursor=self.selection_edit_.textCursor()
    cursor.select(QtGui.QTextCursor.Document)
    cursor.setCharFormat(self.default_font_)  
    
    if query.IsValid():
      self.status_.setText("")
    else:      
      d=query.GetErrorDescription()
      self.status_.setText("<font color='red'>%s</font>"%d.msg)
      self.status_.setFixedSize(self.width(),self.status_.height())
      cursor.movePosition(QtGui.QTextCursor.Start)
      if d.range.Loc<len(query.string):
        
        cursor.movePosition(QtGui.QTextCursor.NextCharacter, 
                            QtGui.QTextCursor.MoveAnchor, d.range.Loc)
        cursor.movePosition(QtGui.QTextCursor.NextCharacter, 
                            QtGui.QTextCursor.KeepAnchor, d.range.Length)
        cursor.setCharFormat(self.error_font_)
    self.changing_text_=False
    
class QueryDialog(QtGui.QDialog):
  def __init__(self, title, parent=None):
    QtGui.QDialog.__init__(self, parent)
    l=QtGui.QVBoxLayout(self)
    self.setWindowTitle(title)
    self.editor=QueryEditorWidget(self)
    l.addWidget(self.editor)
    l.addSpacing(10)
    l3=QtGui.QHBoxLayout()
    ab=QtGui.QPushButton('OK')
    ab.setDefault(True)
    cb=QtGui.QPushButton('Cancel')
    l3.addStretch(1)
    l3.addWidget(cb, 0)
    l3.addWidget(ab, 0)
    l.addLayout(l3)
    QtCore.QObject.connect(cb, QtCore.SIGNAL('clicked()'), self.reject)
    QtCore.QObject.connect(ab, QtCore.SIGNAL('clicked()'), self.accept)
    self.connect(self.editor.selection_edit_,QtCore.SIGNAL("textChanged()"),self._CheckNewline)

  @property 
  def query_flags(self):
    return self.editor.GetQueryFlags()
  @property
  def query(self):
    return self.editor.GetQueryText()
  
  def event(self, e):
    if e.type() == QtCore.QEvent.KeyPress and (e.key () == QtCore.Qt.Key_Return or e.key () == QtCore.Qt.Key_Enter):
      self.accept()
      return True
    else:
      return QtGui.QDialog.event(self, e)
  
  def _CheckNewline(self):
    if self.editor.GetQueryText().endswith("\n"):
      self.accept()