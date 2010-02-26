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

Vec4::Vec4()
{
  this->set(0.0,0.0,0.0,1.0);
}

Vec4::Vec4(Real x, Real y, Real z, Real w)
{
  this->set(x,y,z,w);
}

Vec4::Vec4(const Vec4& v)
{
  this->set(v.data_[0],v.data_[1],v.data_[2],v.data_[3]);
}

Vec4::Vec4(const Vec2& v)
{
  this->set(v[0],v[1],0.0,1.0);
}

Vec4::Vec4(const Vec3& v)
{
  this->set(v[0],v[1],v[2],1.0);
}

Vec4::Vec4(const Real d[4])
{
  this->set(d[0],d[1],d[2],d[3]);
}

/*Vec4::Vec4(const float d[4])
{
  this->set(d[0],d[1],d[2],d[3]);
}
*/
Vec4& Vec4::operator=(const Vec4& v)
{
  if(&v!=this) {
    this->set(v[0],v[1],v[2],v[3]);
  }
  return *this;
}

bool Vec4::operator==(const Vec4& v) const
{
  return data_[0]==v.data_[0] && 
    data_[1]==v.data_[1] &&
    data_[2]==v.data_[2] &&
    data_[3]==v.data_[3];
}

Real& Vec4::operator[](std::size_t indx)
{
  if(indx>3) throw OutOfRangeException();
  return data_[indx];
}

const Real& Vec4::operator[](std::size_t indx) const
{
  if(indx>3) throw OutOfRangeException();
  return data_[indx];
}

Vec4 Vec4::operator-() const
{
  Vec4 nrvo(-data_[0],-data_[1],-data_[2],-data_[3]);
  return nrvo;
}

Vec4& Vec4::operator+=(const Vec4& rhs)
{
  data_[0]+=rhs[0]; data_[1]+=rhs[1]; data_[2]+=rhs[2]; data_[3]+=rhs[3];
  return *this;
}

Vec4& Vec4::operator-=(const Vec4& rhs)
{
  data_[0]-=rhs[0]; data_[1]-=rhs[1]; data_[2]-=rhs[2]; data_[3]-=rhs[3];
  return *this;
}

Vec4& Vec4::operator*=(Real d)
{
  data_[0]*=d; data_[1]*=d; data_[2]*=d; data_[3]*=d;
  return *this;
}

Vec4& Vec4::operator+=(Real d)
{
  data_[0]+=d; data_[1]+=d; data_[2]+=d; data_[3]+=d;
  return *this;
}

Vec4& Vec4::operator-=(Real d)
{
  data_[0]-=d; data_[1]-=d; data_[2]-=d; data_[3]-=d;
  return *this;
}

Vec4& Vec4::operator/=(Real d)
{
  data_[0]/=d; data_[1]/=d; data_[2]/=d; data_[3]/=d;
  return *this;
}

////////////////////////////////////////////
// private member functions

void Vec4::set(Real x, Real y, Real z, Real w)
{
  data_[0]=x; data_[1]=y; data_[2]=z; data_[3]=w;
}

////////////////////////////////////////////
// functions

Vec4 operator/(Real d, const Vec4& v)
{
  Vec4 nrvo(d/v[0],d/v[1],d/v[2],d/v[3]);
  return nrvo;
}

std::ostream& operator<<(std::ostream& os, const Vec4& v)
{
  os << "(" << v[0] << "," << v[1] << "," << v[2] << "," << v[3] << ")";
  return os;
}

}
