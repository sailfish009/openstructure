//------------------------------------------------------------------------------
// This file is part of the OpenStructure project <www.openstructure.org>
//
// Copyright (C) 2008-2010 by the OpenStructure authors
//
// This library is free software; you can redistribute it and/or modify it under
// the terms of the GNU Lesser General Public License as published by the Free
// Software Foundation; either version 3.0 of the License, or (at your option)
// any later version.
// This library is distributed in the hope that it will be useful, but WITHOUT
// ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
// FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public License for more
// details.
//
// You should have received a copy of the GNU Lesser General Public License
// along with this library; if not, write to the Free Software Foundation, Inc.,
// 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
//------------------------------------------------------------------------------
#ifndef OST_SCENE_FX_HH
#define OST_SCENE_FX_HH

#include <ost/gfx/gl_helper.hh>

/*
  low level code for scene shading effects

  Author: Ansgar Philippsen
*/

namespace ost { namespace gfx { 

class Scene;

namespace impl {

void prep_shadow_map(Scene& scene, GLuint texunit, GLuint texid, uint quality);
void prep_depth_darkening(Scene& scene, GLuint texunit, GLuint texid);
void prep_amb_occlusion(Scene& scene, GLuint texunit, GLuint texid);
void prep_depth_map(Scene& scene, uint, uint, GLuint texunit, GLuint texid);
void draw_screen_quad(unsigned int w, unsigned int h, GLuint texunit, GLuint texid);
void draw_debug_tex(unsigned int w, unsigned int h, GLuint texid);

}}} // ns

#endif
