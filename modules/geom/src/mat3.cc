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
#include "mat.hh"
#include "exc.hh"

#include "vecmat3_op.hh"
#include "vec3.hh"

namespace geom {

Mat3::Mat3()
{
  this->set(1.0,0.0,0.0, 0.0,1.0,0.0, 0.0,0.0,1.0);
}

Mat3::Mat3(Real i00, Real i01, Real i02,Real i10, Real i11, Real i12,Real i20, Real i21, Real i22)
{
  this->set(i00,i01,i02,i10,i11,i12,i20,i21,i22);
}

Mat3::Mat3(const Mat3& m)
{
  this->set(m(0,0),m(0,1),m(0,2),m(1,0),m(1,1),m(1,2),m(2,0),m(2,1),m(2,2));
}

Mat3::Mat3(const Mat2& m)
{
  this->set(m(0,0),m(0,1),0.0, m(1,0),m(1,1),0.0, 0.0,0.0,1.0);
}

Mat3::Mat3(const Real arr[9])
{
  this->set(arr[0],arr[1],arr[2],arr[3],arr[4],arr[5],arr[6],arr[7],arr[8]);
}

Mat3& Mat3::operator=(const Mat3& m)
{
  if(&m!=this) {
    this->set(m(0,0),m(0,1),m(0,2),m(1,0),m(1,1),m(1,2),m(2,0),m(2,1),m(2,2));
  }
  return *this;
}

Mat3 Mat3::Identity() 
{
  static Mat3 i(1.0,0.0,0.0,
                0.0,1.0,0.0,
                0.0,0.0,1.0);
  return i;
}

bool Mat3::operator==(const Mat3& rhs) const
{
  return data_[0][0] ==  rhs.data_[0][0] &&
    data_[1][0] == rhs.data_[1][0] &&
    data_[2][0] == rhs.data_[2][0] &&
    data_[0][1] == rhs.data_[0][1] &&
    data_[1][1] ==  rhs.data_[1][1] &&
    data_[2][1] ==  rhs.data_[2][1] &&
    data_[0][2] ==  rhs.data_[0][2] &&
    data_[1][2] ==  rhs.data_[1][2] &&
    data_[2][2] ==  rhs.data_[2][2];
}

Real& Mat3::operator()(std::size_t r, std::size_t c)
{
  if(r>2 || c>2) throw OutOfRangeException();
  return data_[r][c];
}

const Real& Mat3::operator()(std::size_t r, std::size_t c) const
{
  if(r>2 || c>2) throw OutOfRangeException();
  return data_[r][c];
}


Mat3& Mat3::operator+=(const Mat3& rhs)
{
  data_[0][0]+=rhs(0,0);
  data_[0][1]+=rhs(0,1);
  data_[0][2]+=rhs(0,2);
  data_[1][0]+=rhs(1,0);
  data_[1][1]+=rhs(1,1);
  data_[1][2]+=rhs(1,2);
  data_[2][0]+=rhs(2,0);
  data_[2][1]+=rhs(2,1);
  data_[2][2]+=rhs(2,2);
  return *this;
}
Mat3& Mat3::operator-=(const Mat3& rhs)
{
  data_[0][0]-=rhs(0,0);
  data_[0][1]-=rhs(0,1);
  data_[0][2]-=rhs(0,2);
  data_[1][0]-=rhs(1,0);
  data_[1][1]-=rhs(1,1);
  data_[1][2]-=rhs(1,2);
  data_[2][0]-=rhs(2,0);
  data_[2][1]-=rhs(2,1);
  data_[2][2]-=rhs(2,2);
  return *this;
}
Mat3& Mat3::operator*=(const Real d)
{
  data_[0][0]*=d;
  data_[0][1]*=d;
  data_[0][2]*=d;
  data_[1][0]*=d;
  data_[1][1]*=d;
  data_[1][2]*=d;
  data_[2][0]*=d;
  data_[2][1]*=d;
  data_[2][2]*=d;
  return *this;
}
Mat3& Mat3::operator/=(const Real d)
{
  data_[0][0]/=d;
  data_[0][1]/=d;
  data_[0][2]/=d;
  data_[1][0]/=d;
  data_[1][1]/=d;
  data_[1][2]/=d;
  data_[2][0]/=d;
  data_[2][1]/=d;
  data_[2][2]/=d;
  return *this;
}

Mat3& Mat3::operator*=(const Mat3& m)
{
  (*this)=operator*(*this,m);
  return *this;
}

geom::Vec3 Mat3::GetCol(int index) const
{
  return geom::Vec3(data_[0][index], data_[1][index], data_[2][index]);  
}

geom::Vec3 Mat3::GetRow(int index) const
{
  return geom::Vec3(data_[index][0], data_[index][1], data_[index][2]);
}


////////////////////////////////////////////
// private member functions
void Mat3::set(Real i00, Real i01, Real i02,Real i10, Real i11, Real i12,Real i20, Real i21, Real i22)
{
  data_[0][0]=i00; data_[0][1]=i01; data_[0][2]=i02;
  data_[1][0]=i10; data_[1][1]=i11; data_[1][2]=i12;
  data_[2][0]=i20; data_[2][1]=i21; data_[2][2]=i22;
}

std::ostream& operator<<(std::ostream& os, const Mat3& m)
{
  os << "{{" << m(0,0) << "," << m(0,1) << "," << m(0,2) << "},";
  os << "{" << m(1,0) << "," << m(1,1) << "," << m(1,2) << "},";
  os << "{" << m(2,0) << "," << m(2,1) << "," << m(2,2) << "}}";
  return os;
}

} // ns
