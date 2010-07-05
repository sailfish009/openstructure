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
#include <ost/config.hh>
namespace geom {

Vec3::Vec3()
{
  this->set(0.0,0.0,0.0);
}

Vec3::Vec3(Real x, Real y, Real z)
{
  this->set(x,y,z);
}

Vec3::Vec3(Real v)
{
  this->set(v, v, v);
}


//#if OST_DOUBLE_PRECISION
//Vec3::Vec3(float x, float y, float z)
//{
//  this->set(x,y,z);
//}
//#endif

Vec3::Vec3(const Vec3& v)
{
  this->set(v.data_[0],v.data_[1],v.data_[2]);
}

Vec3::Vec3(const Vec2& v)
{
  this->set(v[0],v[1],0.0);
}

Vec3::Vec3(const Vec4& v)
{
  if(v[3]==0.0) {
    throw DivideByZeroException("zero w component in Vec4->Vec3 conversion");
  }
  this->set(v[0]/v[3],v[1]/v[3],v[2]/v[3]);
}


Vec3::Vec3(const Real d[3])
{
  this->set(d[0],d[1],d[2]);
}
#if OST_DOUBLE_PRECISION
Vec3::Vec3(const float d[3])
{
  this->set(d[0],d[1],d[2]);
}
#endif

Vec3& Vec3::operator=(const Vec3& v)
{
  if(&v!=this) {
    this->set(v[0],v[1],v[2]);
  }
  return *this;
}

bool Vec3::operator==(const Vec3& v) const
{
  return data_[0]==v.data_[0] &&
    data_[1]==v.data_[1] &&
    data_[2]==v.data_[2];
}

Real& Vec3::operator[](std::size_t indx)
{
  if(indx>2) throw OutOfRangeException();
  return data_[indx];
}


const Real& Vec3::operator[](std::size_t indx) const
{
  if(indx>2) throw OutOfRangeException();
  return data_[indx];
}

Real Vec3::GetX() const
{
  return data_[0];
}

Real Vec3::GetY() const
{
  return data_[1];
}

Real Vec3::GetZ() const
{
  return data_[2];
}

void Vec3::SetX(Real v)
{
  data_[0]=v;
}
void Vec3::SetY(Real v)
{
  data_[1]=v;
}
void Vec3::SetZ(Real v)
{
  data_[2]=v;
}

Vec3 Vec3::operator-() const
{
  Vec3 nrvo(-data_[0],-data_[1],-data_[2]);
  return nrvo;
}

Vec3& Vec3::operator+=(const Vec3& rhs)
{
  data_[0]+=rhs[0]; data_[1]+=rhs[1]; data_[2]+=rhs[2];
  return *this;
}

Vec3& Vec3::operator-=(const Vec3& rhs)
{
  data_[0]-=rhs[0]; data_[1]-=rhs[1]; data_[2]-=rhs[2];
  return *this;
}

Vec3& Vec3::operator*=(Real d)
{
  data_[0]*=d; data_[1]*=d; data_[2]*=d;
  return *this;
}

Vec3& Vec3::operator+=(Real d)
{
  data_[0]+=d; data_[1]+=d; data_[2]+=d;
  return *this;
}

Vec3& Vec3::operator-=(Real d)
{
  data_[0]-=d; data_[1]-=d; data_[2]-=d;
  return *this;
}

Vec3& Vec3::operator/=(Real d)
{
  data_[0]/=d; data_[1]/=d; data_[2]/=d;
  return *this;
}

////////////////////////////////////////////
// private member functions

void Vec3::set(Real x, Real y, Real z)
{
  data_[0]=x; data_[1]=y; data_[2]=z;
}

////////////////////////////////////////////
// functions

Vec3 operator/(Real d, const Vec3& v)
{
  Vec3 nrvo(d/v[0],d/v[1],d/v[2]);
  return nrvo;
}

std::ostream& operator<<(std::ostream& os, const Vec3& v)
{
  os << "(" << v[0] << "," << v[1] << "," << v[2] << ")";
  return os;
}

} // ns geom

