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
  Author: Marco Biasini
*/

#ifndef OST_GFX_TRACE_RENDER_OPTIONS_HH
#define OST_GFX_TRACE_RENDER_OPTIONS_HH

#include <boost/shared_ptr.hpp>

#include <ost/gfx/module_config.hh>

#include <ost/gfx/render_options/render_options.hh>

namespace ost { namespace gfx {

class DLLEXPORT_OST_GFX TraceRenderOptions: public ost::gfx::RenderOptions {
public:
  TraceRenderOptions();

  virtual RenderMode::Type GetRenderMode();

  virtual bool CanApplyRenderOptions(RenderOptionsPtr render_options);
  virtual void ApplyRenderOptions(RenderOptionsPtr render_options);

  void SetArcDetail(uint arc_detail);
  uint GetArcDetail() const;

  void SetNormalSmoothFactor(float smooth_factor);
  float GetNormalSmoothFactor() const;

  void SetTubeRadius(float tube_radius);
  float GetTubeRadius() const;
  float GetMaxRad() const;

  virtual ~TraceRenderOptions();

private:
  uint spline_detail_;
  uint poly_mode_;
  uint arc_detail_;
  float smooth_factor_;
  float tube_radius_;
};

typedef boost::shared_ptr<TraceRenderOptions> TraceRenderOptionsPtr;

}}

#endif /* OST_GFX_CARTOON_RENDER_OPTIONS_HH */
