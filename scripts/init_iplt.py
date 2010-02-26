from ost import gui
import sip
from ost.iplt import *
import ost.iplt.alg as alg

from PyQt4 import QtGui, QtCore
from ost.gui.init_menubar import _InitMenuBar


def Viewer(image,title=""):
  app=gui.GostyApp.Instance()
  viewer=app.CreateDataViewer(image)
  app.perspective.main_area.AddWidget(title, viewer)
  return viewer

def _InitPanels(app, panels):
  panels.AddWidgetToPool('ost.gui.PythonShell', 1)
  if not panels.Restore("iplt/ui/perspective/panels"):
    panels.AddWidget(gui.PanelPosition.BOTTOM_PANEL, app.py_shell)

def _InitIPLTNextGen():
  app=gui.GostyApp.Instance()
  app.SetAppTitle("IPLT - Iplt Next Generation")
  main_area=app.perspective.main_area
  _InitPanels(app, app.perspective.panels)
  _InitMenuBar(app.perspective.GetMenuBar())
  app.perspective.Restore()

_InitIPLTNextGen()
