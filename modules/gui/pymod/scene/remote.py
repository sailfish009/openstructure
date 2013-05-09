from PyQt4.QtCore import *
from PyQt4.QtGui import *
import threading
from ost import LogError
from ost import gui, gfx
from ost.io.remote import RemoteLoad, REMOTE_REPOSITORIES
import re

class RemoteLoader(QWidget):
  def __init__(self):
    QWidget.__init__(self)
    self._line = QLineEdit(self)
    self._load = QToolButton(self)
    self._load.setAttribute(Qt.WA_MacSmallSize)
    QObject.connect(self._line, SIGNAL('returnPressed()'),
                    self._DoLoad)
    QObject.connect(self._load, SIGNAL('clicked()'),
                    self._DoLoad)
    hbox = QHBoxLayout(self)
    hbox.addWidget(self._line,1)
    hbox.addWidget(self._load,0)
    self.setLayout(hbox)
    hbox.setMargin(3)
    hbox.setSpacing(3)
    self._menu = self._RemoteMenu()
    self._load.setMenu(self._menu)
    self._current_repo = 'pdb'
    self._UpdateLoadButton(self._current_repo)
    self.setFixedHeight(self._load.height())
    self.setSizePolicy(QSizePolicy.Expanding, QSizePolicy.Fixed)

  def _RemoteMenu(self):
    menu = QMenu()
    action_group = QActionGroup(menu)
    for k,v in REMOTE_REPOSITORIES.iteritems():
      action = menu.addAction(v.name)
      action.setCheckable(True)
      if k == 'pdb':
        action.setChecked(True)
      action.setData(QVariant(k))
      action_group.addAction(action)
    QObject.connect(menu, SIGNAL('triggered(QAction*)'),
                    self._ToggleRepo)
    return menu
  def _UpdateLoadButton(self, current):
    name = REMOTE_REPOSITORIES[current].name
    self._load.setText('load from %s  ' % name)

  def _ToggleRepo(self, action):
    self._current_repo = str(action.data().toString())
    self._UpdateLoadButton(self._current_repo)


  def _DoLoad(self):
    split_ids = []
    for p in str(self._line.text()).split(','):
      for k in p.split():
        if len(k.strip()):
          split_ids.append(k.strip())
    self._line.setText('')
    for split_id in split_ids:
      try:
        ent = RemoteLoad(split_id, from_repo=self._current_repo)
      except Exception, e:
        LogError(str(e))
        continue
      g = gfx.Entity(split_id, ent)
      try:
        gfx.Scene().Add(g)
      except Exception, e:
        LogError(str(e))

remote_loader=RemoteLoader()
remote_loader_for_panel=gui.Widget(remote_loader)
remote_loader_for_panel.qobject.setSizePolicy(QSizePolicy.Expanding, 
                                              QSizePolicy.Fixed)
panels=gui.GostyApp.Instance().perspective.panels
panels.AddWidgetToPool("Remote Loader",remote_loader_for_panel)
