import sys, os, platform
import optparse

interactive=False

def show_help(option, opt, value, parser):
  parser.print_help()
  sys.exit(-1)

def interactive_flag(option, opt, value, parser):
  pass

usage = 'usage: ost [ost options] [script to execute] [script parameters]'
class OstOptionParser(optparse.OptionParser):
  def __init__(self, **kwargs):
    optparse.OptionParser.__init__(self, **kwargs)
  def exit(self, status_code, error_message):
    print error_message,
    QtGui.QApplication.instance().exit()
    sys.exit(-1)

parser=OstOptionParser(usage=usage,conflict_handler="resolve")
parser.add_option("-i", "--interactive", action="callback", callback=interactive_flag, help="start interpreter interactively (must be first parameter, ignored otherwise)")
parser.add_option("-h", "--help", action="callback", callback=show_help, help="show this help message and exit")
parser.add_option("-v", "--verbosity_level", action="store", type="int", dest="vlevel", default=0, help="sets the verbosity level [default: %default]")
parser.disable_interspersed_args()
(options, args) = parser.parse_args()

if platform.machine()=='x86_64':
  sys.path.insert(0, os.getenv('DNG_ROOT')+'/lib64/openstructure')
else:
  sys.path.insert(0,os.getenv('DNG_ROOT')+'/lib/openstructure')
     
from ost import *
import ost

ost.SetPrefixPath(os.getenv('DNG_ROOT'))
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
import os.path
HistoryFile=os.path.expanduser('~/.ost_history')
InGUIMode=False

sys.ps1='ost> '
sys.ps2='..... '
sys.argv=sys.argv[1:]
home = os.getenv('HOME') or os.getenv('USERPROFILE')
_ostrc=os.path.join(home, '.ostrc')
if os.path.exists(_ostrc):
  try:
    exec(open(_ostrc))
  except Exception, e:
    print e
else:
  rcfile=open(_ostrc,"w")
  print >> rcfile, '# This python file is parsed by ost and dng at startup'
  print >> rcfile, '# Its content is be made available in the global namespace'
  print >> rcfile, '# It can be used to define custom variables and functions'
  print >> rcfile, '# For example:'
  print >> rcfile, '# IMPORTANT_DIR="path/to/important/dir"'
  rcfile.close()

PushVerbosityLevel(options.vlevel)

sys.path.append(".")

if len(parser.rargs)>0 :
  script=parser.rargs[0]	
  sys_argv_backup=sys.argv
  sys.argv=parser.rargs
  try:
    execfile(script)
  finally:
    sys.argv=sys_argv_backup

