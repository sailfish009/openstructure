import os.path
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


def FillBondStereoChemicalParamsFromFile(filename):
  fh=open(filename,'r')
  lines=fh.readlines()
  fh.close()
  return FillBondStereoChemicalParams("Bond",lines)


def FillAngleStereoChemicalParamsFromFile(filename):
  fh=open(filename,'r')
  lines=fh.readlines()
  fh.close()
  return FillAngleStereoChemicalParams("Angle",lines)

  
def DefaultClashingDistances():
  shared_path=ost.GetSharedDataPath()	
  filename=os.path.join(shared_path,'stereo_chemical_props.txt')
  fh=open(filename,'r')
  lines=fh.readlines()
  fh.close()
  return FillClashingDistances(lines,1.5,0.0)

def DefaultBondStereoChemicalParams():
  shared_path=ost.GetSharedDataPath()	
  filename=os.path.join(shared_path,'stereo_chemical_props.txt')
  fh=open(filename,'r')
  lines=fh.readlines()
  fh.close()
  return FillStereoChemicalParams("Bond",lines)

def DefaultAngleStereoChemicalParams():
  shared_path=ost.GetSharedDataPath()	
  filename=os.path.join(shared_path,'stereo_chemical_props.txt')
  fh=open(filename,'r')
  lines=fh.readlines()
  fh.close()
  return FillStereoChemicalParams("Angle",lines)

