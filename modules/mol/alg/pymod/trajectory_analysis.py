"""
Some functions for analyzing trajectories

Author: Niklaus Johner
"""

import ost.mol.alg
from ost import LogError
import os

def smooth(vec,n):
#Function to smooth a vector or a list of floats
#for each element it takes the average over itself and the
#n elements on each side, so over (2n+1) elements
  try:
    vec2=vec.copy()
  except:
    vec2=vec[:]
  for i in range(n):
    v=0.0
    count=1.0
    v+=vec[i]
    for j in range(n):
      count+=1
      v+=vec[i+j+1]
    for j in range(i):
      count+=1
      v+=vec[i-(j+1)]
    vec2[i]=v/float(count)
  for i in range(1,n+1):
    v=0.0
    count=1.0
    v+=vec[-i]
    for j in range(n):
      count+=1
      v+=vec[-(i+j+1)]
    for j in range(i-1):
      count+=1
      v+=vec[-i+j+1]
    vec2[-i]=v/float(count)
  for i in range(n,len(vec2)-n):
    v=vec[i]
    for j in range(n):
      v+=vec[i+j+1]
      v+=vec[i-j-1]
    vec2[i]=v/float(2.*n+1.)
  return vec2


"""
From here on the module needs numpy
"""

def RMSD_Matrix_From_Traj(t,sele,first=0,last=-1):
  """
  This function calculates a matrix M such that M[i,j] is the
  RMSD of the EntityView sele between frames i and j of the trajectory t
  aligned on sele.
  Its inputs are:
    t       : the trajectory (CoordGroupHandle)
    sele    : the EntityView used for alignment and RMSD calculation
    first=0 : the first frame of t to be used
    last=-1 : the last frame of t to be used
  Returns a numpy NxN matrix, where n is the number of frames.
  """
  try:
    import numpy as npy
    if last==-1:last=t.GetFrameCount()
    n_frames=last-first
    rmsd_matrix=npy.identity(n_frames)
    for i in range(n_frames):
      t=ost.mol.alg.SuperposeFrames(t,sele,begin=first,end=last,ref=i)
      eh=t.GetEntity()
      t.CopyFrame(i)
      rmsd_matrix[i,:]=ost.mol.alg.AnalyzeRMSD(t,sele,sele)
      if i==0:
        last=last-first
        first=0
    return rmsd_matrix
  except ImportError:
    LogError("Function needs numpy, but I could not import it.")
    raise


