//------------------------------------------------------------------------------
// This file is part of the OpenStructure project <www.openstructure.org>
//
// Copyright (C) 2008-2011 by the OpenStructure authors
// Copyright (C) 2003-2010 by the IPLT authors
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
  pixel sampling

  Author: Ansgar Philippsen
*/

#include "pixel_sampling.hh"

#include "extent.hh"
#include "point.hh"
#include "vecmat.hh"

#include <ost/log.hh>

namespace ost { namespace img {

PixelSampling::PixelSampling():
  dom_(SPATIAL),
  spat_scale_mat_(),
  i_spat_scale_mat_(),
  freq_scale_mat_(),
  i_freq_scale_mat_(),
  fac_(Vec3(1.0,1.0,1.0))
{
  SetPixelSampling(Vec3(1.0,1.0,1.0));
}

PixelSampling::PixelSampling(const Vec3& sampling, DataDomain d, const Extent& e):
  dom_(d),
  spat_scale_mat_(),
  i_spat_scale_mat_(),
  freq_scale_mat_(),
  i_freq_scale_mat_(),
  fac_(Vec3(e.GetSize().GetWidth(),
	     e.GetSize().GetHeight(),
	     e.GetSize().GetDepth()))
{
  if(dom_==HALF_FREQUENCY) dom_=FREQUENCY;
  //SetPixelSampling(sampling);
  set_spat_scale(Mat3(sampling[0], 0.0, 0.0,
                      0.0, sampling[1], 0.0,
                      0.0, 0.0, sampling[2]));
}



void PixelSampling::SetPixelSampling(const Vec3& d)
{
  dom_==SPATIAL ? set_spat_scale(Mat3(d[0], 0.0, 0.0, 
                                      0.0, d[1], 0.0, 
                                      0.0, 0.0, d[2])) 
                : set_freq_scale(Mat3(d[0], 0.0, 0.0, 
                                      0.0, d[1], 0.0, 
                                      0.0, 0.0, d[2]));
}

void PixelSampling::SetSpatialSampling(const Vec3& s)
{
  set_spat_scale(Mat3(s[0], 0.0, 0.0, 
                      0.0, s[1], 0.0, 
                      0.0, 0.0, s[2]));
}

void PixelSampling::SetFrequencySampling(const Vec3& s)
{
  set_freq_scale(Mat3(s[0], 0.0, 0.0, 
                      0.0, s[1], 0.0, 
                      0.0, 0.0, s[2]));
}

void PixelSampling::SetDomain(DataDomain d)
{
  if(d==HALF_FREQUENCY) d=FREQUENCY;
  dom_=d;
}

void PixelSampling::SetExtent(const Extent& e)
{
  fac_=Vec3(e.GetSize().GetWidth(),
	          e.GetSize().GetHeight(),
	          e.GetSize().GetDepth());
}

Vec3 PixelSampling::Point2Coord(const Point& p) const
{
  return dom_==SPATIAL ? spat_scale_mat_*p.ToVec3() : freq_scale_mat_*p.ToVec3();
}

Vec3 PixelSampling::Coord2Point(const Vec3& c) const
{
  return dom_==SPATIAL ? i_spat_scale_mat_*c : i_freq_scale_mat_*c;
}

Vec3 PixelSampling::Vec2Coord(const Vec3& p) const
{
  return dom_==SPATIAL ? spat_scale_mat_*p : freq_scale_mat_*p;
}


void PixelSampling::set_spat_scale(const Mat3& dim)
{
  spat_scale_mat_=dim;
  spat_scale_vec_=Vec3(Length(dim*Vec3(1, 0, 0)),
                       Length(dim*Vec3(0, 1, 0)),
                       Length(dim*Vec3(0, 0, 1)));
  i_spat_scale_mat_=geom::Invert(spat_scale_mat_);
  i_spat_scale_vec_=Vec3(1.0/spat_scale_vec_.x, 1.0/spat_scale_vec_.y, 
                         1.0/spat_scale_vec_.z);
  freq_scale_mat_=Mat3(i_spat_scale_mat_(0, 0)/fac_[0], i_spat_scale_mat_(0, 1)/fac_[1], 
                       i_spat_scale_mat_(0, 2)/fac_[2], i_spat_scale_mat_(1, 0)/fac_[0], 
                       i_spat_scale_mat_(1, 1)/fac_[1], i_spat_scale_mat_(1, 2)/fac_[2],
                       i_spat_scale_mat_(2, 0)/fac_[0], i_spat_scale_mat_(2, 1)/fac_[1], 
                       i_spat_scale_mat_(2, 2)/fac_[2]);
  freq_scale_vec_=Vec3(Length(freq_scale_mat_*Vec3(1, 0, 0)),
                       Length(freq_scale_mat_*Vec3(0, 1, 0)),
                       Length(freq_scale_mat_*Vec3(0, 0, 1)));
  i_freq_scale_mat_=geom::Invert(freq_scale_mat_);
  i_freq_scale_vec_=Vec3(1.0/freq_scale_vec_.x, 1.0/freq_scale_vec_.y, 
                         1.0/freq_scale_vec_.z);
}

void PixelSampling::set_freq_scale(const Mat3& dim)
{
  freq_scale_mat_ = dim;
  freq_scale_vec_ = Vec3(Length(dim*Vec3(1, 0, 0)), 
                         Length(dim*Vec3(0, 1, 0)), 
                         Length(dim*Vec3(0, 0, 1)));
  i_freq_scale_mat_ = Invert(freq_scale_mat_);
  i_freq_scale_vec_ = Vec3(1.0/freq_scale_vec_.x, 1.0/freq_scale_vec_.y, 
                           1.0/freq_scale_vec_.z);
  spat_scale_mat_=Mat3(i_freq_scale_mat_(0, 0)/fac_[0], i_freq_scale_mat_(0, 1)/fac_[1], 
                       i_freq_scale_mat_(0, 2)/fac_[2], i_freq_scale_mat_(1, 0)/fac_[0], 
                       i_freq_scale_mat_(1, 1)/fac_[1], i_freq_scale_mat_(1, 2)/fac_[2],
                       i_freq_scale_mat_(2, 0)/fac_[0], i_freq_scale_mat_(2, 1)/fac_[1], 
                       i_freq_scale_mat_(2, 2)/fac_[2]);
  spat_scale_vec_ = Vec3(Length(spat_scale_mat_*Vec3(1, 0, 0)),
                         Length(spat_scale_mat_*Vec3(0, 1, 0)),
                         Length(spat_scale_mat_*Vec3(0, 0, 1)));
  i_spat_scale_mat_ = geom::Invert(spat_scale_mat_);
  i_spat_scale_vec_ = Vec3(1.0/spat_scale_vec_.x, 
                           1.0/spat_scale_vec_.y, 
                           1.0/spat_scale_vec_.z);
  
}

void PixelSampling::SetFrequencySamplingMat(const Mat3& d)
{
  set_freq_scale(d);
}

void PixelSampling::SetSpatialSamplingMat(const Mat3& d)
{
  set_spat_scale(d);
}




}} // namespace
