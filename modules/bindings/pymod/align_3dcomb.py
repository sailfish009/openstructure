"""
3DCOMB module

Author: Niklaus Johner

This module is for structural alignment from OpenStructure using the external program 3DCOMB.

How To Use This Module:
 1. Import it (e.g. as "from ost.bindings import align_3dcomb")
 2. Use it (e.g. as "alignment,transformation_list = align_3dcomb.AlignStructures(view_list)")

Requirement:
 - 3DCOMB installed
"""

from ost.bindings import utils
import subprocess,os
from ost import settings
from ost import io
import ost
import ost.geom

def _GetExecutable(comb_exe, comb_env):
  """
    Function to check if 3DCOMB executable is present
    
    :param comb_exe: Explicit path to 3DCOMB executable
    :param msms_env: Environment variable pointing to 3DCOMB executable
    :returns: Path to the executable
    :raises:  :class:`~ost.FileNotFound` if executable is not found
    """
  return settings.Locate(['3DCOMB_linux','3DCOMB_win.exe'], explicit_file_name=comb_exe,
                         env_name=comb_env)

def _SetupFiles(structure_list):
  """
    Setup files for MSMS calculation in temporary directory
    
    :param structure_list: A list of EntityView that will be aligned.\
      each EntityView should contain a single chain and each residue needs to have a CA atom.
    :returns: calss:settings.TempDir
    :raises:  class:`RuntimeError` if on of the Views is not valid
  """
  
  #write out temporary pdb files
  if not all([ev.IsValid() for ev in structure_list]):
    raise RuntimeError, "Invalid EntityView in structure_list"
  tpd=utils.TempDirWithFiles(structure_list)
  
  #Write out the file containing the list of all structures
  outfile=open(os.path.join(tpd.dirname,'list'),'w')
  outfile.write('\n'.join(tpd.files))
  outfile.close()
  return tpd


def _Run3DCOMB(command,tpd):
  """
    Run the 3DCOMB alignment command
    
    This functions starts the external 3DCOMB executable and returns the stdout of
    3DCOMB.
    
    :param command:          Command to execute
    :returns:                 stdout of 3DCOMB
    :raises:              :class:`CalledProcessError` for non-zero return value
    """
  outname=os.path.join(tpd.dirname,'align.out')
  outfile=open(outname,'w')
  returncode=subprocess.call(command, shell=True, stdout=outfile)
  outfile.close()
  #check for successful completion of 3DCOMB
  if returncode!=0:
    print "WARNING: 3DCOMB error\n"
    raise subprocess.CalledProcessError
  return returncode

def _ParseOutput(tpd):
  #Read Alignment
  ali=io.LoadAlignment(os.path.join(tpd.dirname,'list.ali'),'fasta')
  for i in range(ali.GetCount()):
    ali.SetSequenceName(i,'struc{0}'.format(i))
  #Read Transformations
  f=open(os.path.join(tpd.dirname,'list.rmt'),'r')
  Tl=[]
  for l in f:
    if l.startswith('>'):
      fl=ost.FloatList()
      for i in range(3):
        l=f.next()
        sl=l.split(',')
        fl.extend([float(el) for el in sl[0].split()+[sl[1]]])
      fl.extend([0,0,0,1])
      T=ost.geom.Transform()
      M=ost.geom.Mat4(*fl)
      T.SetMatrix(M)
      Tl.append(T)
  #Read standard output
  outfile=open(os.path.join(tpd.dirname,'align.out'),'r')
  results={}
  for line in outfile:
    if line.startswith('Objective function value is'):results['objective_function']=float(line.split()[-1])
    if line.startswith('CORE_LEN'):
      l=line.split(',')
      for el in l:
        s=el.split('=')
        results[s[0]]=float(s[1])
  return ali,Tl,results


def AlignStructures(structure_list,apply_transform=False,name_list=None,comb_exe=None,comb_env=None):
  comb_executable=_GetExecutable(comb_exe, comb_env)
  tpd=_SetupFiles(structure_list)
  command=' '.join([comb_executable,os.path.join(tpd.dirname,'list')])
  returncode=_Run3DCOMB(command,tpd)
  try:ali,Tl,results=_ParseOutput(tpd)
  except:
    print 'could not parse output'
    raise RuntimeError
  if apply_transform:
    for T,ev in zip(Tl,structure_list):
      ev.handle.FixTransform()
      ev.handle.SetTransform(T)
  tpd.Cleanup()
  return ali,Tl,results




