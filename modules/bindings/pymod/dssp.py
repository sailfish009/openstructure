#------------------------------------------------------------------------------
# This file is part of the OpenStructure project <www.openstructure.org>
#
# Copyright (C) 2008-2009 by the OpenStructure authors
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
Utility functions to load secondary structure information from DSSP files
and assign them to entities.

Authors: Pascal Benkert, Marco Biasini
"""

import os
import tempfile

from ost import io,mol
from ost import settings


def _SkipHeader(stream):
    line=stream.readline()
    while line:
      if line.strip().find('#')==0:
        return True
      line=stream.readline()
    return False


def _ExecuteDSSP(path, temp_dir=None):
  # use of mktemp is a safty problem (use mkstemp and provide file handle to 
  # subsequent process
  temp_dssp_path=tempfile.mktemp(suffix=".out",prefix="dssp", dir=temp_dir)
  dssp_abs_path=settings.Locate('dssp', env_name='DSSP_EXECUTABLE')
  status = os.system(dssp_abs_path+" "+path+" 2>/dev/null 1> "+temp_dssp_path)
  return temp_dssp_path


def _CalcRelativeSA(residue_type, absolute_sa):
  solvent_max_list=[118,317,238,243,183,262,286,154,258,228,
                    243,278,260,271,204,234,206,300,303,216] #TODO: source?
  residue_indices = "ARNDCQEGHILKMFPSTWYV"
  if residue_type.islower()==True:
    residue_type='C'
  if residue_indices.find(residue_type)==-1:
    raise RuntimeError('residue %s is a non-standard residue' %(residue_type))	
  else:
    rel=float(absolute_sa)/(solvent_max_list[residue_indices.find(residue_type)])
  return rel
  

def AssignDSSP(ent, pdb_path="", extract_burial_status_flag=0, tmp_dir=None):
  entity_saved_flag = 0
  if pdb_path=="":
    # use of mktemp is a safty problem (use mkstemp and provide file handle to 
    # subsequent process
    pdb_path=tempfile.mktemp(suffix=".pdb",prefix="temp_entity", 
                             dir=tmp_dir)
    io.SaveEntity(ent, pdb_path)
    entity_saved_flag = 1

  #TODO: exception handling (currently errors occuring here
  # are handled in the parser LoadDSSP)
  temp_dssp_path=_ExecuteDSSP(pdb_path)

  # assign DSSP to entity
  try:
    LoadDSSP(temp_dssp_path, ent, extract_burial_status_flag, 
             entity_saved_flag)
  except Exception, e:
    # clean up
    print "Exception in DSSP:", e
    if entity_saved_flag == 1:
      os.remove(pdb_path)
    os.remove(temp_dssp_path)
    raise RuntimeError(e)

  # clean up
  #print pdb_path, temp_dssp_path
  if entity_saved_flag == 1:
    os.remove(pdb_path)
  os.remove(temp_dssp_path)

  return ent



def LoadDSSP(file_name, model, extract_burial_status_flag=0, 
             entity_saved_flag=0, calculate_relative_sa=True):
    if model.IsValid() == 0:
      print "DSSP: model is not valid"
    stream=open(file_name)
    if not _SkipHeader(stream):
      stream.close()
      raise RuntimeError('Ill-formatted DSSP file')

    for line in stream:
      num=line[6:10].strip()
      ins_code=line[10].strip()
      chain_name=line[11]
      solvent_accessibility=float(line[34:39].strip())
      #solvent_accessibility=line[34:39].strip()
      amino_acid=line[13]
      #print line


      if isinstance(model,mol.ChainView):
        chain=model
      else:
        chain=model.FindChain(chain_name)
        
      if not chain.IsValid():
        continue
      if num=='':
        continue
      residue=None
      try:
        if ins_code == "":
          residue=chain.FindResidue(mol.ResNum(int(num)))
        else:
          residue=chain.FindResidue(mol.ResNum(int(num),ins_code))

        # set property "burial status:
        if extract_burial_status_flag == 1:
         #set default (dummy) burial status for incomplete residues:
         residue.SetGenericStringProperty("burial_status", 'X')

         #handle seleno-methionine appearing as amino acid 'X' in DSSP:
         if residue.name=="MSE" and amino_acid=='X':
           amino_acid='M'

         residue.SetGenericFloatProperty("solvent_accessibility", 
                                         solvent_accessibility)
         if calculate_relative_sa:
           relative_sa=_CalcRelativeSA(amino_acid,solvent_accessibility)
           residue.SetGenericFloatProperty("relative_solvent_accessibility", 
                                           relative_sa)
           if relative_sa < 0.25:
             residue.SetGenericStringProperty("burial_status", 'b')
           else:
             residue.SetGenericStringProperty("burial_status", 'e')
      except Exception, e:
        print "ERROR:",e
        continue

      rtype=line[16:17]
      rt=mol.SecStructure.COIL
      if rtype=='H':
        rt=mol.SecStructure.ALPHA_HELIX
      elif rtype=='E':
        rt=mol.SecStructure.EXTENDED
      elif rtype=='B':
        rt=mol.SecStructure.BETA_BRIDGE
      elif rtype=='S':
        rt=mol.SecStructure.BEND
      elif rtype=='T':
        rt=mol.SecStructure.TURN
      elif rtype=='I':
        rt=mol.SecStructure.PI_HELIX
      elif rtype=='G':
        rt=mol.SecStructure.THREE_TEN_HELIX
      # for corrupted DSSP files. Catch in calling routine:
      if residue.IsValid() == 0:
        #Todo: if residues with occupancy 0 have been removed before
        #using a selection statement, they are missed here
        #IMPORTANT: asign DSSP before any selections
        stream.close()
        raise RuntimeError('Ill-formatted DSSP file: invalid residue')	
      residue.SetSecStructure(mol.SecStructure(rt))
    stream.close()
