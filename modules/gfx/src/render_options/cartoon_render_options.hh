//------------------------------------------------------------------------------
// This file is part of the OpenStructure project <www.openstructure.org>
//
// Copyright (C) 2008-2011 by the OpenStructure authors
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

#include "render_options.hh"

namespace ost { namespace gfx {

class DLLEXPORT_OST_GFX CartoonRenderOptions: public RenderOptions {
public:
  CartoonRenderOptions(bool force_tube=false);

  // RenderOptions interface
  virtual RenderMode::Type GetRenderMode();
  virtual bool CanApplyRenderOptions(RenderOptionsPtr render_options);
  virtual void ApplyRenderOptions(RenderOptionsPtr render_options);

  // own interface
  void SetSplineDetail(uint spline_detail);
  uint GetSplineDetail() const;

  void SetPolyMode(uint poly_mode);
  uint GetPolyMode() const;

  void SetArcDetail(uint arc_detail);
  uint GetArcDetail() const;

  void SetNormalSmoothFactor(float smooth_factor);
  float GetNormalSmoothFactor() const;

  void SetTubeRadius(float tube_radius);
  float GetTubeRadius() const;
  void SetTubeRatio(float tube_ratio);
  float GetTubeRatio() const;
  unsigned int GetTubeProfileType() const;
  void SetTubeProfileType(unsigned int);

  void SetHelixWidth(float helix_width);
  float GetHelixWidth() const;
  void SetHelixThickness(float helix_thickness);
  float GetHelixThickness() const;
  void SetHelixEcc(float helix_ecc);
  float GetHelixEcc() const;
  unsigned int GetHelixProfileType() const;
  void SetHelixProfileType(unsigned int);
  unsigned int GetHelixMode() const;
  void SetHelixMode(unsigned int);

  void SetStrandWidth(float strand_width);
  float GetStrandWidth() const;
  void SetStrandThickness(float strand_thickness);
  float GetStrandThickness() const;
  void SetStrandEcc(float strand_ecc);
  float GetStrandEcc() const;
  unsigned int GetStrandProfileType() const;
  void SetStrandProfileType(unsigned int);
  unsigned int GetStrandMode() const;
  void SetStrandMode(unsigned int);

  void SetColorBlendMode(unsigned int);
  unsigned int GetColorBlendMode() const;

  float GetMaxRad() const;

private:
  bool force_tube_;
  uint spline_detail_;
  uint poly_mode_;
  uint arc_detail_;

  float smooth_factor_;

  float tube_radius_;
  float tube_ratio_;
  unsigned int tube_profile_;
  float helix_width_;
  float helix_thickness_;
  float helix_ecc_;
  unsigned int helix_profile_;
  unsigned int helix_mode_;
  float strand_width_;
  float strand_thickness_;
  float strand_ecc_;
  unsigned int strand_profile_;
  unsigned int strand_mode_;

  unsigned int color_blend_mode_;
};

typedef boost::shared_ptr<CartoonRenderOptions> CartoonRenderOptionsPtr;

}}

#endif /* OST_GFX_CARTOON_RENDER_OPTIONS_HH */
