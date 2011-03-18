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
  unsigned integer triplet based size

  Author: Ansgar Philippsen
*/

#ifndef IMG_SIZE_H
#define IMG_SIZE_H

#include <iosfwd>
#include <stdexcept>

#include <ost/img/module_config.hh>

namespace ost { namespace img {

class Point;

//! class encapsulating 1D to 3D size
class DLLEXPORT_OST_IMG_BASE Size {
 public:
  Size(): w_(1), h_(1), d_(1) { }
  
  Size(const Size &s): w_(s.w_), h_(s.h_), d_(s.d_) { }

  explicit Size(unsigned int w):
    w_(w<1 ? 1 : w), h_(1), d_(1)
  { }

  Size(unsigned int w, unsigned int h):
    w_(w<1 ? 1 : w), h_(h<1 ? 1 : h), d_(1)
  { }

  Size(unsigned int w, unsigned int h, unsigned int d):
    w_(w<1 ? 1 : w),
    h_(h<1 ? 1 : h),
    d_(d<1 ? 1 : d)
  {}

  //! initialize from two points
  Size(const Point& p1, const Point& p2);


  //! return the point at half the size
  /*!
    for even dimension, its N/2-1, for odd dimension its (N-1)/2
  */
  Point GetHalf() const;
  
  //! get width component (convenience to index 0)
  unsigned int GetWidth() const { return w_; }
  //! get height component (convenience to index 1)
  unsigned int GetHeight() const { return h_; }
  //! get depth component (convenience to index 2)
  unsigned int GetDepth() const { return d_; }
  //! get area (width * height)
  unsigned int GetSlab() const { return w_*h_; }
  //! get volume (depcrecated, use GetVolume() instead)
  unsigned int GetVol() const { return w_*h_*d_; }
  //! get volume
  unsigned int GetVolume() const { return w_*h_*d_; }

  //! return dimensionality (1-3)
  unsigned int GetDim() const { return d_>1 ? 3 : (h_>1 ? 2 : 1); }

  // some operators
  Size& operator=(const Size& s)
  {
    w_=s.w_;
    h_=s.h_;
    d_=s.d_;
    return *this;
  }

  unsigned int& operator[](unsigned int index) 
  {
    if (index>2)
      throw std::range_error("Size index out of range");
    return (&w_)[index];
  }

  unsigned int operator[](unsigned int index) const 
  {
    if (index>2)
      throw std::range_error("Size index out of range");
    return (&w_)[index];
  }

  bool operator==(const Size& s) const 
  {
    return equal(s);
  }

  bool operator!=(const Size& s) const 
  {
    return !equal(s);
  }

  Size& operator+=(const Size& s)
  {
    w_+=s.w_;
    h_+=s.h_;
    d_+=s.d_;
    return *this;
  }

  // clamped to 1
  Size& operator-=(const Size& s)
  {
    w_ = (w_>s.w_) ? w_-s.w_ : 1;
    h_ = (h_>s.h_) ? h_-s.h_ : 1;
    d_ = (d_>s.d_) ? d_-s.d_ : 1;
    return *this;
  }



 private:
  unsigned int w_,h_,d_;

  bool equal(const Size& s) const
  {
    return (w_==s.w_ && h_==s.h_ && d_==s.d_);
  }
};

inline Size operator+(const Size& s1, const Size& s2)
{
  Size s(s1);
  s+=s2;
  return s;
}
inline Size operator-(const Size& s1, const Size& s2)
{
  Size s(s1);
  s-=s2;
  return s2;
}

DLLEXPORT_OST_IMG_BASE std::ostream& operator<<(std::ostream& os, const Size& s);


}} // namespace img


#endif
