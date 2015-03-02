#------------------------------------------------------------------------------
# This file is part of the OpenStructure project <www.openstructure.org>
#
# Copyright (C) 2008-2015 by the OpenStructure authors
#
# This library is free software; you can redistribute it and/or modify it under
# the terms of the GNU Lesser General Public License as published by the Free
# Software Foundation; either version 3.0 of the License, or (at your option)
# any later version.
# This library is distributed in the hope that it will be useful, but WITHOUT
# ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
# FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public License for more
# details.
#
# You should have received a copy of the GNU Lesser General Public License
# along with this library; if not, write to the Free Software Foundation, Inc.,
# 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
#------------------------------------------------------------------------------
"""
Wrapper for the CAD score.

References:

Olechnovic K, Kulberkyte E, Venclovas C., CAD-score: A new contact area 
difference-based function for evaluation of protein structural models
Proteins. 2012 Aug 30. [Epub ahead of print]

Authors: Valerio Mariani, Alessandro Barbato
"""

import subprocess, os, tempfile, platform
from ost import settings, io

def _SetupFiles(model,reference):
  # create temporary directory
  tmp_dir_name=tempfile.mkdtemp()
  dia = 'PDB'
  for chain in model.chains:
    if chain.name==" ":
      raise RuntimeError("One of the chains in the model has no name. Cannot calculate CAD score")
    if len(chain.name) > 1:
      dia = 'CHARMM'
      break;
    for res in chain.residues:
      if len(res.name) > 3:
        dia = 'CHARMM'
        break;
  io.SavePDB(model, os.path.join(tmp_dir_name, 'model.pdb'), dialect=dia)
  dia = 'PDB'
  for chain in reference.chains:
    if chain.name==" ":
      raise RuntimeError("One of the chains in the reference has no name. Cannot calculate CAD score")
    if len(chain.name) > 1:
      dia = 'CHARMM'
      break;
    for res in chain.residues:
      if len(res.name) > 3:
        dia = 'CHARMM'
        break;
  io.SavePDB(reference, os.path.join(tmp_dir_name, 'reference.pdb'),dialect=dia)
  return tmp_dir_name

def _CleanupFiles(dir_name):
  import shutil
  shutil.rmtree(dir_name)


class CADResult:
  """
  Holds the result of running CAD
  
  .. attribute:: globalAA
    
    The global CAD's atom-atom (AA) score
  
  .. attribute:: localAA
  
    Dictionary containing local CAD's atom-atom (AA) scores. 
    
    :type: dictionary (key: chain.resnum (e.g.: A.24), value: CAD local AA score (see CAD Documentation online)
  """
  def __init__(self, globalAA, localAA):    
    self.globalAA=globalAA
    self.localAA=localAA    

def _ParseCADGlobal(lines):
  interesting_lines=lines[1]
  aa=float(interesting_lines.split()[10])
  return aa

def _ParseCADLocal(lines):
  local_aa_dict={}
  for lin in lines[11:]:
    items=lin.split()
    chain=items[0]
    resnum=int(items[1])
    key=chain+'.'+str(resnum)
    aa=float(items[2])
    local_aa_dict[key]=aa
  return local_aa_dict

def _RunCAD(max_iter, tmp_dir):
  model_filename=os.path.join(tmp_dir, 'model.pdb')
  reference_filename=os.path.join(tmp_dir, 'reference.pdb')
  if platform.system() == "Windows":
    raise RuntimeError('CAD score not available on Windows')
  else:
    cad_calc_path=settings.Locate('CADscore_calc.bash')  
    cad_read_g_path=settings.Locate('CADscore_read_global_scores.bash')  
    cad_read_l_path=settings.Locate('CADscore_read_local_scores.bash')  
    command1="\"%s\" -o %i -m \"%s\" -t \"%s\" -D \"%s\"" %(cad_calc_path, max_iter, model_filename, reference_filename, os.path.join(tmp_dir,"cadtemp"))
    command2="\"%s\" -D \"%s\"" %(cad_read_g_path, os.path.join(tmp_dir,"cadtemp"))
    command3="\"%s\" -m \"%s\" -t \"%s\" -D \"%s\" -c AA" %(cad_read_l_path, model_filename, reference_filename,os.path.join(tmp_dir,"cadtemp"))
  ps1=subprocess.Popen(command1, shell=True, stdout=subprocess.PIPE)
  ps1.wait()
  ps2=subprocess.Popen(command2, shell=True, stdout=subprocess.PIPE)
  ps2.wait()
  lines=ps2.stdout.readlines()
  try:
    globalAA=_ParseCADGlobal(lines)
  except:
    raise RuntimeError("CAD calculation failed")
  ps3=subprocess.Popen(command3, shell=True, stdout=subprocess.PIPE)
  ps3.wait()
  lines=ps3.stdout.readlines()
  try:
    localAA=_ParseCADLocal(lines)
  except:
    raise RuntimeError("CAD calculation failed")

  return CADResult(globalAA,localAA)


def CADScore(model, reference, max_iter=300):
  """
  Calculates global and local atom-atom (AA) CAD Scores
  

  :param model: The model structure. 
  :type model: :class:`~ost.mol.EntityView` or :class:`~ost.mol.EntityHandle`
  :param reference: The reference structure
  :type model2: :class:`~ost.mol.EntityView` or :class:`~ost.mol.EntityHandle`
  :param max_iter: Optional. The maximum number of iteration for the tessellation algorithm before giving up. By default 300
  :returns: The result of the CAD score calculation
  :rtype: :class:`CADResult`
  
  :raises: :class:`~ost.settings.FileNotFound` if any of the CAD score exacutables could not be located.
  :raises: :class:`RuntimeError` if the calculation failed
  """
  tmp_dir_name=_SetupFiles(model, reference)
  result=_RunCAD(max_iter, tmp_dir_name)
  _CleanupFiles(tmp_dir_name)
  return result

