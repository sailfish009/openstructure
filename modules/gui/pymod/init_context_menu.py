from PyQt4 import QtCore, QtGui

from ost import geom, gfx, gui
from ost import settings
from ost.bindings import tmtools
  
class AlignmentContextMenu(QtCore.QObject):
  def __init__(self, context_menu):
    try:
      settings.Locate('tmalign')
      QtCore.QObject.__init__(self, context_menu.qobject)

      self.action = QtGui.QAction("Align", self)
      QtCore.QObject.connect(self.action,QtCore.SIGNAL("triggered()"), self.Align)
      print gui.ContextActionType.ENTITY | gui.ContextActionType.MULTI
      context_menu.AddAction(self.action, gui.ContextActionType.ENTITY | gui.ContextActionType.MULTI)
    except settings.FileNotFound:
      return
    
  def Align(self):
    scene_selection = gui.SceneSelection.Instance()
    if scene_selection.GetActiveNodeCount() >= 2:
      node = scene_selection.GetActiveNode(0)
      if isinstance(node, gfx.Entity):
        ref = node.view.handle
        for i in range(1,scene_selection.GetActiveNodeCount()):
          node = scene_selection.GetActiveNode(i)
          if isinstance(node, gfx.Entity):
            print tmtools.TMAlign(node.view.handle, ref)
            node.UpdatePositions()
          
      
      
      

def _InitContextMenu():
  cm=gui.GostyApp.Instance().GetSceneWin().GetContextMenu()
  AlignmentContextMenu(cm)