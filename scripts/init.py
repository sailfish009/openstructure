import __main__
import sys
import optparse
from ost import io, mol, seq, geom, conop, gui
import ost
try: 
  from ost import img
  import ost.img.alg
  _img_present=True
except ImportError:
  _img_present=False
  pass
import httplib

from PyQt4 import QtGui, QtCore
from ost.gui.scene.init_inspector import _InitInspector
from ost.gui.init_menubar import _InitMenuBar
from ost.gui.init_spacenav import _InitSpaceNav
from ost.gui.init_context_menu import _InitContextMenu

def _InitRuleBasedBuilder():
  compound_lib_path=os.path.join(ost.GetSharedDataPath(), 'compounds.chemlib')
  if os.path.exists(compound_lib_path):
    conop_inst=conop.Conopology.Instance()
    compound_lib=conop.CompoundLib.Load(compound_lib_path)
    conop_inst.RegisterBuilder(conop.RuleBasedBuilder(compound_lib), 'RBB')
    conop_inst.SetDefaultBuilder('RBB')

# switch to rule-based builder for high fidelity if compounds.chemlib is 
# available
_InitRuleBasedBuilder()
def _InitPanels(app):
  panels = app.perspective.panels
  panels.AddWidgetToPool('ost.gui.FileBrowser', -1)
  panels.AddWidgetToPool('ost.gui.PythonShell', 1)
  panels.AddWidgetToPool('ost.gui.RemoteLoader', -1)
  panels.AddWidgetToPool('ost.gui.SceneWin', 1)
  panels.AddWidgetToPool('ost.gui.SequenceViewer', 1)
  if not panels.Restore("ui/perspective/panels"):
    panels.AddWidget(gui.PanelPosition.LEFT_PANEL, app.scene_win)
    panels.AddWidgetByName(gui.PanelPosition.LEFT_PANEL, 
                           'ost.gui.FileBrowser', False)    
    panels.AddWidgetByName(gui.PanelPosition.LEFT_PANEL, 
                           'ost.gui.RemoteLoader', False)
    panels.AddWidget(gui.PanelPosition.BOTTOM_PANEL, app.seq_viewer)
    panels.AddWidget(gui.PanelPosition.BOTTOM_PANEL, app.py_shell)

def _InitFrontEnd():
  app=gui.GostyApp.Instance()
  app.SetAppTitle("DNG")
  main_area=app.perspective.main_area
  _InitPanels(app)
  _InitMenuBar(app)
  _InitSpaceNav(app)
  _InitContextMenu(app)
  main_area.AddPersistentWidget("3D Scene", "gl_win" , app.gl_win, int(QtCore.Qt.WindowMaximized))
  app.perspective.Restore()
  additional_modules=getattr(__main__, 'ADDITIONAL_GUI_MODULES', [])
  for module_name in additional_modules:
    __import__(module_name)
  app.ProcessEvents()
  _InitInspector(app)

def _load_files():
  for pdb_id in options.pdb_ids:
    pdb_id, sel=_SplitIDSel(pdb_id)
    selection=_get_selection_query(sel)
    gui.FileLoader.LoadFrom(pdb_id,"pdb.org",selection)
    
  input_files=[_SplitIDSel(arg) for arg in loading_list]
  for f in input_files:
    selection=_get_selection_query(f[1])
    gui.FileLoader.LoadObject(f[0],selection)

def _get_selection_query(sel):
  if len(options.query)>0:
    if len(sel)>0:
      return '(%s) and (%s)' % (options.query, sel)
    else:
      return options.query
  elif len(sel)>0:
    return sel
  return ""

def _execute_script():
  script=script_argv[0]
  sys_argv_backup=sys.argv
  sys.argv=script_argv
  try:
    execfile(script, __main__.__dict__)
  finally:
    sys.argv=sys_argv_backup     

def show_help(option, opt, value, parser):
  parser.print_help()
  QtGui.QApplication.instance().exit()
  sys.exit(-1)

def parse_script_option(option, opt, value, parser):
  script_argv.append(value)
  for arg in parser.rargs:
     script_argv.append(arg)
  del parser.rargs[0:len(parser.rargs)]

def _SplitIDSel(name):
  pos=name.find('[')
  if pos>-1:
    return name[:pos], name[pos+1:-1]
  return name, ''  

loading_list=[]
script_argv=[]
images=[]
viewers=[]
usage = 'usage: dng [options] [files to load]'
class OstOptionParser(optparse.OptionParser):
  def __init__(self, **kwargs):
    optparse.OptionParser.__init__(self, **kwargs)
  def exit(self, status_code, error_message):
    print error_message,
    QtGui.QApplication.instance().exit()
    sys.exit(-1)

parser=OstOptionParser(usage=usage,conflict_handler="resolve")
parser.add_option("-h", "--help", action="callback", callback=show_help, help="show this help message and exit")
parser.add_option("-v", "--verbosity_level", action="store", type="int", dest="vlevel", default=1, help="sets the verbosity level [default: %default]")
parser.add_option("-s", "--script", action="callback", default=[], dest="script", type="string", callback=parse_script_option, help="executes a script (syntax: -s SCRIPT [options] [args]) Anything that follows this option is passed to the script")
parser.add_option("-p", "--pdb_id", dest="pdb_ids", default=[],action="append", help="PDB file ID. The file will be retrieved from PDB")
parser.add_option("-b", "--builder", dest="builder", default="HEURISTIC", help="Type of builder used by the progam (either RULE_BASED or HEURISTIC) [default: %default]")
parser.add_option("-c", "--compound_library", dest="complib", default="compounds.chemlib", help="Compound library for the RULE_BASED builder (only used if --builder option is set to RULE_BASED, otherwise ignored [default: %default]")
parser.add_option("-q", "--query", dest="query", default="", help="Selection query to be highlighted automatically upon loading (only used together with -p option or when a PDB file is loaded, otherwise ignored [default: None]")
parser.disable_interspersed_args()
(options, args) = parser.parse_args()

if len(parser.rargs)!=0:
  for rargs_string in parser.rargs:
    if not rargs_string.endswith('.py'):  
      loading_list.append(rargs_string)
    else:
      print 'Error:  one of the files to load is a Python script, use -s flag to execute it\n'
      QtGui.QApplication.instance().exit()
      sys.exit(-1)    

if len(options.script)!=0:
  script_argv=options.script

if options.builder=="RULE_BASED":
  from ost import conop
  compound_lib=conop.CompoundLib.Load(options.complib)
  rbb=conop.RuleBasedBuilder(compound_lib)
  conop.Conopology.Instance().RegisterBuilder(rbb,'rbb')
  conop.Conopology.Instance().SetDefaultBuilder('rbb')

PushVerbosityLevel(options.vlevel)
_InitFrontEnd()

if len(loading_list)!=0 or len(options.pdb_ids)!=0:
  _load_files()
  scene.Autoslab()
if len(script_argv)!=0:
  _execute_script()

