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

def LoadPDB(filename, restrict_chains="", no_hetatms=False,
            fault_tolerant=False, load_multi=False,
            join_spread_atom_records=False, calpha_only=False, remote=False):
  """
  Load PDB file from disk and returns one or more entities. Several options 
  allow to customize the exact behaviour of the PDB import.

  :param restrict_chains: If not an empty string, only chains listed in the
     string will be imported.

  :param fault_tolerant: If True, the import will succeed, even if the
     PDB contains faulty records. The faulty records will be ignored and import 
     continues as if the records haven't been present.

  :param no_hetatms: If set to True, HETATM records will be ignored

  :param load_multi: If set to True, a list of entities will be returned instead
     of only the first. This is useful when dealing with multi-PDB files.

  :param join_spread_atom_records: If set to true, atom records belonging to the
     same residue are joined, even if they do not appear sequentially in the PDB
     file.
  
  :param remote: If set to true, the method tries to load the pdb from the remote
     pdb repository www.pdb.org.
  :rtype: :class:`~ost.mol.EntityHandle` or a list thereof if `load_multi` is 
      True.
      
  :raises: :exc:`~ost.io.IOException` if the import fails due to an erroneous or 
      inexistent file
  """
  if remote:
    output_dir = tempfile.gettempdir()
    if __GetModelFromPDB(filename, output_dir):
      filename = os.path.join(output_dir, 'pdb%s.ent.gz' % filename)
    else:
      raise IOError('Can not load PDB %s from www.pdb.org'%filename) 
  
  conop_inst=conop.Conopology.Instance()
  builder=conop_inst.GetBuilder("DEFAULT")
  reader=PDBReader(filename)

  flags=0
  if calpha_only:
    flags|=PDB.CALPHA_ONLY
  if fault_tolerant:
    flags|=PDB.SKIP_FAULTY_RECORDS
  if no_hetatms:
    flags|=PDB.NO_HETATMS
  if join_spread_atom_records:
    flags|=PDB.JOIN_SPREAD_ATOM_RECORDS
  try:
    PDB.PushFlags(PDB.Flags() | flags)
    if load_multi:
      ent_list=[]
      while reader.HasNext():
        ent=mol.CreateEntity()
        reader.Import(ent, restrict_chains)
        conop_inst.ConnectAll(builder, ent, 0)
        ent_list.append(ent)
      PDB.PopFlags()
      return ent_list
    else:
      ent=mol.CreateEntity()
      if reader.HasNext():
        reader.Import(ent, restrict_chains)
        conop_inst.ConnectAll(builder, ent, 0)
      PDB.PopFlags()
      return ent
  except:
    PDB.PopFlags()
    raise

def SavePDB(models, filename):
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
  writer=PDBWriter(filename)
  try:
    if len(models)>1:
      PDB.PushFlags(PDB.Flags() |PDB.WRITE_MULTIPLE_MODELS)
    else:
      PDB.PushFlags(0)
    for model in models:
      writer.Write(model)
    PDB.PopFlags()
  except:
    PDB.PopFlags()
    raise
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

## \example fft_li.py
#
# This scripts loads one or more images and shows their Fourier Transforms on the screen. A viewer
# is opened for each loaded image. The Fourier Transform honors the origin of the reference system,
# which is assumed to be at the center of the image.
#
# Usage:
#
# \code giplt view_ft.py <image1> [<image2> <image3> .... ] \endcode
#
# <BR>
# <BR>
