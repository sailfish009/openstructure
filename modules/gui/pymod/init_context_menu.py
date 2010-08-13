import platform

from PyQt4 import QtCore, QtGui

import sip

from subprocess import CalledProcessError

from ost import geom, gfx, gui, seq
from ost import settings
from ost import LogError
from ost.bindings import tmtools
from ost.bindings import msms
from ost.seq import alg

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
    self.show_scores = QtGui.QCheckBox(self)
    self.show_scores.setText("Show Scores")
    self.show_scores.setChecked(True)
    vb.addWidget(self.show_scores)
    self.show_alignment = QtGui.QCheckBox(self)
    self.show_alignment.setText("Display Alignment")
    self.show_alignment.setChecked(False)
    vb.addWidget(self.show_alignment)
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
     
  def GetShowScores(self):
    return self.show_scores.isChecked()
  
  def GetDisplayAlignment(self):
    return self.show_alignment.isChecked()
  
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

class CalculateSurfaceSettingsDialog(QtGui.QDialog):
  def __init__(self, executable, parent=None):
    QtGui.QDialog.__init__(self, parent)
    vb = QtGui.QGridLayout()
    self.setLayout(vb)
    self.setWindowTitle("MSMS Surface Settings")
    msmsexe_label=QtGui.QLabel("executable")
    self.msmsexe_field=QtGui.QLineEdit()
    self.msmsexe_field.setText(executable)
    msmsexe_browsebutton=QtGui.QPushButton("Browse")
    vb.addWidget(msmsexe_label, 0, 0)
    vb.addWidget(self.msmsexe_field, 0, 1)
    vb.addWidget(msmsexe_browsebutton, 0, 2)
    surfname_label=QtGui.QLabel("surface name")
    self.surfname_field=QtGui.QLineEdit()
    self.surfname_field.setText("surface")
    vb.addWidget(surfname_label, 1, 0)
    vb.addWidget(self.surfname_field, 1, 1, 1, 2)
    density_label=QtGui.QLabel("density")
    self.density_spinbox=QtGui.QSpinBox()
    self.density_spinbox.setRange(1, 10)
    self.density_spinbox.setValue(4)
    vb.addWidget(density_label, 2, 0)
    vb.addWidget(self.density_spinbox, 2, 1, 1, 2)
    probe_label=QtGui.QLabel("probe radius")
    self.probe_spinbox=QtGui.QDoubleSpinBox()
    self.probe_spinbox.setDecimals(1)
    self.probe_spinbox.setSingleStep(0.1)
    self.probe_spinbox.setRange(0.3, 5.0)
    self.probe_spinbox.setValue(1.4)
    vb.addWidget(probe_label, 3, 0)
    vb.addWidget(self.probe_spinbox, 3, 1, 1, 2)
    selection_label=QtGui.QLabel("selection")
    self.selection_field=QtGui.QLineEdit()
    self.selection_field.setText("")
    vb.addWidget(selection_label, 4, 0)
    vb.addWidget(self.selection_field, 4, 1, 1, 2)
    self.noh_box=QtGui.QCheckBox("no hydrogens")
    vb.addWidget(self.noh_box, 5, 0)
    self.nohet_box=QtGui.QCheckBox("no hetatoms")
    vb.addWidget(self.nohet_box, 5, 1)
    self.nowat_box=QtGui.QCheckBox("no waters")
    vb.addWidget(self.nowat_box, 5, 2)
    
    cancel_btn = QtGui.QPushButton("Cancel", self)
    ok_btn = QtGui.QPushButton("OK", self)
    vb.addWidget(cancel_btn, 6, 1)
    vb.addWidget(ok_btn, 6, 2)
    
    QtCore.QObject.connect(msmsexe_browsebutton, QtCore.SIGNAL("clicked()"), self.GetPath)
    QtCore.QObject.connect(ok_btn, QtCore.SIGNAL("clicked()"), self.accept)
    QtCore.QObject.connect(cancel_btn, QtCore.SIGNAL("clicked()"), self.reject)
    
  def GetPath(self):
    path=QtGui.QFileDialog().getOpenFileName(self, "Choose MSMS Executable")
    if path!='':
      self.msmsexe_field.setText(path)

