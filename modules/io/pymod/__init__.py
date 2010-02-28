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
from _io import *
from ost import mol,conop

def LoadPDB(filename, restrict_chains="", no_hetatms=False,
            fault_tolerant=False, load_multi=False):
  """
  Load PDB file from disk.
  
  If restrict_chains is not an empty string, only chains listed in the
  string will be imported. 
  
  If fault_tolerant is set to true, the import will succeed, even if the
  PDB contains faulty records. The faulty records will be ignored in that
  case.
  
  If not_hetatms is set to True, HETATM records will be ignored
  
  If load_multi is set to true, a list of entities will be returned instead
  of only the first.
  """          
  conop_inst=conop.Conopology.Instance()
  builder=conop_inst.GetBuilder("DEFAULT")
  reader=PDBReader(filename)

  flags=0
  if fault_tolerant:
    flags=PDB.SKIP_FAULTY_RECORDS
  if no_hetatms:
    flags|=PDB.NO_HETATMS
  reader.SetFlags(flags)
  if load_multi:
    ent_list=[]
    while reader.HasNext():
      ent=mol.CreateEntity()
      reader.Import(ent, restrict_chains)
      conop_inst.ConnectAll(builder, ent, 0)
      ent_list.append(ent)
    return ent_list
  else:
    ent=mol.CreateEntity()    
    if reader.HasNext():
      reader.Import(ent, restrict_chains)
      conop_inst.ConnectAll(builder, ent, 0)
    return ent

def SavePDB(models, filename):
  """
  Save entity or list of entities to disk
  """
  if not getattr(models, '__len__', None):
    models=[models]
  writer=PDBWriter(filename)
  if len(models)>1:
    writer.SetFlags(PDB.WRITE_MULTIPLE_MODELS)
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
