import sys, os, platform
if platform.machine()=='x86_64':
  sys.path.insert(0, os.getenv('DNG_ROOT')+'/lib64/openstructure')
else:
  sys.path.insert(0,os.getenv('DNG_ROOT')+'/lib/openstructure')
     
from ost import io, mol, seq, gfx, conop
import ost
try: 
  from ost import iplt
except ImportError:
  pass
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

  
#gfx.set_offscreen_mode()
sys.ps1='ost> '
sys.ps2='..... '
print ''
sys.argv=sys.argv[1:]
home = os.getenv('HOME') or os.getenv('USERPROFILE')
_ostrc=os.path.join(home, '.ostrc')
if os.path.exists(_ostrc):
  try:
    exec(open(_ostrc))
  except Exception, e:
    print e

if(len(sys.argv)>0):
  script=sys.argv[0]
  execfile(script)

