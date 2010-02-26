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
  Author: Ansgar Philippsen
*/

#ifndef IMAGE_STATE_SPATIAL_DOMAIN_HH
#define IMAGE_STATE_SPATIAL_DOMAIN_HH

#include <iostream>

#include <ost/base.hh>
#include <ost/iplt/point.hh>
#include <ost/iplt/size.hh>
#include <ost/iplt/extent.hh>
#include <ost/iplt/pixel_sampling.hh>
#include <ost/iplt/value_util.hh>
#include "value_holder.hh"
#include "index.hh"

namespace ost { namespace iplt { namespace image_state {

/*
  while all domain implementations share a common interface
  they should be seen as policy classes, used as a template
  parameter, and thus are not derived from a common base class.

  copy ctor and assignment op should work automatically
*/

class DLLEXPORT_OST_IPLT_BASE SpatialDomain {
public:
  SpatialDomain(const Extent& e):
    extent_(e),
    physical_extent_(ExtractPhysicalExtent(e))
  {}

  // interface for ImageStateImpl
  DataDomain GetDomain() const {return SPATIAL;}

  void SetSpatialOrigin(const Point& o) {
    extent_=Extent(o,extent_.GetSize());
  }

  Point GetSpatialOrigin() const {
    return extent_.GetStart();
  }

  Extent GetExtent() const {
    return extent_;
  }

  Extent GetLogicalExtent() const {
    return extent_;
  }

  Extent GetPhysicalExtent() const {
    return physical_extent_;
  }

  template <typename V>
  Real GetReal(const Point &p, const ValueHolder<V>& data) const {
    if(extent_.Contains(p)) {
      return Val2Val<V,Real>(data.Value(Point2Index(p)));
    } else {
      return 0.0;
    }
  }

  template <typename V>
  void SetReal(const Point &p, const Real& r, ValueHolder<V>& data) {
    if(extent_.Contains(p)) {
      data.Value(Point2Index(p))=Val2Val<Real,V>(r);
    }
  }

  template <typename V>
  Complex GetComplex(const Point &p, const ValueHolder<V>& data) const {
    if(extent_.Contains(p)) {
      return Val2Val<V,Complex>(data.Value(Point2Index(p)));
    } else {
      return Complex(0.0,0.0);
    }
  }

  template <typename V>
  void SetComplex(const Point &p, const Complex& c, ValueHolder<V>& data) {
    if(extent_.Contains(p)) {
      data.Value(Point2Index(p))=Val2Val<Complex,V>(c);
    }
  }

  Index Point2Index(const Point& p) const {
    return Index(p[0]-extent_.GetStart()[0],
		 p[1]-extent_.GetStart()[1],
		 p[2]-extent_.GetStart()[2]);
  }
  
private:
  Extent extent_;
  Extent physical_extent_;
  
  Extent ExtractPhysicalExtent(const Extent& e) const {
#if 1
    Point pad;
    if(e.GetDim()==1) {
      pad=Point( (e.GetSize()[0]&0x1) ? 1 : 2,0,0);
    } else if(e.GetDim()==2) {
      pad=Point(0,(e.GetSize()[1]&0x1) ? 1 : 2,0);
    } else {
      pad=Point(0,0,(e.GetSize()[2]&0x1) ? 1 : 2);
    }
    return Extent(e.GetStart(),e.GetEnd()+pad);
#else
    return Extent(e.GetStart(),e.GetEnd());
#endif
  }
};

}}} // ns

#endif
