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
from _gfx import *

WHITE=Color(1.0,1.0,1.0)
BLACK=Color(0.0,0.0,0.0)
GREY=Color(0.5,0.5,0.5)
RED=Color(1.0,0.0,0.0)
DARKRED=Color(0.5,0.0,0.0)
LIGHTRED=Color(1.0,0.5,0.5)
GREEN=Color(0.0,1.0,0.0)
DARKGREEN=Color(0.0,0.5,0.0)
LIGHTGREEN=Color(0.5,1.0,0.5)
BLUE=Color(0.0,0.0,1.0)
DARKBLUE=Color(0.0,0.0,0.5)
LIGHTBLUE=Color(0.5,0.5,1.0)
YELLOW=Color(1.0,1.0,0.0)
DARKYELLOW=Color(0.5,0.5,0.0)
LIGHTYELLOW=Color(1.0,1.0,0.5)
CYAN=Color(0.0,1.0,1.0)
DARKCYAN=Color(0.0,0.5,0.5)
LIGHTCYAN=Color(0.5,1.0,1.0)
MAGENTA=Color(1.0,0.0,1.0)
DARKMAGENTA=Color(0.5,0.0,0.5)
LIGHTMAGENTA=Color(1.0,0.5,1.0)
PURPLE=MAGENTA
DARKPURPLE=DARKMAGENTA
LIGHTPURPLE=LIGHTMAGENTA
ORANGE=Color(1.0,0.5,0.0)
DARKORANGE=Color(0.5,0.25,0.0)
LIGHTORANGE=Color(1.0,0.75,0.5)


def Stereo(mode,flip=None,alg=None):
  """
  Stereo control

  :param mode: 0=off, 1=quad-buffered, 2=interlaced
  :type mode: int
  :param flip: invert order of left/right display
  :type flip: bool
  :param alg: stereo algorithm (0 or 1)
  :type param: int
  """
  if(flip):
    _gfx.Scene().SetStereoFlip(flip)
  if(alg):
    _gfx.Scene().SetStereoAlg(alg)
  if(mode):
    _gfx.Scene().SetStereoMode(mode)

def FitToScreen(gfx_ent, width=None, height=None, margin=0.01):
  """
  Setup camera such that it is centered on the graphical entity and the entity 
  fits the entire viewport. The longest axes of the entity are aligned along 
  the x- and y- axes of the screen.
  
  :param gfx_ent: The graphical entity
  :type gfx_ent: str or :class:`Entity`
  
  
  """
  from ost import geom
  import math
  def _XYZ(view, axes):
    """
    returns the vectors in x, y and z direction respectively. The smallest 
    vector is in z, then y, and the largest along x.
    """
    rows=[axes.GetRow(i) for i in range(3)]
    lengths=[]
    for axe in rows:
      min_proj=geom.Dot(axe, view.atoms[0].pos)
      max_proj=min_proj
      for atom in view.atoms[1:]:
        proj=geom.Dot(axe, atom.pos)
        min_proj=min(proj, min_proj)
        max_proj=max(proj, max_proj)
      lengths.append(max_proj-min_proj)
    def cmp_x(rhs, lhs):
      return cmp(lhs[1], rhs[1])
    sorted_axes=sorted(zip(rows, lengths), cmp_x)
    return [r*l for r,l in sorted_axes]
  scene=Scene()
  if not isinstance(gfx_ent, Entity):
    gfx_ent=scene[str(gfx_ent)]
  width=width and width or scene.viewport.width
  height=height and height or scene.viewport.height
  atom_positions=geom.Vec3List([atom.pos for atom in gfx_ent.view.atoms])
  axes=atom_positions.principal_axes
  sorted_axes=_XYZ(gfx_ent.view, axes)
  x_bigger_than_y=geom.Length(sorted_axes[0])>geom.Length(sorted_axes[1])
  if x_bigger_than_y:
    if width>height:    
      x_axes=geom.Normalize(sorted_axes[0])
      y_axes=geom.Normalize(sorted_axes[1])
    else:
      x_axes=geom.Normalize(sorted_axes[1])
      y_axes=geom.Normalize(sorted_axes[0])
  else:
    if width>height:    
      x_axes=geom.Normalize(sorted_axes[1])
      y_axes=geom.Normalize(sorted_axes[0])
    else:
      x_axes=geom.Normalize(sorted_axes[0])
      y_axes=geom.Normalize(sorted_axes[1])
  z_axes=geom.Normalize(geom.Cross(x_axes, y_axes))
  rotation=geom.Mat3(x_axes[0], x_axes[1], x_axes[2],
                     y_axes[0], y_axes[1], y_axes[2],
                     z_axes[0], z_axes[1], z_axes[2])
  rtc=geom.Mat4(rotation)
  
  center=gfx_ent.center
  aspect=float(width)/float(height)
  factor_y=1.0/math.tan(math.radians(scene.fov))
  factor_x=factor_y/aspect
  z_off=geom.Length(sorted_axes[2])*0.5
  rtc[0,3]=center[0]
  rtc[1,3]=center[1]
  rtc[2,3]=center[2]
  rtc[3,0]=0
  rtc[3,1]=0
  rtc[3,2]=-(max(factor_x*(1+margin)*geom.Length(sorted_axes[0]), 
                factor_y*(1+margin)*geom.Length(sorted_axes[1]))+z_off)
  scene.SetRTC(rtc)

