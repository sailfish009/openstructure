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
  Authors: Ansgar Philippsen, Marco Biasini
*/

#include <ost/log.hh>

#include <ost/gfx/glext_include.hh>

#include "offscreen_buffer.hh"

#include <ost/gfx/scene.hh>

namespace ost { namespace gfx {

OffscreenBuffer& OffscreenBuffer::Instance()
{
  static OffscreenBuffer instance(500,500,8,8,8,8,24);
  return instance;
}

bool OffscreenBuffer::Begin() 
{
  LOG_DEBUG("switching to offscreen rendering");

  if(active_) return true;
  
  glGetIntegerv(GL_VIEWPORT,old_vp_);
#if defined(__linux__)

  old_context_ = glXGetCurrentContext();

  if(context_ != old_context_) {
    old_dr1_ = glXGetCurrentDrawable();
    old_dr2_ = glXGetCurrentReadDrawable();
    
    if(!glXMakeContextCurrent(dpy_, pbuffer_, pbuffer_, context_)) {
      LOG_ERROR("error switching to offscreen rendering context: glXMakeContextCurrent failed");
      return false;
    }
  }
#elif defined(__APPLE__)

  old_context_=CGLGetCurrentContext();

  if(context_ != old_context_) {
    if (CGLError err=CGLSetCurrentContext(context_)) {
      LOG_ERROR("error switching to offscreen rendering context. "
                 "CGLSetCurrentContext failed: " << CGLErrorString(err));
      return false;
    }
  }
#elif defined(_WIN32)
  /*old_context_ = wglGetCurrentContext();
  
  if(context_ != old_context_) {
    dev_context_=wglGetCurrentDC();
    if (BOOL err=wglMakeCurrent(dev_context_, context_)) {
      LOG_ERROR("error switching to offscreen rendering context. "
                 "wglMakeCurrent failed: ");
      return false;
    }
  }
  */
#endif
  active_=true;
  return true;
}

bool OffscreenBuffer::End() 
{
  LOG_DEBUG("switching back to normal rendering");

  if(!active_) return true;

#if defined(__linux__)

  if(context_ != old_context_) {
    // ignore error
    glXMakeContextCurrent(dpy_, old_dr1_, old_dr2_, old_context_);
  }

#elif defined(__APPLE__)

  if(context_!=old_context_) {
    // ignore error
    CGLSetCurrentContext(old_context_);
  }
  
#elif defined(_WIN32)
  /*
  if(context_!=old_context_) {
    old_dev_context_=wglGetCurrentDC();
    wglMakeCurrent(old_dev_context_, old_context_);
  }
  */
#endif  

  Scene::Instance().SetViewport(old_vp_[2],old_vp_[3]);
  active_=false;
  return true;
}

bool OffscreenBuffer::Resize(unsigned int width, unsigned int height)
{
#if defined(__linux__)

  std::vector<int> attrib_list;
  attrib_list.push_back(GLX_PBUFFER_WIDTH);
  attrib_list.push_back(width);
  attrib_list.push_back(GLX_PBUFFER_HEIGHT);
  attrib_list.push_back(height);
  attrib_list.push_back(0);
  
  GLXPbuffer new_pbuffer = glXCreatePbuffer(dpy_, fbconfig_[0], &attrib_list[0]);

  if(!new_pbuffer) {
    LOG_ERROR("offscreen rendering resize failed");
    return false;
  }
  
  GLXContext new_context = glXCreateNewContext(dpy_, fbconfig_[0], GLX_RGBA_TYPE, 
                                               glXGetCurrentContext(), True);
  
  if(!new_context) {
    LOG_ERROR("offscreen rendering resize failed to get new context");
    return false;
  }

  glXDestroyContext(dpy_, context_);
  glXDestroyPbuffer(dpy_, pbuffer_);

  context_=new_context;
  pbuffer_=new_pbuffer;

#elif defined(__APPLE__)
  CGLPBufferObj      new_pbuffer;  
  CGLError err=CGLCreatePBuffer(width, height, GL_TEXTURE_RECTANGLE_EXT,
                                GL_RGBA, 0,  &new_pbuffer);
  if (err) {
    LOG_ERROR("error resizing offscreen rendering context: "
               "CGLCreatePBuffer failed: " << CGLErrorString(err));
    return false;
  }
  GLint screen=0;
  assert(CGLGetVirtualScreen(context_, &screen)==0);  
  err=CGLSetPBuffer(context_, new_pbuffer, 0, 0, screen);
  if (err) {
    LOG_ERROR("error resizing offscreen rendering context. "
               "CGLSetPBuffer failed: " << CGLErrorString(err));
    return false;
  }
  CGLDestroyPBuffer(pbuffer_);
  pbuffer_=new_pbuffer;

#elif defined(_WIN32)
 /* int attribList[] = {0};
  int format = 0;

  HPBUFFERARB        new_pbuffer;
  new_pbuffer = wglCreatePbufferARB(dev_context_, format, width, height, attribList);
  if (new_pbuffer == NULL) 
  {
    LOG_ERROR("Error resizing offscreen rendering context (wglCreatePbufferARB failed)\n");
    return false;
  }

  dev_context_ = wglGetPbufferDCARB(new_pbuffer);
  if (dev_context_ == NULL) 
  {
    LOG_ERROR("Unable to retrieve handle to resized pbuffer device context\n");
    return false;
  }

  context_ = wglCreateContext(dev_context_);
  if (context_ == NULL) 
  {
    LOG_ERROR("Unable to create a rendering context for the resized pbuffer\n");
    return false;
  }
  //
  //if (!wglShareLists(old_context_, context_)) 
  //{
  //  LOG_ERROR("Unable to share data between resized rendering contexts\n");
  //  return;
  //}
  */
#endif
  // nothing failed, set new width and height
  width_=width;
  height_=height;
  return true;
}

bool OffscreenBuffer::IsValid() const
{
  return valid_;
}

bool OffscreenBuffer::IsActive() const
{
  return valid_;
}

OffscreenBuffer::OffscreenBuffer(int width, int height, int r_bits, 
                                 int b_bits, int g_bits, 
                                 int a_bits, int depth_bits):
  width_(width), height_(height),valid_(false)
{

#if defined(__linux__)

  if(getenv("DISPLAY")==NULL) {
    LOG_ERROR("error creating offscreen rendering context: missing DISPLAY environment variable");
    return;
  }
  dpy_ = XOpenDisplay(getenv("DISPLAY"));
  if(dpy_==NULL) {
    LOG_ERROR("error creating offscreen rendering context: XOpenDisplay failed");
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
  attrib_list.push_back(depth_bits);
  attrib_list.push_back(GLX_RED_SIZE);
  attrib_list.push_back(r_bits);
  attrib_list.push_back(GLX_GREEN_SIZE);
  attrib_list.push_back(g_bits);
  attrib_list.push_back(GLX_BLUE_SIZE);
  attrib_list.push_back(b_bits);
  attrib_list.push_back(GLX_ALPHA_SIZE);
  attrib_list.push_back(a_bits);
  attrib_list.push_back(0);

  int nelem=0;
  fbconfig_ =glXChooseFBConfig(dpy_,0,&attrib_list[0],&nelem);
  if(fbconfig_==0 || nelem==0) {
    LOG_ERROR("error creating offscreen rendering context: glXChooseFBConfig failed");
    return;
  }

  attrib_list.clear();
  attrib_list.push_back(GLX_PBUFFER_WIDTH);
  attrib_list.push_back(width);
  attrib_list.push_back(GLX_PBUFFER_HEIGHT);
  attrib_list.push_back(height);
  attrib_list.push_back(0);
  
  pbuffer_ = glXCreatePbuffer(dpy_, fbconfig_[0], &attrib_list[0]);
  if(!pbuffer_) {
    LOG_ERROR("error creating offscreen rendering context: glXCreatePBuffer failed");
    return;
  }

  context_ = glXCreateNewContext(dpy_, fbconfig_[0], GLX_RGBA_TYPE, 
                                 glXGetCurrentContext(), True);
  if(!context_) {
    LOG_ERROR("error creating offscreen rendering context: glXCreateNewContext failed");
    return;
  }

#elif defined(__APPLE__)                                   
  CGLPixelFormatAttribute attributes[]={
    kCGLPFAPBuffer,
    kCGLPFAColorSize, CGLPixelFormatAttribute(8),
    kCGLPFAAlphaSize, CGLPixelFormatAttribute(8),
    kCGLPFADepthSize, CGLPixelFormatAttribute(24),
    CGLPixelFormatAttribute(0)
  };
  GLint npix=0;
  CGLError err=CGLChoosePixelFormat(attributes, &pix_format_, &npix);
  if(err) {
    LOG_ERROR("error creating offscreen rendering context. "
               "CGLChoosePixFormat failed:" << CGLErrorString(err));
    return;
  }
  // if a context exists, share resources such as shader programs and display
  // lists.
  err=CGLCreateContext(pix_format_, CGLGetCurrentContext(), &context_);
  if(err) {
    LOG_ERROR("error creating offscreen rendering context. "
               "CGLCreateContext failed" << CGLErrorString(err));
    return;
  }
  err=CGLCreatePBuffer(width, height, GL_TEXTURE_RECTANGLE_EXT, GL_RGBA, 0, 
                       &pbuffer_);
  if (err) {
    LOG_ERROR("error creating offscreen rendering context. "
               "CGLCreatePBuffer failed: " << CGLErrorString(err));
    return;
  }
  GLint screen=0;
  assert(CGLGetVirtualScreen(context_, &screen)==0);  
  err=CGLSetPBuffer(context_, pbuffer_, 0, 0, screen);
  if (err) {
    LOG_ERROR("error creating offscreen rendering context. "
               "CGLSetPBuffer failed: " << CGLErrorString(err));
    return;
  }

#elif defined(_WIN32)
  /*    
  // store current windows device and rendering context
  dev_context_ = wglGetCurrentDC();
  context_ = wglGetCurrentContext();

  int format = 0;
  unsigned int nformats;
  int attribList[] = 
  {
      WGL_RED_BITS_ARB,               32,
      WGL_GREEN_BITS_ARB,             32,
      WGL_BLUE_BITS_ARB,              32,
      WGL_ALPHA_BITS_ARB,             32,
      WGL_STENCIL_BITS_ARB,           8,
      WGL_DEPTH_BITS_ARB,             24,
      WGL_FLOAT_COMPONENTS_NV,        true,
      WGL_DRAW_TO_PBUFFER_ARB,        true,
      0,
  };

  wglChoosePixelFormatARB(dev_context_, attribList, NULL, 1, &format, &nformats);
  if (nformats == 0)
  {
    LOG_ERROR("Unable to find any RGBA32 floating point pixel formats\n");
    return;
  }

  // clear attribute list
  //attribList[0] = 0;
  
  int attribs[] = {
      WGL_RED_BITS_ARB,               8,
      WGL_GREEN_BITS_ARB,             8,
      WGL_BLUE_BITS_ARB,              8,
      WGL_ALPHA_BITS_ARB,             8,
      WGL_STENCIL_BITS_ARB,           8,
      WGL_DEPTH_BITS_ARB,             24,
      0,
  };

  pbuffer_ = wglCreatePbufferARB(dev_context_, format, width, height, attribs);
  if (pbuffer_ == NULL) 
  {
    LOG_ERROR("Unable to create floating point pbuffer (wglCreatePbufferARB failed)\n");
    return;
  }

  old_dev_context_ = wglGetPbufferDCARB(pbuffer_);
  if (dev_context_ == NULL) 
  {
    LOG_ERROR("Unable to retrieve handle to pbuffer device context\n");
    return;
  }

  context_ = wglCreateContext(dev_context_);
  if (context_ == NULL) 
  {
    LOG_ERROR("Unable to create a rendering context for the pbuffer\n");
    return;
  }
  
  if (!wglShareLists(old_context_, context_)) 
  {
    LOG_ERROR("Unable to share data between rendering contexts\n");
    return;
  }
*/
#endif

  // nothing failed, all is good
  valid_=true;
  active_=false;
} 

}}
