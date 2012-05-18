"""
Some functions for analyzing trajectories

Author: Niklaus Johner
"""

import os
import ost

def GetFrameFromEntity(eh):
  """
  This function returns a CoordFrame from an EntityHandle
  Input:
    eh : EntityHandle
  """
  return ost.mol.CreateCoordFrame(eh.GetAtomPosList())
  
