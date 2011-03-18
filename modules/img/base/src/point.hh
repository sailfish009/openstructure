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
  integer triplet based point

  Author: Ansgar Philippsen
*/

#ifndef IMG_POINT_H
#define IMG_POINT_H

#include <vector>
#include <iosfwd>

#include <ost/img/module_config.hh>

#include "vecmat.hh"

namespace ost { namespace img {

//fw decl
class Size;

//! class encapsulating 1D to 3D point
class DLLEXPORT_OST_IMG_BASE Point {
public:
  Point() { data_[0]=data_[1]=data_[2]=0; }
  Point(const Point &p) { data_[0]=p[0]; data_[1]=p[1]; data_[2]=p[2]; }

  //! 1D int constructor
  explicit Point(int a) { data_[0]=a; data_[1]=data_[2]=0;}
  //! 2D int constructor
  Point(int a, int b) { data_[0]=a; data_[1]=b; data_[2]=0; }
  //! 3D int constructor
  Point(int a, int b, int c) { data_[0]=a; data_[1]=b; data_[2]=c; }
  //! conversion from Vec2
  explicit Point(const Vec2& v) { data_[0]=v.x; data_[1]=v.y; }

  //! conversion from Vec3
  explicit Point(const Vec3& v) { data_[0]=v.x; data_[1]=v.y; data_[2]=v.z; }

  //! conversion from Vec4 (normalization)
  explicit Point(const Vec4& v)
  {
    if(std::abs(v[3])<1e-100) {
      throw std::range_error("4th element of Vec4 is too close to zero for normalization");
    } else {
      Real sf = 1.0/v[3];
      data_[0]=static_cast<int>(round(v[0]*sf));
      data_[1]=static_cast<int>(round(v[1]*sf));
      data_[2]=static_cast<int>(round(v[2]*sf));
    }    
  }
  
  //! (implicit) conversion of size to point
  Point(const Size& s);

  // operators
  int& operator[](unsigned int index) 
  {
    if(index>2) throw std::range_error("Point index out of range");
    return data_[index];
  }

  int operator[](unsigned int index) const 
  {
    if(index>2) throw std::range_error("Point index out of range");
    return data_[index];
  }

  Point& operator+=(const Point& p) 
  {
    data_[0]+=p.data_[0];   
    data_[1]+=p.data_[1];
    data_[2]+=p.data_[2];
    return *this;
  }

  Point& operator-=(const Point& p) 
  {
    data_[0]-=p.data_[0];   
    data_[1]-=p.data_[1];
    data_[2]-=p.data_[2];
    return *this;
  }

  Point operator-() const 
  {
    return Point(-data_[0],-data_[1],-data_[2]);
  }

  Point& operator=(const Point& p) 
  {
    data_[0]=p.data_[0];
    data_[1]=p.data_[1];
    data_[2]=p.data_[2];
    return *this;
  }


  bool operator==(const Point &p) const 
  {
    return equal(p);
  }

  bool operator!=(const Point &p) const
  {
    return !equal(p);
  }

  bool operator<(const Point &p) const 
  {
    return less(p);
  }

  bool operator<=(const Point &p) const 
  {
    return (equal(p) || less(p));
  }

  bool operator>(const Point &p) const
  {
    return p.less((*this));
  }

  bool operator>=(const Point &p) const 
  {
    return (equal(p) || p.less((*this)));
  }

  Point& operator+=(const Size& s);

  Point& operator-=(const Size& s);
  
  Vec2 ToVec2() const 
  {
    Vec2 nrvo(static_cast<Real>(data_[0]),
              static_cast<Real>(data_[1]));
    return nrvo;
  }

  Vec3 ToVec3() const 
  {
    Vec3 nrvo(static_cast<Real>(data_[0]),
        static_cast<Real>(data_[1]),
        static_cast<Real>(data_[2]));
    return nrvo;
  }

  Vec4 ToVec4() const 
  {
    Vec4 nrvo(static_cast<Real>(data_[0]),
        static_cast<Real>(data_[1]),
        static_cast<Real>(data_[2]),
        1.0);
    return nrvo;
  }
  
  Point Mirror(int planes)
  {
    return Point(planes & Plane::YZ ? -data_[0] : data_[0],
                 planes & Plane::XZ ? -data_[1] : data_[1],
                 planes & Plane::XY ? -data_[2] : data_[2]);
  }

private:
  int data_[3];

  bool equal(const Point &p) const 
  {
    return  (data_[0]==p.data_[0] && 
             data_[1]==p.data_[1] && 
             data_[2]==p.data_[2]);
  };

  bool less(const Point &p) const 
  {
    return ( data_[2]!=p.data_[2] ? data_[2]<p.data_[2] : (data_[1]!=p.data_[1] ?  data_[1]<p.data_[1] : data_[0]<p.data_[0]) ); 
  }

  Point absolute() const 
  {
    return Point(std::abs(data_[0]),std::abs(data_[1]),std::abs(data_[2]));
  }

  friend Point absolute(const Point&);
};

inline Point operator+(const Point& p1, const Point& p2)
{
  Point r(p1);
  r+=(p2);
  return r;
}

inline Point operator-(const Point& p1, const Point& p2)
{
  Point r(p1);
  r-=(p2);
  return r;
}

inline Point absolute(const Point& p)
{
  return p.absolute();
}

DLLEXPORT_OST_IMG_BASE std::ostream& operator<<(std::ostream& os, const Point &p);


}} // namespace img


#endif



