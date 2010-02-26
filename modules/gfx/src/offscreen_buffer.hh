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
#ifndef OST_GFX_OFFSCREEN_BUFFER_HH
#define OST_GFX_OFFSCREEN_BUFFER_HH

/*
  Wraps GL offscreen rendering per platform

  Authors: Ansgar Philippsen, Marco Biasini
*/

#include <ost/gfx/gl_include.hh>

#if defined(__linux__)
#  include <GL/glx.h>
#elif defined(__APPLE__)
#  include <OpenGL/OpenGL.h>
//#elif defined(_WIN32)
//#  include <ost/gfx/GL/wglew.h>
#endif

#ifdef Complex
#  undef Complex
#endif

namespace ost { namespace gfx {

/// \brief offscreen management
class OffscreenBuffer {
public:
  
  /// \brief singleton interface
  static OffscreenBuffer& Instance();

  /// \brief activate offscreen context for rendering
  bool Begin();
  /// \brief switch back to normal rendering context
  bool End();
  /// \ brief resize offscreen rendering context
  bool Resize(unsigned int width, unsigned int height);

  /// \brief returns true if offscreen rendering context is available
  bool IsValid() const;

  /// \brief returns true between Begin/End bracket
  bool IsActive() const;

private:
  OffscreenBuffer(int width, int height, int r_bits, int b_bits, 
                  int g_bits, int a_bits, int depth_bits);

  OffscreenBuffer(const OffscreenBuffer& b) {}
  OffscreenBuffer& operator=(const OffscreenBuffer& b) {return *this;}
  
  int width_;
  int height_;
  bool valid_;
  bool active_;

#if defined(__linux__)
  Display*    dpy_;
  GLXFBConfig* fbconfig_;
  GLXPbuffer  pbuffer_;
  GLXContext  context_;
  GLXContext  old_context_;
  GLXDrawable        old_dr1_;
  GLXDrawable        old_dr2_;

#elif defined(__APPLE__)

  CGLPBufferObj      pbuffer_;
  CGLContextObj      context_;
  CGLPixelFormatObj  pix_format_;
  CGLContextObj      old_context_;

#elif defined(_WIN32)
/*  HPBUFFERARB        pbuffer_;
  HGLRC              context_; //rendering context
  HGLRC              old_context_;
  
  HDC                dev_context_;//device context
  HDC                old_dev_context_;
  */
#endif

  GLint       old_vp_[4];
};
 
}}

#endif
