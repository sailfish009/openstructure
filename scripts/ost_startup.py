import sys, os, platform
import optparse

def show_help(option, opt, value, parser):
  parser.print_help()
  sys.exit(-1)

def interactive_flag(option, opt, value, parser):
  pass

def stop():
  sys.exit(0)

usage = 'usage: ost [ost options] [script to execute] [script parameters]'
class OstOptionParser(optparse.OptionParser):
  def __init__(self, **kwargs):
    optparse.OptionParser.__init__(self, **kwargs)
  def exit(self, status_code, error_message):
    print error_message,
    sys.exit(-1)

parser=OstOptionParser(usage=usage,conflict_handler="resolve", prog='ost''')
parser.add_option("-i", "--interactive", action="callback", callback=interactive_flag, help="start interpreter interactively (must be first parameter, ignored otherwise)")
parser.add_option("-h", "--help", action="callback", callback=show_help, help="show this help message and exit")
parser.add_option("-v", "--verbosity_level", action="store", type="int", dest="vlevel", default=2, help="sets the verbosity level [default: %default]")
parser.disable_interspersed_args()
(options, args) = parser.parse_args()

_site_packs='python%d.%d/site-packages' % sys.version_info[0:2]
_base_dir=os.getenv('DNG_ROOT')
if platform.machine()=='x86_64':
  sys.path.insert(0, os.path.join(_base_dir, 'lib64', _site_packs))
else:
  sys.path.insert(0,os.path.join(_base_dir, 'lib', _site_packs))
     
from ost import *
import ost
try:
  from ost import gfx
  ost.scene = gfx.Scene()
  scene=gfx.Scene()
  ost.scene.Stereo=gfx.Stereo
  scene.Stereo=gfx.Stereo
except ImportError:
  print "ost build without gfx"
  pass

ost.SetPrefixPath(_base_dir)
def _InitRuleBasedBuilder():
  compound_lib_path=os.path.join(ost.GetSharedDataPath(), 'compounds.chemlib')
  if os.path.exists(compound_lib_path):
    compound_lib=conop.CompoundLib.Load(compound_lib_path)
    conop.RegisterBuilder(conop.RuleBasedBuilder(compound_lib), 'RBB')
    conop.SetDefaultBuilder('RBB')

# switch to rule-based builder for high fidelity if compounds.chemlib is 
# available
_InitRuleBasedBuilder()
import os.path
HistoryFile=os.path.expanduser('~/.ost_history')

# we are not in GUI mode. 
gui_mode=False

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

# this should probably only be added when running an interactive shell
sys.path.append(".")

if len(parser.rargs)>0 :
  script=parser.rargs[0]	
  sys_argv_backup=sys.argv
  sys.argv=parser.rargs
  try:
    execfile(script)
  finally:
    sys.argv=sys_argv_backup

