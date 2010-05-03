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
#include <ost/geom/geom.hh>

/*
  low level code for scene shading effects

  Author: Ansgar Philippsen
*/

namespace ost { namespace gfx { 

class Scene;

namespace impl {

class SceneFX {
public:
  ~SceneFX();
  static SceneFX& Instance();

  void Setup();

  void Resize(unsigned int w, unsigned int h);
  
  void Preprocess();
  // assumes scene has been drawn in the active framebuffer
  void Postprocess();

  bool shadow_flag;
  int shadow_quality;
  bool depth_dark_flag;
  float depth_dark_factor;
  bool amb_occl_flag;
  float amb_occl_factor;

private:
  SceneFX();
  SceneFX(const SceneFX&) {}
  SceneFX& operator=(const SceneFX&) {return *this;}

  void prep_shadow_map();
  void prep_depth_darkening();
  void prep_amb_occlusion();
  void draw_screen_quad(uint w, uint h);
  void draw_debug_tex(unsigned int w, unsigned int h, GLuint texid);

  GLuint scene_tex_id_;
  GLuint depth_tex_id_;
  GLuint shadow_tex_id_;
  geom::Mat4 shadow_tex_mat_;
  GLuint occl_tex_id_;
  GLuint dark_tex_id_;
  GLuint norm_tex_id_;
  GLuint kernel_tex_id_;
  uint kernel_size_;
  GLuint kernel2_tex_id_;
  uint kernel2_size_;

  GLuint scene_tex2_id_;
  GLuint norm_tex2_id_;
  GLuint scene_fb_;
  GLuint depth_rb_;

  bool use_fb_;
};

}}} // ns

#endif
