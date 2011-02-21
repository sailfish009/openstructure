from PyQt4.QtGui import *
from PyQt4.QtCore import *
from ost import gui
class DocWidget(QWidget):
  def __init__(self, parent=None):
    QWidget.__init__(self, parent)
    l=QVBoxLayout(self)
    l.setMargin(0)
    self.setWindowTitle('OpenStructure Help')
    self.searchbox=QLineEdit(self)
    l.addWidget(self.searchbox)
    self.doctext=QTextEdit('', self)
    l.addWidget(self.doctext)
    self.setLayout(l)
    QObject.connect(self.searchbox, SIGNAL('returnPressed()'),
                    self.OnReturnPressed)
  def OnReturnPressed(self):
    help(str(self.searchbox.text()))
doc_widget=DocWidget()
doc_widget_for_panel=gui.Widget(doc_widget)
panels=gui.GostyApp.Instance().perspective.panels
panels.AddWidgetToPool("Doc",doc_widget_for_panel)


class Help:
  def __call__(self, what=None):
    import pydoc
    d=pydoc.HTMLDoc()
    if what==None:
      doc_widget.doctext.setHtml('')
      doc_widget.show()
      doc_widget.searchbox.setText('')
      return
    s=d.document(pydoc.resolve(what)[0])
    doc_widget.doctext.setHtml(s)
    doc_widget.searchbox.setText('')
    if not doc_widget.isVisible():
      panels.AddWidget(gui.PanelPosition.RIGHT_PANEL, doc_widget_for_panel, False)
  def __repr__(self):
    return 'Type help() for interactive help, or help(object) for help about object.'

help=Help()
