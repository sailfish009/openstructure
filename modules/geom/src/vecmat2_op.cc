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
#include <cmath>

#include "vecmat2_op.hh"
#include "vecmat3_op.hh"
#include "vec3.hh"

#include "vec2.hh"
#include "mat2.hh"

namespace geom {

Real Length(const Vec2& v)
{
  return std::sqrt(Length2(v));
}

Real Length2(const Vec2& v)
{
  return v[0]*v[0]+v[1]*v[1];
}

bool Equal(const Vec2& v1, const Vec2& v2, Real ephilon)
{
  return std::fabs(v1[0]-v2[0])<ephilon &&
    std::fabs(v1[1]-v2[1])<ephilon;
}
bool Equal(const Mat2& m1, const Mat2& m2, Real ephilon)
{
  return std::fabs(m1(0,0)-m2(0,0))<ephilon &&
    std::fabs(m1(0,1)-m2(0,1))<ephilon &&
    std::fabs(m1(1,0)-m2(1,0))<ephilon &&
    std::fabs(m1(1,1)-m2(1,1))<ephilon;
}

Real Dot(const Vec2& v1, const Vec2& v2)
{
  return v1[0]*v2[0]+v1[1]*v2[1];
}

Vec2 Normalize(const Vec2& v)
{
  Real l=Length(v);
  if(l==0.0) {
    return v;
  }
  return v/l;
}


Vec2 CompMultiply(const Vec2& v1, const Vec2& v2)
{
  Vec2 nrvo(v1[0]*v2[0],v1[1]*v2[1]);
  return nrvo;
}

Vec2 CompDivide(const Vec2& v1, const Vec2& v2)
{
  Vec2 nrvo(v1[0]/v2[0],v1[1]/v2[1]);
  return nrvo;
}

Vec2 operator*(const Vec2& v,const Mat2& m)
{
  Vec2 nrvo(v[0]*m(0,0)+v[1]*m(1,0),
      v[0]*m(0,1)+v[1]*m(1,1));
  return nrvo;
}

Vec2 operator*(const Mat2& m, const Vec2& v)
{
  Vec2 nrvo(v[0]*m(0,0)+v[1]*m(0,1),
      v[0]*m(1,0)+v[1]*m(1,1));
  return nrvo;
}
// determinant
Real Det(const Mat2& m)
{
  return m(0,0)*m(1,1)-m(0,1)*m(1,0);
}

Mat2 Transpose(const Mat2& m)
{
  Mat2 r;
  for (int i=0;i<2;i++){
    for (int j=0;j<2;j++){
      r(j,i)=m(i,j);
    }
  }
  return r;
}
Mat2 Invert(const Mat2& m)
{
  Real d=Det(m);
  return Mat2(m(1,1),-m(0,1),-m(1,0),m(0,0))/d;    
}

Real Angle(const Vec2& v1, const Vec2& v2)
{
  return std::acos(Dot(v1,v2)/Length(v1)/Length(v2));
}
Mat2 operator*(const Mat2& m1, const Mat2& m2)
{
  Mat2 nrvo(m1(0,0)*m2(0,0)+m1(0,1)*m2(1,0),
      m1(0,0)*m2(0,1)+m1(0,1)*m2(1,1),
      m1(1,0)*m2(0,0)+m1(1,1)*m2(1,0),
      m1(1,0)*m2(0,1)+m1(1,1)*m2(1,1));
  return nrvo;
}

Real SignedAngle(const Vec2& v1, const Vec2& v2)
{
  Vec3 vc;
  vc=Cross(Vec3(v1),Vec3(v2));
  if(Length(v1)==0.0 || Length(v2)==0.0 || Length(vc)==0.0)
    return 0.0;
  return acos(Dot(v1,v2)/Length(v1)/Length(v2))*vc.GetZ()/std::fabs(vc.GetZ());
}

Vec2 Min(const Vec2& v1, const Vec2& v2)
{
  Vec2 nrvo(std::min(v1[0],v2[0]),
            std::min(v1[1],v2[1]));
  return nrvo;
}

Vec2 Max(const Vec2& v1, const Vec2& v2)
{
  Vec2 nrvo(std::max(v1[0],v2[0]),
            std::max(v1[1],v2[1]));
  return nrvo;
}

Vec2 Rotate(const Vec2& v,Real ang)
{
  return Mat2(cos(ang),-sin(ang),sin(ang),cos(ang))*v;
}

} // ns
