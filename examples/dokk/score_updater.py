import os, threading
from PyQt4 import QtCore, QtGui
from ost import qa

class ScoreUpdater(QtCore.QObject):
  def __init__(self, level, parent=None):
    QtCore.QObject.__init__(self, parent)
    self.level = level


  def UpdateScores(self):
    ligand = self.level.ligand
    prot_within=self.level.protein.handle.FindWithin(ligand.GetCenter(), 
                                             ligand.radius)
    lig_view=ligand.handle.CreateFullView()
    for a in prot_within:
      score=qa.ClashScore(a, lig_view)
      a.SetGenericFloatProperty('clash', score)
    self.level.surface.go.ReapplyColorOps()
