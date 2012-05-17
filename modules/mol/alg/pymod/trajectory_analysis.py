"""
Some functions for analyzing trajectories

Author: Niklaus Johner
"""

import ost.mol.alg
import ost.geom
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


def PairwiseDistancesFromTraj(t,sele,first=0,last=-1,seq_sep=1):
  """
  This function calculates the distances between any pair of atoms in the
  EntityView sele  with sequence separation larger than seq_sep from a trajectory t.
  It return a matrix containing one line for each atom pair and N columns, where
  N is the length of the trajectory.
  Its inputs are:
    t       : the trajectory (CoordGroupHandle)
    sele    : the EntityView used to determine the atom pairs
    first=0 : the first frame of t to be used
    last=-1 : the last frame of t to be used
    seq_sep=1 : The minimal sequence separation between
  Returns a numpy NpairsxNframes matrix.
  """
  try:
    import numpy as npy
    if last==-1:last=t.GetFrameCount()
    n_frames=last-first
    n_var=0
    for i,a1 in enumerate(sele.atoms):
      for j,a2 in enumerate(sele.atoms):
        if not j-i<seq_sep:n_var+=1
    #n_var=sele.GetAtomCount()
    #n_var=(n_var-1)*(n_var)/2.
    dist_matrix=npy.zeros(n_frames*n_var)
    dist_matrix=dist_matrix.reshape(n_var,n_frames)
    k=0
    for i,a1 in enumerate(sele.atoms):
      for j,a2 in enumerate(sele.atoms):
        if j-i<seq_sep:continue
        dist_matrix[k]=ost.mol.alg.AnalyzeDistanceBetwAtoms(t,a1.GetHandle(),a2.GetHandle())[first:last]
        k+=1
    return dist_matrix
  except ImportError:
    LogError("Function needs numpy, but I could not import it.")
    raise
    
def DistanceMatrixFromPairwiseDistances(distances,p=2):
  """
  This function calculates an distance matrix M(NxN) from
  the pairwise distances matrix D(MxN), where N is the number
  of frames in the trajectory and M the number of atom pairs.
  M[i,j] is the distance between frame i and frame j
  calculated as a p-norm of the differences in distances
  from the two frames (distance-RMSD for p=2).
  Inputs:
    distances : a pairwise distance matrix as obtained from PairwiseDistancesFromTraj()
  Returns a numpy NxN matrix, where N is the number of frames.
  """
  try:
    import numpy as npy
    n1=distances.shape[0]
    n2=distances.shape[1]
    dist_mat=npy.identity(n2)
    for i in range(n2):
      for j in range(n2):
        if j<=i:continue
        d=(((abs(distances[:,i]-distances[:,j])**p).sum())/float(n1))**(1./p)
        dist_mat[i,j]=d
        dist_mat[j,i]=d
    return dist_mat
  except ImportError:
    LogError("Function needs numpy, but I could not import it.")
    raise

def DistRMSDFromTraj(t,sele,ref_sele,radius=7.0,average=False,seq_sep=4,first=0,last=-1):
  """
  This function calculates the distance RMSD from a trajectory.
  The distances selected for the calculation are all the distances
  between pair of atoms that from residues that are at least seq_sep apart
  in the sequence and that are smaller than radius in ref_sel.
  The number and order of atoms in ref_sele and sele should be the same.
  Its inputs are:
    t       : the trajectory (CoordGroupHandle)
    sele    : the EntityView used to determine the distances from t
    radius=7  : the upper limit of distances in ref_sele considered for the calculation
    seq_sep=4 : The minimal sequence separation between atom pairs considered for the calculation 
    average=false : use the average distance in the trajectory as reference instead of the distance obtained from ref_sele
    first=0 : the first frame of t to be used
    last=-1 : the last frame of t to be used
  Returns a numpy vecor dist_rmsd(Nframes).  
  """
  if not sele.GetAtomCount()==ref_sele.GetAtomCount():
    print 'Not same number of atoms in the two views'
    return
  try:
    import numpy as npy
    if last==-1:last=t.GetFrameCount()
    n_frames=last-first
    dist_rmsd=npy.zeros(n_frames)
    pair_count=0.0
    for i,a1 in enumerate(ref_sele.atoms):
      for j,a2 in enumerate(ref_sele.atoms):
        if j<=i:continue
        r1=a1.GetResidue()
        c1=r1.GetChain()
        r2=a2.GetResidue()
        c2=r2.GetChain()      
        if c1==c2 and abs(r2.GetNumber().num-r1.GetNumber().num)<seq_sep:continue
        d=ost.geom.Distance(a1.pos,a2.pos)
        if d<radius:
          a3=sele.atoms[i]
          a4=sele.atoms[j]
          d_traj=ost.mol.alg.AnalyzeDistanceBetwAtoms(t,a3.GetHandle(),a4.GetHandle())[first:last]
          if average:d=npy.mean(d_traj)
          for k,el in enumerate(d_traj):
            dist_rmsd[k]+=(el-d)**2.0
          pair_count+=1.0
    return (dist_rmsd/float(pair_count))**0.5
  except ImportError:
    LogError("Function needs numpy, but I could not import it.")
    raise
    
    