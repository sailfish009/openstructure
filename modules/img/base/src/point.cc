//------------------------------------------------------------------------------
// This file is part of the OpenStructure project <www.openstructure.org>
//
// Copyright (C) 2008-2015 by the OpenStructure authors
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
  point

  Author: Ansgar Philippsen
*/

#include <ost/message.hh>
#include <iostream>
#include <sstream>

#include <ost/img/peak.hh>
#include "point.hh"
#include "size.hh"
#include "vecmat.hh"

namespace ost { namespace img {

Point::Point():
  data_()
{
  data_[0]=0;
  data_[1]=0;
  data_[2]=0;
} 

Point::Point(int a):
  data_()
{
  data_[0]=a;
  data_[1]=0;
  data_[2]=0;
} 

Point::Point(int a, int b):
  data_()
{
  data_[0]=a;
  data_[1]=b;
  data_[2]=0;
} 

Point::Point(int a, int b, int c):
  data_()
{
  data_[0]=a;
  data_[1]=b;
  data_[2]=c;
} 

Point::Point(const Vec2& v):
  data_()
{
  data_[0]=static_cast<int>(round(v[0]));
  data_[1]=static_cast<int>(round(v[1]));
  data_[2]=0;
} 

Point::Point(const Vec3& v):
  data_()
{
  data_[0]=static_cast<int>(round(v[0]));
  data_[1]=static_cast<int>(round(v[1]));
  data_[2]=static_cast<int>(round(v[2]));
} 

Point::Point(const Vec4& v):
  data_()
{
  if(std::abs(v[3])<1e-100) {
    throw geom::OutOfRangeException("4th element of Vec4 is too close to zero for normalization");
  } else {
    Real sf = 1.0/v[3];
    data_[0]=static_cast<int>(round(v[0]*sf));
    data_[1]=static_cast<int>(round(v[1]*sf));
    data_[2]=static_cast<int>(round(v[2]*sf));
  }
}

Point::Point(const Size& s):
  data_()
{
  data_[0]=s[0];
  data_[1]=s[1];
  data_[2]=s[2];
}

Point::Point(const Point &p):
  data_()
{
  data_[0]=p.data_[0];
  data_[1]=p.data_[1];
  data_[2]=p.data_[2];
} 

Point Point::Mirror(int planes)
{
  return Point(planes & Plane::YZ ? -data_[0] : data_[0],
               planes & Plane::XZ ? -data_[1] : data_[1],
               planes & Plane::XY ? -data_[2] : data_[2]);
}


bool Point::equal(const Point &p) const 
{
  return  (data_[0]==p.data_[0] && 
           data_[1]==p.data_[1] && 
           data_[2]==p.data_[2]);
};

bool Point::less(const Point &p) const 
{
  return ( data_[2]!=p.data_[2] ? data_[2]<p.data_[2] : (data_[1]!=p.data_[1] ?  data_[1]<p.data_[1] : data_[0]<p.data_[0]) ); 
}

Point Point::absolute() const 
{
  return Point(std::abs(data_[0]),std::abs(data_[1]),std::abs(data_[2]));
}

Point& Point::operator+=(const Point& p) 
{
  data_[0]+=p.data_[0];   
  data_[1]+=p.data_[1];
  data_[2]+=p.data_[2];
  return *this;
}

Point& Point::operator-=(const Point& p) 
{
  data_[0]-=p.data_[0];   
  data_[1]-=p.data_[1];
  data_[2]-=p.data_[2];
  return *this;
}

Point Point::operator-() const 
{
  return Point(-data_[0],-data_[1],-data_[2]);
}

Point& Point::operator=(const Point& p) 
{
  data_[0]=p.data_[0];
  data_[1]=p.data_[1];
  data_[2]=p.data_[2];
  return *this;
}


bool Point::operator==(const Point &p) const 
{
  return equal(p);
}

bool Point::operator!=(const Point &p) const
{
  return !equal(p);
}

bool Point::operator<(const Point &p) const 
{
  return less(p);
}

bool Point::operator<=(const Point &p) const 
{
  return (equal(p) || less(p));
}

bool Point::operator>(const Point &p) const
{
  return p.less((*this));
}

bool Point::operator>=(const Point &p) const 
{
  return (equal(p) || p.less((*this)));
}

Point& Point::operator+=(const Size& s) 
{
  data_[0]+=s[0];   
  data_[1]+=s[1];
  data_[2]+=s[2];
  return *this;
}

Point& Point::operator-=(const Size& s) 
{
  data_[0]-=s[0];   
  data_[1]-=s[1];
  data_[2]-=s[2];
  return *this;
}

Vec2 Point::ToVec2() const 
{
  Vec2 nrvo(static_cast<Real>(data_[0]),
            static_cast<Real>(data_[1]));
  return nrvo;
}

Vec3 Point::ToVec3() const 
{
  Vec3 nrvo(static_cast<Real>(data_[0]),
      static_cast<Real>(data_[1]),
      static_cast<Real>(data_[2]));
  return nrvo;
}

Vec4 Point::ToVec4() const 
{
  Vec4 nrvo(static_cast<Real>(data_[0]),
      static_cast<Real>(data_[1]),
      static_cast<Real>(data_[2]),
      1.0);
  return nrvo;
}

int& Point::operator[](unsigned int index) 
{
  if(index>2) throw std::range_error("Point index out of range");
  return data_[index];
}

int Point::operator[](unsigned int index) const 
{
  if(index>2) throw geom::OutOfRangeException("Point index out of range");
  return data_[index];
}

// global funcs

Point operator+(const Point& p1, const Point& p2) 
{
  Point r(p1);
  r+=(p2);
  return r;
}

Point operator-(const Point& p1, const Point& p2) {
  Point r(p1);
  r-=(p2);
  return r;
}



Point absolute(const Point& p) {
  return p.absolute();
}

std::ostream& operator<<(std::ostream& os, const Point &p) 
{
  os << "(" << p[0] << "," << p[1] << "," << p[2] << ")";
  return os;
}


}} // namespace
