import os.path
from _ost_mol_mm import *
import ost	

def LoadAMBERForcefield():
  return Forcefield.Load(os.path.join(ost.GetSharedDataPath(),'forcefields','AMBER03.dat'))

def LoadCHARMMForcefield():
  return Forcefield.Load(os.path.join(ost.GetSharedDataPath(),'forcefields','CHARMM27.dat'))
