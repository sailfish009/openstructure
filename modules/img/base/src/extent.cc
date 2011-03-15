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
  Extent implementation

  Author: Ansgar Philippsen
*/

#include <cstdlib>
#include <iostream>

#include <ost/img/module_config.hh>
#include "extent.hh"
#include "vecmat.hh"

namespace ost { namespace img {

Extent::Extent() {set(Point(),Point());}

Extent::Extent(const Extent &r) {set(r.start_,r.end_);}

Extent::Extent(const Point& p1, const Point& p2) {set(p1,p2);}

Extent::Extent(const Size& size)
{
  set(Point(0,0,0), size - Point(1,1,1));
}

Extent::Extent(const Size& size, const Point& cen)
{
  Point st = -size.GetHalf() + cen;
  set(st, size + st - Point(1,1,1));
}

Extent::Extent(const Point& start, const Size& size)
{
  set(start, size + start - Point(1,1,1));
}

const Point& Extent::GetStart() const {return start_;}

void Extent::SetStart(const Point& o) 
{
  set(o,end_);
}

void Extent::SetEnd(const Point& e) 
{
  set(start_,e);
}

const Point& Extent::GetEnd() const {return end_;}

bool Extent::Contains(const Point& p) const
{
  return (p[0]>=start_[0] && p[0]<=end_[0] &&
	  p[1]>=start_[1] && p[1]<=end_[1] &&
	  p[2]>=start_[2] && p[2]<=end_[2]);
}

bool Extent::Contains(const Extent& e) const
{
  return Contains(e.GetStart()) && Contains(e.GetEnd());
}

Point Extent::GetCenter() const 
{
  return size_.GetHalf()+start_;
}

const Size& Extent::GetSize() const {return size_;}

int Extent::GetWidth() const {return size_.GetWidth();}

int Extent::GetHeight() const {return size_.GetHeight();}

int Extent::GetDepth() const {return size_.GetDepth();}

int Extent::GetVolume() const {return size_.GetVolume();}

int Extent::GetDim() const {return dim_;}


Point Extent::WrapAround(const Point& p) const
{
  Point r(p-start_);
  for(int i=0;i<3;i++) {
    r[i] = start_[i] + (r[i]<0 ? size_[i]+std::div(r[i],size_[i]).rem : std::div(r[i],size_[i]).rem);
  }
  return r;
}


Extent Extent::Mirror(int planes)
{
  Point new_start(planes & Plane::YZ ? -end_[0] : start_[0],
		  planes & Plane::XZ ? -end_[1] : start_[1],
		  planes & Plane::XY ? -end_[2] : start_[2]);
  return Extent(new_start,size_);
}

unsigned int Extent::Point2Offset(const Point& p) const
{
  if(this->Contains(p)) {
    Point d(p-start_);
    return d[0]+size_[0]*(d[1]+d[2]*size_[1]);
  } else {
    return 0;
  }
}

void Extent::Shift(const Point& p)
{
  set(start_+p,end_+p);
}

void Extent::AddBorder(int border)
{
  this->set(start_-img::Point(border,border,border), 
            end_+img::Point(border,border,border));
}

// private methods

void Extent::set(const Point& p1, const Point& p2) 
{
  // order them automatically
  for(int i=0;i<3;i++) {
    if(p1[i]<=p2[i]) {
      start_[i]=p1[i];
      end_[i]=p2[i];
    } else {
      start_[i]=p2[i];
      end_[i]=p1[i];
    }
  }
  size_ = Size(start_,end_);
  dim_ = size_.GetDim();
}

bool Extent::equal(const Extent& b) const 
{
  return (start_==b.start_ && end_==b.end_);
}

// 'global' functions


Extent Overlap(const Extent& e1, const Extent& e2)
{
  Point a,b;

  for(int k=0;k<3;++k) {
    a[k] = std::max(e1.GetStart()[k],e2.GetStart()[k]);
    b[k] = std::min(e1.GetEnd()[k],e2.GetEnd()[k]);
    if(b[k]<a[k]) throw InvalidExtentException("non-overlapping extents in call to Overlap");
  }
  return Extent(a,b);
}

bool HasOverlap(const Extent& e1, const Extent& e2)
{
  Point a,b;

  for(int k=0;k<3;++k) {
    a[k] = std::max(e1.GetStart()[k],e2.GetStart()[k]);
    b[k] = std::min(e1.GetEnd()[k],e2.GetEnd()[k]);
    if(b[k]<a[k]) return false;
  }
  return true;
}



std::ostream& operator<<(std::ostream& os, const Extent &b) 
{
  os << "[" << b.GetStart() << "," << b.GetEnd() << "]";
  return os;
}


}}// namespace
