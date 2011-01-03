#------------------------------------------------------------------------------
# This file is part of the OpenStructure project <www.openstructure.org>
#
# Copyright (C) 2008-2010 by the OpenStructure authors
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
import os, tempfile, ftplib, httplib

from _io import *
from ost import mol, conop

profiles=None

class IOProfiles:
  def __init__(self):
     self._dict={}

  def __getitem__(self, key):
    return IOProfileRegistry.Instance().Get(key)

  def __setitem__(self, key, value):
    if isinstance(value, str):
      value=self[value]
    IOProfileRegistry.Instance().Set(key, value)
    self._dict[key]=value

  def __len__(self):
    return len(self._dict)

  def __iter__(self):
    return self._dict.__iter__()

if not profiles:
  profiles=IOProfiles()
  profiles['STRICT']=IOProfile(dialect='PDB', fault_tolerant=False,
                               strict_hydrogens=False, quack_mode=False)
  profiles['SLOPPY']=IOProfile(dialect='PDB', fault_tolerant=True,
                               strict_hydrogens=False, quack_mode=True)
  profiles['CHARMM']=IOProfile(dialect='CHARMM', fault_tolerant=True,
                               strict_hydrogens=False, quack_mode=False)
  profiles['DEFAULT']='STRICT'

def _override(val1, val2):
  if val2!=None:
    return val2
  else:
    return val1

def __GetModelFromPDB(model_id, output_dir, file_pattern='pdb%s.ent.gz'):
  file_name = file_pattern % model_id
  file_path = os.path.join(output_dir,file_name)
  try:
    server="ftp.wwpdb.org"
    ftp=ftplib.FTP(server,"anonymous","user@")
    ftp.cwd("pub/pdb/data/structures/all/pdb")
    ftp_retrfile=open(file_path,"wb")
    ftp.retrbinary("RETR "+file_name,ftp_retrfile.write)
    ftp_retrfile.close()
  except:
    conn=httplib.HTTPConnection('www.pdb.org')
    conn.request('GET', '/pdb/files/%s.pdb.gz' % model_id )
    response=conn.getresponse()
    if response.status==200:
      data=response.read()
      f=open(os.path.join(output_dir, file_pattern % model_id), 'w+')
      f.write(data)
      f.close()
    else:
      conn.close()
      return False
  return os.path.getsize(file_path) > 0

def LoadPDB(filename, restrict_chains="", no_hetatms=None,
            fault_tolerant=None, load_multi=False, quack_mode=None,
            join_spread_atom_records=None, calpha_only=None,
            profile='DEFAULT', remote=False, dialect=None,
            strict_hydrogens=None):
  """
  Load PDB file from disk and returns one or more entities. Several options 
  allow to customize the exact behaviour of the PDB import. For more information 
  on these options, see :doc:`profile`.
  
  Residues are flagged as ligand if they are mentioned in a HET record.

  :param restrict_chains: If not an empty string, only chains listed in the
     string will be imported.

  :param fault_tolerant: Enable/disable fault-tolerant import. If set, overrides 
     the value of :attr:`IOProfile.fault_tolerant`.

  :param no_hetatms: If set to True, HETATM records will be ignored. Overrides 
      the value of :attr:`IOProfile.no_hetatms`

  :param load_multi: If set to True, a list of entities will be returned instead
      of only the first. This is useful when dealing with multi-PDB files.

  :param join_spread_atom_records: If set, overrides the value of 
      :attr:`IOProfile.join_spread_atom_records`.
  
  :param remote: If set to true, the method tries to load the pdb from the 
     remote pdb repository www.pdb.org. The filename is then interpreted as the 
     pdb id.
     
  :rtype: :class:`~ost.mol.EntityHandle` or a list thereof if `load_multi` is 
      True.

  :param dialect: Specifies the particular dialect to use. If set, overrides 
    the value of :attr:`IOProfile.dialect`

  :type dialect: :class:`str`
  
  :param strict_hydrogens: If set, overrides the value of 
     :attr:`IOProfile.strict_hydrogens`.

  :raises: :exc:`~ost.io.IOException` if the import fails due to an erroneous or 
      inexistent file
  """
  def _override(val1, val2):
    if val2!=None:
      return val2
    else:
      return val1
  if isinstance(profile, str):
    prof=profiles[profile].Copy()
  else:
    prof=profile.Copy()
  if dialect not in (None, 'PDB', 'CHARMM',):
    raise ValueError('dialect must be PDB or CHARMM')
  prof.calpha_only=_override(prof.calpha_only, calpha_only)
  prof.no_hetatms=_override(prof.no_hetatms, no_hetatms)
  prof.dialect=_override(prof.dialect, dialect)
  prof.quack_mode=_override(prof.quack_mode, quack_mode)
  prof.strict_hydrogens=_override(prof.strict_hydrogens, strict_hydrogens)
  prof.fault_tolerant=_override(prof.fault_tolerant, fault_tolerant)
  prof.join_spread_atom_records=_override(prof.join_spread_atom_records,
                                          join_spread_atom_records)

  if remote:
    output_dir = tempfile.gettempdir()
    if __GetModelFromPDB(filename, output_dir):
      filename = os.path.join(output_dir, 'pdb%s.ent.gz' % filename)
    else:
      raise IOError('Can not load PDB %s from www.pdb.org'%filename) 
  
  conop_inst=conop.Conopology.Instance()
  builder=conop_inst.GetBuilder("DEFAULT")
  if prof.dialect=='PDB':
    builder.dialect=conop.PDB_DIALECT
  elif prof.dialect=='CHARMM':
    builder.dialect=conop.CHARMM_DIALECT
  builder.strict_hydrogens=prof.strict_hydrogens
  reader=PDBReader(filename, prof)
  try:
    if load_multi:
      ent_list=[]
      while reader.HasNext():
        ent=mol.CreateEntity()
        reader.Import(ent, restrict_chains)
        conop_inst.ConnectAll(builder, ent, 0)
        ent_list.append(ent)
      if len(ent_list)==0:
        raise IOError("File doesn't contain any entities")
      return ent_list
    else:
      ent=mol.CreateEntity()
      if reader.HasNext():
        reader.Import(ent, restrict_chains)
        conop_inst.ConnectAll(builder, ent, 0)
      else:
        raise IOError("File doesn't contain any entities")
      return ent
  except:
    raise

