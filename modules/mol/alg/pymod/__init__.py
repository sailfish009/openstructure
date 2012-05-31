from _ost_mol_alg import *
from ost.mol.alg.superpose import *
import ost.mol.alg.trajectory_analysis
import ost.mol.alg.structure_analysis
import ost.mol.alg.helix_kinks


def FillClashingDistancesFromFile(filename,default_clashing_distance,default_tolerance):
  fh=open(filename,'r')
  lines=fh.readlines()
  fh.close()
  return FillClashingDistances(lines,default_clashing_distance,default_tolerance)
