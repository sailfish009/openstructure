'''
Naccess module

Author: Florian Kiefer, Gerardo Tauriello (cleanup/speedup)

This module is for calculating surface areas
from OpenStructure using the external program naccess

How To Use This Module:
 1. Import it (e.g. as "from ost.bindings import naccess")
 2. Use it (e.g. as "sasa = naccess.CalculateSurfaceArea(entity)")

Requirement:
 - naccess installed
'''

import tempfile
import subprocess
import os
import re
from ost import io, mol, settings, LogWarning

def _GetExecutable(naccess_exe):
  """
  Method to check if naccess executable is present

  :param naccess:   Explicit path to naccess executable
  :returns:         Path to the executable
  :exception:       FileNotFound if executable is not found
  """
  return settings.Locate('naccess', explicit_file_name=naccess_exe)

def _SetupFiles(entity, selection, scratch_dir, max_number_of_atoms):
  """
  Setup files for naccess calculation in temporary directory

  :param entity:      EntityHandle or EntityView to calculate surface
  :param selection:   Calculate surface for subset of entity
  :param scratch_dir: Directory for temporary files (NACCESS is sensitive to "." in directory names
  :param max_number_of_atoms: Max Number of atoms in the entity (i.e. is limited in the default NACCESS version to 50 000)
  :returns:            array containing temporary directory, input filename for naccess and directory of the input file
  :exception:         RuntimeError if selection is not valid
  """

  # create temporary directory
  tmp_dir_name = ""
  if scratch_dir != None:
    tmp_dir_name = tempfile.mkdtemp(dir=scratch_dir)
  else:
    tmp_dir_name = tempfile.mkdtemp()

  # select as specified by user
  if selection != "":
    entity_view = entity.Select(selection)
  else:
    entity_view = entity
  if len(entity_view.atoms) > max_number_of_atoms:
    raise RuntimeError, "Too much atoms for NACCESS (> %s)" % max_number_of_atoms
  if not entity_view.IsValid():
    raise RuntimeError, "Could not create view for selection (%s)"%(selection)
  
  # write entity to tmp file
  tmp_file_name = "entity.pdb"
  tmp_file_base = os.path.join(tmp_dir_name, "entity")
  io.SavePDB(entity_view, os.path.join(tmp_dir_name, tmp_file_name))
  return (tmp_dir_name, tmp_file_name, tmp_file_base)


def _ParseAsaFile(entity, file, asa_atom):
  """
  Reads Area file (.asa) and attach asa per atom to an entitiy

  :param entity:   EntityHandle or EntityView for attaching sasa on atom level
  :param file:     Filename of area file
  :param asa_atom: Name of the float property for SASA
  """
  
  asa_fh = open(file)
  asa_lines = asa_fh.readlines()
  asa_fh.close()
  
  for l in  asa_lines:
    if l.startswith("ATOM"):
      # get res_number, chain_id and atom name
      atom_name = l[12:16]
      chain_id = l[21]
      res_number = l[22:27]
      asa = l[54:63]
      atom_name = atom_name.strip()
      chain_id = chain_id
      res_number = res_number.strip()
      asa = asa.strip()
      #print "res_number:", res_number
      m=re.match(r'(?P<num>-?\d+)(?P<ins>\w)?', res_number)
      di = m.groupdict()
      
      if di["ins"] == None:
        resNum = mol.ResNum(int(di["num"]))
      else:
        resNum = mol.ResNum(int(di["num"]), di["ins"])
      #print res_number, resNum.num, resNum.ins
      a = entity.FindAtom(chain_id, resNum, atom_name)
      if(a.IsValid()):
        a.SetFloatProp(asa_atom, float(asa))
      else:
        print chain_id, resNum, atom_name
      
def _ParseRsaFile(enti,file, asa_abs, asa_rel):
  """
  Reads Area file (.rsa) and attach asa (absolute + relative) per residue to an entitiy

  :param entity:     EntityHandle or EntityView for attaching sasa on atom level
  :param file:       Filename of .rsa file
  :param asa_atom:   Name of the float property for absolute SASA
  :param asa_atom:   Name of the float property for relative SASA
  :exception:        RuntimeError if residue names are not the same
  """
  area_fh = open(file)
  area_lines = area_fh.readlines()
  area_fh.close()
  # shift first line
  area_lines = area_lines[4:]
  
  
  for l in area_lines:
    if l.startswith("RES"):
      p = re.compile(r'\s+')
      t = p.split(l)
      #res_name, chain_id , res_number, abs_all, rel_all = t[1:6]
      res_name = l[3:8]
      res_name = res_name.strip()
      chain_id = l[8:9]
      res_number = l[9:14]
      res_number= res_number.strip()
      #print l[15:30]
      abs_all, rel_all =l[15:28].strip().split()
      m=re.match(r'(?P<num>-?\d+)(?P<ins>\w)?', res_number)
      di = m.groupdict()
      if di["ins"] == None:
        resNum = mol.ResNum(int(di["num"]))
      else:
        resNum = mol.ResNum(int(di["num"]), di["ins"])
      
      res = enti.handle.FindResidue(chain_id, resNum)
      #res = entity.FindResidue(chain_id, mol.ResNum(int(res_number)))
      #print res_name, chain_id, res_number
      if res_name == res.name:
        res.SetFloatProp(asa_rel, float(rel_all) )
        res.SetFloatProp(asa_abs, float(abs_all) )
      else:
        raise RuntimeError, "Residue Names are not the same for ResNumb: %s (%s vs %s)" % (res_number, res.name, res_name)
      

