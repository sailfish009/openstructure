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
Wrappers for the tmalign and tmscore utilities.

References:

tmscore: Yang Zhang and Jeffrey Skolnick, Proteins 2004 57: 702-710 
tmalign: Y. Zhang and J. Skolnick, Nucl. Acids Res. 2005 33, 2302-9 


Authors: Pascal Benkert, Marco Biasini
"""

import subprocess, os, tempfile, platform
from ost import settings, io, geom, seq

def _SetupFiles(models):
  # create temporary directory
  tmp_dir_name=tempfile.mkdtemp()
  for index, model in enumerate(models):
    io.SavePDB(model, os.path.join(tmp_dir_name, 'model%02d.pdb' % (index+1)))
  return tmp_dir_name

def _CleanupFiles(dir_name):
  import shutil
  shutil.rmtree(dir_name)

class TMAlignResult:
  def __init__(self, rmsd, tm_score, aligned_length, transform, ref_sequence, alignment):
    self.rmsd=rmsd
    self.tm_score=tm_score    
    self.aligned_length=aligned_length
    self.transform=transform
    self.ref_sequence =ref_sequence
    self.alignment=alignment

def _ParseTmAlign(lines):
  info_line=lines[11].split(',')
  aln_length=float(info_line[0].split('=')[1].strip())
  rmsd=float(info_line[1].split('=')[1].strip())  
  tm_score=float(info_line[2].split('=')[1].strip())
  tf1=[float(i.strip()) for i in lines[15].split()]
  tf2=[float(i.strip()) for i in lines[16].split()]
  tf3=[float(i.strip()) for i in lines[17].split()]
  rot=geom.Mat3(tf1[2], tf1[3], tf1[4], tf2[2], tf2[3],
                tf2[4], tf3[2], tf3[3], tf3[4])
  tf=geom.Mat4(rot)
  tf.PasteTranslation(geom.Vec3(tf1[1], tf2[1], tf3[1]))
  seq1 = seq.CreateSequence("1",lines[20].strip())
  seq2 = seq.CreateSequence("2",lines[22].strip())
  alignment = seq.CreateAlignment()
  alignment.AddSequence(seq2)
  alignment.AddSequence(seq1)
  return TMAlignResult(rmsd, aln_length, tm_score, tf, seq2, alignment)

def _RunTmAlign(tmalign, tmp_dir):
  model1_filename=os.path.join(tmp_dir, 'model01.pdb')
  model2_filename=os.path.join(tmp_dir, 'model02.pdb')
  if platform.system() == "Windows":
    tmalign_path=settings.Locate('tmalign.exe', explicit_file_name=tmalign)
    command="\"%s\" %s %s" %(os.path.normpath(tmalign_path), model1_filename, model2_filename)
  else:
    tmalign_path=settings.Locate('tmalign', explicit_file_name=tmalign)  
    command="\"%s\" \"%s\" \"%s\"" %(tmalign_path, model1_filename, model2_filename)
  ps=subprocess.Popen(command, shell=True, stdout=subprocess.PIPE)
  ps.wait()
  lines=ps.stdout.readlines()
  if (len(lines))<22:
    raise RuntimeError("tmalign superposition failed")
  return _ParseTmAlign(lines)

class TMScoreResult:
  def __init__(self, rmsd_common, tm_score, max_sub, 
               gdt_ts, gdt_ha, rmsd_below_five, transform):
    self.rmsd_common=rmsd_common
    self.tm_score=tm_score    
    self.max_sub=max_sub
    self.gdt_ts=gdt_ts
    self.gdt_ha=gdt_ha
    self.rmsd_below_five=rmsd_below_five
    self.transform=transform
    
def _ParseTmScore(lines):
  tf1=[float(i.strip()) for i in lines[23].split()]
  tf2=[float(i.strip()) for i in lines[24].split()]
  tf3=[float(i.strip()) for i in lines[25].split()]
  rot=geom.Mat3(tf1[2], tf1[3], tf1[4], tf2[2], tf2[3],
                  tf2[4], tf3[2], tf3[3], tf3[4])
  tf=geom.Mat4(rot)
  tf.PasteTranslation(geom.Vec3(tf1[1], tf2[1], tf3[1]))
  result=TMScoreResult(float(lines[14].split()[-1].strip()),
                       float(lines[16].split()[2].strip()),
                       float(lines[17].split()[1].strip()),
                       float(lines[18].split()[1].strip()),
                       float(lines[19].split()[1].strip()),
                       float(lines[27].split()[-1].strip()),
                       tf)
  return result


def _RunTmScore(tmscore, tmp_dir):
  model1_filename=os.path.join(tmp_dir, 'model01.pdb')
  model2_filename=os.path.join(tmp_dir, 'model02.pdb')  
  if platform.system() == "Windows":
    tmscore_path=settings.Locate('tmscore.exe', explicit_file_name=tmscore)
    command="\"%s\" %s %s" %(os.path.normpath(tmscore_path), model1_filename, model2_filename)
  else:
    tmscore_path=settings.Locate('tmscore', explicit_file_name=tmscore)
    command="\"%s\" \"%s\" \"%s\"" %(tmscore_path, model1_filename, model2_filename)
  ps=subprocess.Popen(command, shell=True, stdout=subprocess.PIPE)
  ps.wait()
  lines=ps.stdout.readlines()
  if (len(lines))<22:
    raise RuntimeError("tmscore superposition failed")
  return _ParseTmScore(lines)

def TMAlign(model1, model2, tmalign=None):
  """
  Run tmalign on two protein structures
  """
  tmp_dir_name=_SetupFiles((model1, model2))
  result=_RunTmAlign(tmalign, tmp_dir_name)
  model1.handle.RequestXCSEditor().ApplyTransform(result.transform)
  _CleanupFiles(tmp_dir_name)
  return result
def TMScore(model1, model2, tmscore=None):
  """
  Run tmscore on two protein structures
  """
  tmp_dir_name=_SetupFiles((model1, model2))
  result=_RunTmScore(tmscore, tmp_dir_name)
  model1.handle.RequestXCSEditor().ApplyTransform(result.transform)  
  _CleanupFiles(tmp_dir_name)
  return result
