//------------------------------------------------------------------------------
// This file is part of the OpenStructure project <www.openstructure.org>
//
// Copyright (C) 2008-2010 by the OpenStructure authors
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
  function base
  
  Author: Ansgar Philippsen
*/

#include "function_base.hh"
#include "data_algorithm.hh"
#include "image_factory.hh"
#include "image_handle.hh"
namespace ost {
namespace img {

Function::Function(DataDomain d, const Extent& e):
  extent_(e),
  domain_(d),
  obs_(DataObservable()),
  sampling_(Vec3(1.0,1.0,1.0),domain_,extent_)
{}

Function::Function(const Function& f):
  Data(f),
  extent_(f.extent_),
  domain_(f.domain_),
  obs_(f.obs_), // use Observable copy logic
  sampling_(f.sampling_)
{}

Function& Function::operator=(const Function& f)
{
  Data::operator=(f);
  extent_=f.extent_;
  domain_=f.domain_;
  obs_=f.obs_; // use Observable assignement logic
  sampling_=f.sampling_;
  return *this;
}

Function::~Function()
{}

DataDomain Function::GetDomain() const 
{
  return domain_;
}

Extent Function::GetExtent() const 
{
  return extent_;
}

void Function::SetSpatialOrigin(const Point& o) 
{
  extent_=Extent(o,extent_.GetSize());
}

Point Function::GetSpatialOrigin() const
{
  return extent_.GetStart();
}

void Function::SetExtent(const Extent& e) 
{
  extent_ = e;
  sampling_.SetExtent(extent_);
}

void Function::Apply(NonModAlgorithm& a) const
{
  a.Visit(*this);
}
void Function::ApplyIP(NonModAlgorithm& a) const
{
  Apply(a);
}


void Function::Attach(DataObserver *o) const
{
  obs_.Attach(o);
}

void Function::Detach(DataObserver *o) const
{
  obs_.Detach(o);
}

void Function::Notify() const
{
  obs_.Notify();
}

PixelSampling& Function::Sampling()
{
  return sampling_;
}

const PixelSampling& Function::Sampling() const
{
  return sampling_;
}

Real Function::GetReal(const Point& p) const
{
  return GetIntpolReal(p.ToVec3());
}

Complex Function::GetComplex(const Point& p) const
{
  return GetIntpolComplex(p.ToVec3());
}

Real Function::GetIntpolReal(const Vec2 &v) const
{
  Vec3 v3(v);
  v3[2]=extent_.GetStart()[2];
  return GetIntpolReal(v3);
}
Real Function::GetIntpolReal(const Real &d) const
{
  Vec3 v3(d,extent_.GetStart()[1],extent_.GetStart()[2]);
  return GetIntpolReal(v3);
}

Complex Function::GetIntpolComplex(const Vec2 &v) const
{
  Vec3 v3(v);
  v3[2]=extent_.GetStart()[2];
  return GetIntpolComplex(v3);
}
Complex Function::GetIntpolComplex(const Real &d) const
{
  Vec3 v3(d,extent_.GetStart()[1],extent_.GetStart()[2]);
  return GetIntpolComplex(v3);
}

}} //ns