def SavePDB(models, filename, dialect=None,  pqr=False, profile='DEFAULT'):
  """
  Save entity or list of entities to disk. If a list of entities is supplied the 
  PDB file will be saved as a multi PDB file. Each of the entities is wrapped 
  into a MODEL/ENDMDL pair.
  
  :param models: The entity or list of entities (handles or views) to be saved
  :param filename: The filename
  :type  filename: string
  """
  if not getattr(models, '__len__', None):
    models=[models]
  if isinstance(profile, str):
    profile=profiles[profile].Copy()
  else:
    profile.Copy()
  profile.dialect=_override(profile.dialect, dialect)
  writer=PDBWriter(filename, profile)
  writer.SetIsPQR(pqr)
  if len(models)>1:
    writer.write_multi_model=True
  for model in models:
    writer.Write(model)

try:
  from ost import img
  LoadMap = LoadImage
  SaveMap = SaveImage
except ImportError:
  pass

 ## loads several images and puts them in an ImageList
 # \sa \ref fft_li.py "View Fourier Transform Example"
def LoadImageList (files):
  image_list=img.ImageList()
  for file in files:
    image=LoadImage(file)
    image_list.append(image)
  return image_list

LoadMapList=LoadImageList

def LoadCHARMMTraj(crd, dcd_file=None, profile='CHARMM',
                   lazy_load=False, stride=1, 
                   dialect=None):
  """
  Load CHARMM trajectory file.
  
  :param crd: EntityHandle or filename of the (PDB) file containing the
      structure. The structure must have the same number of atoms as the 
      trajectory
  :param dcd_file: The filename of the DCD file. If not set, and crd is a 
      string, the filename is set to the <crd>.dcd
  :param layz_load: Whether the trajectory should be loaded on demand. Instead 
      of loading the complete trajectory into memory, the trajectory frames are 
      loaded from disk when requested.
  :param stride: The spacing of the frames to load. When set to 2, for example, 
      every second frame is loaded from the trajectory. By default, every frame 
      is loaded.
  :param dialect: The dialect for the PDB file to use. See :func:`LoadPDB`. If 
      set, overrides the value of the profile
  :param profile: The IO profile to use for loading the PDB file. See 
      :doc:`profile`.
  """
  if not isinstance(crd, mol.EntityHandle):
    if dcd_file==None:
      dcd_file='%s.dcd' % os.path.splitext(crd)[0]    
    crd=LoadPDB(crd, profile=profile, dialect=dialect)

  else:
    if not dcd_file:
      raise ValueError("No DCD filename given")
  return LoadCHARMMTraj_(crd, dcd_file, stride, lazy_load)

## \example fft_li.py
#
# This scripts loads one or more images and shows their Fourier Transforms on 
# the screen. A viewer is opened for each loaded image. The Fourier Transform 
# honors the origin of the reference system, which is assumed to be at the 
# center of the image.
#
# Usage:
#
# \code giplt view_ft.py <image1> [<image2> <image3> .... ] \endcode
#
# <BR>
# <BR>
