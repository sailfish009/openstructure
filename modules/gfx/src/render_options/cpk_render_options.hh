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
  Author: Stefan Scheuber
*/

#ifndef OST_GFX_CPK_RENDER_OPTIONS_HH
#define OST_GFX_CPK_RENDER_OPTIONS_HH

#include <boost/shared_ptr.hpp>
#include <ost/base.hh>

#include <ost/gfx/module_config.hh>
#include <ost/gfx/render_options/render_options.hh>

namespace ost { namespace gfx {

class DLLEXPORT_OST_GFX CPKRenderOptions: public RenderOptions {
public:
  CPKRenderOptions();

  virtual RenderMode::Type GetRenderMode();
  virtual bool CanApplyRenderOptions(RenderOptionsPtr render_options);
  virtual void ApplyRenderOptions(RenderOptionsPtr render_options);

  virtual void SetSphereDetail(uint detail);
  virtual uint GetSphereDetail();

  virtual void SetCPKMode(uint mode);
  virtual uint GetCPKMode();


  virtual ~CPKRenderOptions();

private:
  uint sphere_detail_;
  uint cpk_mode_;

};

typedef boost::shared_ptr<CPKRenderOptions> CPKRenderOptionsPtr;

}}

#endif /* OST_GFX_CPK_RENDER_OPTIONS_HH */
