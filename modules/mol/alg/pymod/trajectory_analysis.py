"""
Some functions for analyzing trajectories

Author: Niklaus Johner
"""

from ost import *
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