def __CleanupFiles(dir_name):
  """
  Method which recursively deletes a directory

  :warning: This method removes also non-empty directories without asking, so
            be careful!
  """
  import shutil
  shutil.rmtree(dir_name)

def _RunNACCESS(command, temp_dir):
  """
  Method to run the MSMS surface calculation

  This method starts the external MSMS executable and returns the stdout of MSMS

  :param command:          Command to execute
  :returns:                stdout of MSMS
  :exception:              CalledProcessError for non-zero return value
  """
  proc = subprocess.Popen(command, shell=True, stdout=subprocess.PIPE,
                          cwd=temp_dir)
  stdout_value, stderr_value = proc.communicate()

  # check for successful completion of naccess
  if proc.returncode!=0:
    print "WARNING: naccess error\n", stdout_value
    raise subprocess.CalledProcessError(proc.returncode, command)

  return stdout_value
  

def CalculateSurfaceArea(entity,  radius=1.4,  
                         include_hydrogens=False, include_hetatm = False, 
                         include_water=False, selection="", naccess_exe=None,
                         keep_files=False, asa_abs= "asaAbs", asa_rel="asaRel",
                         asa_atom="asaAtom", scratch_dir=None,
                         max_number_of_atoms=50000):
  """
  Calculates analytical the solvent accessible surface area by using the
  external naccess program

  This method calculates the molecular surface areas by invoking the external
  program naccess. First, it is checked if the naccess executable is present, then,
  the necessary files are prepared in a temporary directory and naccess is
  executed. The last step is to remove the temporary directory.


  :param entity:              OST entity to calculate surface
  :param radius:              Surface probe radius
  :param include_hydrogens:   Calculate surface including hydrogens
  :param include_hetatm:      Calculate surface including hetatms
  :param include_water:       Calculate surface including water
  :param selection:           Calculate surface for subset of entity
  :param naccess_exe:         naccess executable (full path to executable)
  :param keep_files:          If True, do not delete temporary files
  :param asa_abs:             Attaches per residue absolute SASA to specified
                              FloatProp on residue level
  :param asa_rel:             Attaches per residue relative SASA to specified
                              FloatProp on residue level
  :param asa_atom:            Attaches per atom SASA to specified FloatProp at
                              atom level
  :param scratch_dir:         Scratch directory. A subfolder for temporary files
                              is created in there. If not specified, a default
                              directory is used (see :func:`tempfile.mkdtemp`).
  :param max_number_of_atoms: Max Number of atoms in the entity (i.e. is limited
                              in the default NACCESS version to 50 000)

  :returns:                   absolute SASA calculated using asa_atom
  """
  
  # check if naccess executable is specified
  naccess_executable=_GetExecutable(naccess_exe)
  
  # setup files for naccess
  (naccess_data_dir, naccess_data_file, naccess_data_base) \
    = _SetupFiles(entity, selection, scratch_dir, max_number_of_atoms)

  # set command line
  command = "%s %s -p %f " % \
            (naccess_executable, naccess_data_file, radius)
  if include_hydrogens:
    command = "%s -w" % command
  if include_water:
    command = "%s -y" % command
  if include_hetatm:
    command = "%s -h" % command
  # execute naccess
  stdout_value = _RunNACCESS(command, naccess_data_dir)
  
  # parse outout
  new_asa= os.path.join(naccess_data_dir, "%s.asa" % naccess_data_base) 
  _ParseAsaFile(entity, new_asa, asa_atom)
    
  new_rsa = os.path.join(naccess_data_dir, "%s.rsa" % naccess_data_base) 
  _ParseRsaFile(entity, new_rsa, asa_abs, asa_rel)
  
  # sum up Asa for all atoms
  sasa = 0.0 
  for a in entity.atoms:
    sasa += a.GetFloatProp(asa_atom, 0.0)
    
  # clean up
  if not keep_files:
    __CleanupFiles(naccess_data_dir)

  return sasa