class SurfaceContextMenu(QtCore.QObject):
  def __init__(self, context_menu):
    try:
      settings_name="msms"
      self.executable=settings.Locate(settings_name)
    except settings.FileNotFound:
      self.executable=""
    QtCore.QObject.__init__(self, context_menu.qobject)
    self.action = QtGui.QAction("Calculate Surface", self)
    QtCore.QObject.connect(self.action, QtCore.SIGNAL("triggered()"),
                           self.CalculateSurface)
    context_menu.AddAction(self.action, gui.ContextActionType.ENTITY)
    
  def CalculateSurface(self):
    scene_selection = gui.SceneSelection.Instance()
    ent_list = list()
    for i in range(0,scene_selection.GetActiveNodeCount()):
      ent_list.append(scene_selection.GetActiveNode(i))
    cssd = CalculateSurfaceSettingsDialog(self.executable)
    if(cssd.exec_()):
      self.__CalculateSurface(ent_list,
                              str(cssd.surfname_field.text()),
                              str(cssd.msmsexe_field.text()),
                              cssd.density_spinbox.value(),
                              cssd.probe_spinbox.value(),
                              str(cssd.selection_field.text()),
                              cssd.noh_box.isChecked(),
                              cssd.nohet_box.isChecked(),
                              cssd.nowat_box.isChecked())
      
  def __CalculateSurface(self,ent_list,name,msms_exe,density,
                         radius,selection,noh,nohet,nowat):
    for entity in ent_list:
      if isinstance(entity, gfx.Entity):
        try:
          s=msms.CalculateSurface(entity.view.handle,
                                  msms_exe=msms_exe,
                                  density=density,
                                  radius=radius,
                                  selection=selection,
                                  no_hydrogens=noh,
                                  no_hetatoms=nohet,
                                  no_waters=nowat)[0]
          gfx.Scene().Add(gfx.Surface("%s_%s"%(entity.GetName(),name),s))
        except (RuntimeError, CalledProcessError):
          LogError("WARNING: Surface could not be calculated")
          return
        except UserWarning:
          LogError("WARNING: Entry with the same name already present in scene")
          return
  

class AlignmentContextMenu(QtCore.QObject):

  def __init__(self, context_menu):
    try:
      try: # workaround for interrupted system call bug on OSX
        if platform.system() == "Windows":
          settings_name="tmalign.exe"
        else:
          settings_name="tmalign"
      except IOError:
        # if platform.system() fails with an IOError we are most likely on a buggy mac an therefore
        # use "tmalign"
        settings_name="tmalign"

      settings.Locate(settings_name)
      QtCore.QObject.__init__(self, context_menu.qobject)

      self.action = QtGui.QAction("Align", self)
      QtCore.QObject.connect(self.action,QtCore.SIGNAL("triggered()"), self.Align)
      context_menu.AddAction(self.action, gui.ContextActionType.ENTITY | gui.ContextActionType.MULTI)
      self.seq_viewer = None
    except settings.FileNotFound:
      return
    
  def Align(self):
    scene_selection = gui.SceneSelection.Instance()
    ent_list = list()
    for i in range(0,scene_selection.GetActiveNodeCount()):
      ent_list.append(scene_selection.GetActiveNode(i))
    sd = SelectRefDialog(ent_list)
    if(sd.exec_()):
      self.__Align(sd.GetEntities(),sd.GetShowScores(), sd.GetDisplayAlignment())
        
  def __Align(self, ent_list,show_scores=True, display_alignment=False):
    node = ent_list[0]
    res_list = list()
    if isinstance(node, gfx.Entity):
      ref = node.view.handle
      for i in range(1,len(ent_list)):
        node = ent_list[i]
        if isinstance(node, gfx.Entity):
          res_list.append(tmtools.TMAlign(node.view.handle, ref))
          node.UpdatePositions()
    if show_scores:
      self.__ShowScore(ent_list, res_list)
    if display_alignment:
      self.__DisplayAlignment(ent_list, res_list)
    
  def __ShowScore(self, ent_list, res_list):
    if(len(res_list)==1):
      res = res_list[0]
      string = "RMSD: %.2f, TMScore: %i"%(res.rmsd, res.tm_score)
      gui.GostyApp.Instance().perspective.StatusMessage(string)
    elif(len(res_list)>1):
      ShowResultDialog(ent_list, res_list).exec_()
      
  def __DisplayAlignment(self, ent_list, res_list):
    if(len(res_list)>0):
      ref_seq = seq.CreateSequence("%s (ref)"%ent_list[0].GetName(),res_list[0].ref_sequence.GetGaplessString())
      aln_list = seq.AlignmentList()
      if(ref_seq.IsValid()):
        for i in range(0, len(res_list)):
          res_list[i].alignment.SetSequenceName(1,ent_list[i+1].GetName())
          aln_list.append(res_list[i].alignment)
        alignment = alg.MergePairwiseAlignments(aln_list, ref_seq)
        gosty = gui.GostyApp.Instance()
        main_area = gosty.perspective.GetMainArea()
        if self.seq_viewer:
          self.seq_viewer.qobject.close()
        self.seq_viewer = gui.SequenceViewer(True)
        self.seq_viewer.AddAlignment(alignment)
        self.seq_viewer.ChangeDisplayMode("Highlight conservation 1")
        self.seq_viewer.Show()
        
def _InitContextMenu(app):
  cm=app.scene_win.GetContextMenu()
  AlignmentContextMenu(cm)
  SurfaceContextMenu(cm)
