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

/*
  Author: Ansgar Philippsen
*/

#include <ost/log.hh>
#include <ost/gfx/offscreen_buffer.hh>
#include <ost/gfx/scene.hh>

namespace ost { namespace gfx {

OffscreenBuffer::OffscreenBuffer(unsigned int width, unsigned int height, const OffscreenBufferFormat& f, bool shared):
  width_(width), height_(height), valid_(false), active_(false)
{
  LOGN_DEBUG("offscreen buffer: checking for DISPLAY");
  if(getenv("DISPLAY")==NULL) {
    LOGN_ERROR("error creating offscreen rendering context: missing DISPLAY environment variable");
    return;
  }

  LOGN_DEBUG("offscreen buffer: XOpenDisplay");
  dpy_ = XOpenDisplay(getenv("DISPLAY"));
  if(dpy_==NULL) {
    LOGN_ERROR("error creating offscreen rendering context: XOpenDisplay failed");
    return;
  }

  std::vector<int> attrib_list;
  attrib_list.push_back(GLX_RENDER_TYPE);
  attrib_list.push_back(GLX_RGBA_BIT);
  attrib_list.push_back(GLX_DRAWABLE_TYPE);
  attrib_list.push_back(GLX_PBUFFER_BIT);
  attrib_list.push_back(GLX_DOUBLEBUFFER);
  attrib_list.push_back(False);
  attrib_list.push_back(GLX_DEPTH_SIZE);
  attrib_list.push_back(f.dbits);
  attrib_list.push_back(GLX_RED_SIZE);
  attrib_list.push_back(f.cbits);
  attrib_list.push_back(GLX_GREEN_SIZE);
  attrib_list.push_back(f.cbits);
  attrib_list.push_back(GLX_BLUE_SIZE);
  attrib_list.push_back(f.cbits);
  attrib_list.push_back(GLX_ALPHA_SIZE);
  attrib_list.push_back(f.abits);
  attrib_list.push_back(0);

  int nelem=0;
  LOGN_DEBUG("offscreen buffer: glXChooseFBConfig");
  fbconfig_ =glXChooseFBConfig(dpy_,0,&attrib_list[0],&nelem);
  if(fbconfig_==0 || nelem==0) {
    LOGN_ERROR("error creating offscreen rendering context: glXChooseFBConfig failed");
    return;
  }

  attrib_list.clear();
  attrib_list.push_back(GLX_PBUFFER_WIDTH);
  attrib_list.push_back(width_);
  attrib_list.push_back(GLX_PBUFFER_HEIGHT);
  attrib_list.push_back(height_);
  attrib_list.push_back(0);
  
  LOGN_DEBUG("offscreen buffer: glXCreatePBuffer");
  pbuffer_ = glXCreatePbuffer(dpy_, fbconfig_[0], &attrib_list[0]);
  if(!pbuffer_) {
    LOGN_ERROR("error creating offscreen rendering context: glXCreatePBuffer failed");
    return;
  }

  if(shared) {
    LOGN_DEBUG("offscreen buffer: glxCreateNewContext(shared=true)");
    context_ = glXCreateNewContext(dpy_, fbconfig_[0], GLX_RGBA_TYPE, 
				   glXGetCurrentContext(), True);
  } else {
    LOGN_DEBUG("offscreen buffer: glxCreateNewContext(shared=false)");
    context_ = glXCreateNewContext(dpy_, fbconfig_[0], GLX_RGBA_TYPE, 
				   NULL, True);
    
  }

  if(!context_) {
    LOGN_ERROR("error creating offscreen rendering context: glXCreateNewContext failed");
    glXDestroyPbuffer(dpy_, pbuffer_);
    return;
  }

  valid_=true;
}
    
OffscreenBuffer::~OffscreenBuffer()
{
  if(valid_) {
    LOGN_DEBUG("offscreen buffer: glXDestroyContext()");
    glXDestroyContext(dpy_, context_);
    LOGN_DEBUG("offscreen buffer: glXDestroyPbuffer()");
    glXDestroyPbuffer(dpy_, pbuffer_);
  }
}

bool OffscreenBuffer::Resize(unsigned int width, unsigned int height)
{
  std::vector<int> attrib_list;
  attrib_list.push_back(GLX_PBUFFER_WIDTH);
  attrib_list.push_back(width);
  attrib_list.push_back(GLX_PBUFFER_HEIGHT);
  attrib_list.push_back(height);
  attrib_list.push_back(0);
  
  GLXPbuffer new_pbuffer = glXCreatePbuffer(dpy_, fbconfig_[0], &attrib_list[0]);

  if(!new_pbuffer) {
    LOGN_ERROR("offscreen rendering resize failed to allocate new pbuffer");
    return false;
  }
  
  GLXContext new_context = glXCreateNewContext(dpy_, fbconfig_[0], GLX_RGBA_TYPE, 
                                               glXGetCurrentContext(), True);
  
  if(!new_context) {
    LOGN_ERROR("offscreen rendering resize failed to get new context");
    return false;
  }

  glXDestroyContext(dpy_, context_);
  glXDestroyPbuffer(dpy_, pbuffer_);

  context_=new_context;
  pbuffer_=new_pbuffer;

  width_=width;
  height_=height;

  return true;
}

bool OffscreenBuffer::MakeActive()
{
  if(active_) return true;

  if(!glXMakeContextCurrent(dpy_, pbuffer_, pbuffer_, context_)) {
    LOGN_ERROR("error switching to offscreen rendering context: glXMakeContextCurrent failed");
    return false;
  }
  return true;
}

}} // ns
