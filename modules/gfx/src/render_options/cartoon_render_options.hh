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

#ifndef OST_GFX_CARTOON_RENDER_OPTIONS_HH
#define OST_GFX_CARTOON_RENDER_OPTIONS_HH

#include <boost/shared_ptr.hpp>

#include <ost/gfx/module_config.hh>

#include <ost/gfx/render_options/render_options.hh>

namespace ost { namespace gfx {

class DLLEXPORT_OST_GFX CartoonRenderOptions: public ost::gfx::RenderOptions {
public:
  CartoonRenderOptions();

  virtual RenderMode::Type GetRenderMode();

  virtual bool CanApplyRenderOptions(RenderOptionsPtr render_options);
  virtual void ApplyRenderOptions(RenderOptionsPtr render_options);

  virtual void SetSplineDetail(uint spline_detail);
  virtual uint GetSplineDetail() const;

  virtual void SetPolyMode(uint poly_mode);
  virtual uint GetPolyMode() const;

  virtual void SetArcDetail(uint arc_detail);
  virtual uint GetArcDetail() const;

  virtual void SetNormalSmoothFactor(float smooth_factor);
  virtual float GetNormalSmoothFactor() const;

  virtual void SetTubeRadius(float tube_radius);
  virtual float GetTubeRadius() const;
  virtual void SetTubeRatio(float tube_ratio);
  virtual float GetTubeRatio() const;

  virtual void SetHelixWidth(float helix_width);
  virtual float GetHelixWidth() const;
  virtual void SetHelixThickness(float helix_thickness);
  virtual float GetHelixThickness() const;
  virtual void SetHelixEcc(float helix_ecc);
  virtual float GetHelixEcc() const;

  virtual void SetStrandWidth(float strand_width);
  virtual float GetStrandWidth() const;
  virtual void SetStrandThickness(float strand_thickness);
  virtual float GetStrandThickness() const;
  virtual void SetStrandEcc(float strand_ecc);
  virtual float GetStrandEcc() const;

  float GetMaxRad() const;

  virtual ~CartoonRenderOptions();

private:
  uint spline_detail_;
  uint poly_mode_;
  uint arc_detail_;

  float smooth_factor_;

  float tube_radius_;
  float tube_ratio_;
  float helix_width_;
  float helix_thickness_;
  float helix_ecc_;
  float strand_width_;
  float strand_thickness_;
  float strand_ecc_;

};

typedef boost::shared_ptr<CartoonRenderOptions> CartoonRenderOptionsPtr;

}}

#endif /* OST_GFX_CARTOON_RENDER_OPTIONS_HH */
