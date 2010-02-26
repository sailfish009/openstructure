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
#include "vec2.hh"
#include "vec3.hh"
#include "vec4.hh"

#include "exc.hh"

namespace geom {

Vec2::Vec2()
{
  this->set(0.0,0.0);
}

Vec2::Vec2(Real x, Real y)
{
  this->set(x,y);
}

Vec2::Vec2(const Vec2& v)
{
  this->set(v.data_[0],v.data_[1]);
}

Vec2::Vec2(const Vec3& v)
{
  this->set(v[0],v[1]); // z is silenly swallowed
}

Vec2::Vec2(const Vec4& v) 
{
  if(v[3]==0.0) {
    throw DivideByZeroException("zero w component in Vec4->Vec2 conversion");
  }
  this->set(v[0]/v[3],v[1]/v[3]); // z is silently swallowed
}

Vec2::Vec2(const Real d[2])
{
  this->set(d[0],d[1]);
}

/*Vec2::Vec2(const float d[2])
{
  this->set(d[0],d[1]);
}
*/
Vec2& Vec2::operator=(const Vec2& v)
{
  if(&v!=this) {
    this->set(v[0],v[1]);
  }
  return *this;
}

bool Vec2::operator==(const Vec2& v) const
{
  return data_[0]==v.data_[0] && 
    data_[1]==v.data_[1];
}


Real& Vec2::operator[](std::size_t indx)
{
  if(indx>1) throw OutOfRangeException();
  return data_[indx];
}


const Real& Vec2::operator[](std::size_t indx) const
{
  if(indx>1) throw OutOfRangeException();
  return data_[indx];
}


Vec2 Vec2::operator-() const
{
  Vec2 nrvo(-data_[0],-data_[1]);
  return nrvo;
}

Vec2& Vec2::operator+=(const Vec2& rhs)
{
  data_[0]+=rhs[0]; data_[1]+=rhs[1];
  return *this;
}

Vec2& Vec2::operator-=(const Vec2& rhs)
{
  data_[0]-=rhs[0]; data_[1]-=rhs[1];
  return *this;
}

Vec2& Vec2::operator*=(Real d)
{
  data_[0]*=d; data_[1]*=d;
  return *this;
}

Vec2& Vec2::operator+=(Real d)
{
  data_[0]+=d; data_[1]+=d;
  return *this;
}

Vec2& Vec2::operator-=(Real d)
{
  data_[0]-=d; data_[1]-=d;
  return *this;
}

Vec2& Vec2::operator/=(Real d)
{
  data_[0]/=d; data_[1]/=d;
  return *this;
}

////////////////////////////////////////////
// private member functions

void Vec2::set(Real x, Real y)
{
  data_[0]=x; data_[1]=y;
}

////////////////////////////////////////////
// functions

Vec2 operator/(Real d, const Vec2& v)
{
  Vec2 nrvo(d/v[0],d/v[1]);
  return nrvo;
}

std::ostream& operator<<(std::ostream& os, const Vec2& v)
{
  os << "(" << v[0] << "," << v[1] << ")";
  return os;
}

Real Vec2::GetX() const
{
  return data_[0];
}

Real Vec2::GetY() const
{
  return data_[1];
}

void Vec2::SetX(Real v)
{
  data_[0]=v;
}
void Vec2::SetY(Real v)
{
  data_[1]=v;
}


} // ns
