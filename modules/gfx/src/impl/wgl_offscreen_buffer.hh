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
#ifndef OST_GFX_WGL_OFFSCREEN_BUFFER_HH
#define OST_GFX_WGL_OFFSCREEN_BUFFER_HH

#include <ost/gfx/GL/wglew.h>

namespace ost { namespace gfx {

/*
  wgl implementation for offscreen buffers
*/

class WGLOffscreenBuffer: public OffscreenBuffer
{
public:
  WGLOffscreenBuffer(unsigned int width, unsigned int height, const OffscreenBufferFormat& f, bool shared=true);

  bool Resize(unsigned int w, unsigned int h);

  bool MakeActive();

  bool IsActive() {return active_;}

  bool IsValid() {return valid_;}

private:
  int width_;
  int height_;
  bool valid_;
  bool active_;

  HPBUFFERARB        pbuffer_;
  HGLRC              context_; //rendering context
  HGLRC              old_context_;
  
  HDC                dev_context_;//device context
  HDC                old_dev_context_;
};

}} // ns

#endif
