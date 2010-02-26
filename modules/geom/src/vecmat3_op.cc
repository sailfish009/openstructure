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
#include <iostream>

#include "vecmat3_op.hh"
#include "vecmat2_op.hh"

#include "vec3.hh"
#include "mat3.hh"
#include "mat2.hh"
#include "exc.hh"

namespace geom {

Real Length(const Vec3& v)
{
  return std::sqrt(Length2(v));
}

Real Length2(const Vec3& v)
{
  return v[0]*v[0]+v[1]*v[1]+v[2]*v[2];
}

bool Equal(const Vec3& v1, const Vec3& v2, Real ephilon)
{
  return std::fabs(v1[0]-v2[0])<ephilon &&
    std::fabs(v1[1]-v2[1])<ephilon &&
    std::fabs(v1[2]-v2[2])<ephilon;
}

bool Equal(const Mat3& m1, const Mat3& m2, Real ephilon)
{
  return std::fabs(m1(0,0)-m2(0,0))<ephilon &&
    std::fabs(m1(0,1)-m2(0,1))<ephilon &&
    std::fabs(m1(0,2)-m2(0,2))<ephilon &&
    std::fabs(m1(1,0)-m2(1,0))<ephilon &&
    std::fabs(m1(1,1)-m2(1,1))<ephilon &&
    std::fabs(m1(1,2)-m2(1,2))<ephilon &&
    std::fabs(m1(2,0)-m2(2,0))<ephilon &&
    std::fabs(m1(2,1)-m2(2,1))<ephilon &&
    std::fabs(m1(2,2)-m2(2,2))<ephilon;
}

Real Dot(const Vec3& v1, const Vec3& v2)
{
  return v1[0]*v2[0]+v1[1]*v2[1]+v1[2]*v2[2];
}

Vec3 Cross(const Vec3& v1, const Vec3& v2)
{
  Vec3 nrvo(v1[1]*v2[2]-v2[1]*v1[2],
      v1[2]*v2[0]-v2[2]*v1[0],
      v1[0]*v2[1]-v2[0]*v1[1]);
  return nrvo;
}

Vec3 Normalize(const Vec3& v)
{
  Real l=Length(v);
  if(l==0.0) {
    return v;
  }
  return v/l;
}

Vec3 CompMultiply(const Vec3& v1, const Vec3& v2)
{
  Vec3 nrvo(v1[0]*v2[0],v1[1]*v2[1],v1[2]*v2[2]);
  return nrvo;
}

Vec3 CompDivide(const Vec3& v1, const Vec3& v2)
{
  Vec3 nrvo(v1[0]/v2[0],v1[1]/v2[1],v1[2]/v2[2]);
  return nrvo;
}

Vec3 operator*(const Vec3& v,const Mat3& m)
{
  Vec3 nrvo(v[0]*m(0,0)+v[1]*m(1,0)+v[2]*m(2,0),
      v[0]*m(0,1)+v[1]*m(1,1)+v[2]*m(2,1),
      v[0]*m(0,2)+v[1]*m(1,2)+v[2]*m(2,2));
  return nrvo;
}

Vec3 operator*(const Mat3& m, const Vec3& v)
{
  Vec3 nrvo(v[0]*m(0,0)+v[1]*m(0,1)+v[2]*m(0,2),
      v[0]*m(1,0)+v[1]*m(1,1)+v[2]*m(1,2),
      v[0]*m(2,0)+v[1]*m(2,1)+v[2]*m(2,2));
  return nrvo;
}

Mat3 operator*(const Mat3& m1, const Mat3& m2)
{
  Mat3 nrvo(m1(0,0)*m2(0,0)+m1(0,1)*m2(1,0)+m1(0,2)*m2(2,0),
      m1(0,0)*m2(0,1)+m1(0,1)*m2(1,1)+m1(0,2)*m2(2,1),
      m1(0,0)*m2(0,2)+m1(0,1)*m2(1,2)+m1(0,2)*m2(2,2),

      m1(1,0)*m2(0,0)+m1(1,1)*m2(1,0)+m1(1,2)*m2(2,0),
      m1(1,0)*m2(0,1)+m1(1,1)*m2(1,1)+m1(1,2)*m2(2,1),
      m1(1,0)*m2(0,2)+m1(1,1)*m2(1,2)+m1(1,2)*m2(2,2),

      m1(2,0)*m2(0,0)+m1(2,1)*m2(1,0)+m1(2,2)*m2(2,0),
      m1(2,0)*m2(0,1)+m1(2,1)*m2(1,1)+m1(2,2)*m2(2,1),
      m1(2,0)*m2(0,2)+m1(2,1)*m2(1,2)+m1(2,2)*m2(2,2));
  return nrvo;
}

Real Comp(const Mat3& m,unsigned int i_in, unsigned int j_in)
{
  return Minor(m,i_in,j_in)* static_cast<Real>( ((i_in+j_in)&0x1) ? -1 : 1);
}

Real Minor(const Mat3& m, unsigned int i_in, unsigned int j_in)
{
  if(i_in>2 || j_in>2){ 
    throw OutOfRangeException();
  }
  Mat2 r;
  unsigned int itmp=0;
  for (unsigned int i=0;i<3;i++) {
    if(i==i_in) continue;
    unsigned int jtmp=0;
    for (unsigned int j=0;j<3;j++){
      if(j==j_in) continue;
      r(itmp,jtmp)=m(i,j);
      jtmp++;
    }
    itmp++;
  }
  
  return Det(r);
}

Mat3 Transpose(const Mat3& m)
{
  Mat3 r;
  for (int i=0;i<3;i++){
    for (int j=0;j<3;j++){
      r(j,i)=m(i,j);
    }
  }
  return r;
}

Mat3 Invert(const Mat3& m)
{
  Mat3 r;
  Real d=Det(m);
  if(d<1.0e-30) d=1.0e30;
  for (int i=0;i<3;i++){
    for (int j=0;j<3;j++){
      r(j,i)=Comp(m,i,j)/d;
    }
  }
  return r;
}

Real Det(const Mat3& m)
{
#if 0
  return m(0,0)*m(1,1)*m(2,2)-m(0,0)*m(1,2)*m(2,1)-m(0,1)*m(1,0)*m(2,2)+m(0,2)*m(1,0)*m(2,1)-m(0,2)*m(1,1)*m(2,0);
#else
  Real d=0;
  for(int i=0;i<3;i++)
  {
    d+=m(0,i)*Comp(m,0,i);
  }
  return d;
#endif
}

Real Angle(const Vec3& v1, const Vec3& v2)
{
  return std::acos(Dot(Normalize(v1), Normalize(v2)));
}

Mat3 EulerTransformation(Real theta, Real phi, Real xi)
{
  Real costheta=cos(theta);
  Real sintheta=sin(theta);
  Real cosphi=cos(phi);
  Real sinphi=sin(phi);
  Real cosxi=cos(xi);
  Real sinxi=sin(xi);

  // rotation of xi around z-axis
  Mat3 rmat1(cosxi, sinxi, 0.0,
       -sinxi, cosxi, 0.0,
       0.0, 0.0, 1.0);

  // out of plane rotation of phi around y-axis
  Mat3 rmat2(cosphi, 0.0, sinphi,
       0.0, 1.0, 0.0,
       -sinphi, 0.0, cosphi);

  // xy plane orientation of theta - again rotation around z-axis
  Mat3 rmat3(costheta, -sintheta, 0.0,
       sintheta, costheta, 0.0,
       0.0, 0.0, 1.0);

  /*
    note column vector multiplication ala v2=A B C v
    where C is the first transformation, followed by B and A
  */
  Mat3 nrvo=rmat3*rmat2*rmat1;
  return nrvo;
}

Vec3 OrthogonalVector(const Vec3& vec) {
  if (vec[0] < vec[1]) {
    if (vec[0] < vec[2])
      return Normalize(Cross(vec, Vec3(1, 0, 0)+vec));
    else
      return Normalize(Cross(vec, Vec3(0, 0, 1)+vec));
  } else {
    if (vec[1] < vec[2]) 
      return Normalize(Cross(vec, Vec3(0, 1, 0)+vec));
    else
      return Normalize(Cross(vec, Vec3(0, 0, 2)+vec));     
  }
}

Mat3 AxisRotation(const Vec3& axis, Real angle)
{
  Real sa=sin(angle);
  Real ca=cos(angle);
  Real v=Length(axis);
  if(v==0.0) return Mat3();

  Vec3 vv=1.0/v * axis;
  
  Real x=vv[0];
  Real y=vv[1];
  Real z=vv[2];
  Real xx=x*x;
  Real xy=x*y;
  Real xz=x*z;
  Real yy=y*y;
  Real yz=y*z;
  Real zz=z*z;

  return Mat3(xx+ca-xx*ca, xy-ca*xy-sa*z, xz-ca*xz+sa*y,
          xy-ca*xy+sa*z, yy+ca-ca*yy, yz-ca*yz-sa*x,
          xz-ca*xz-sa*y, yz-ca*yz+sa*x,zz+ca-ca*zz);
}

Vec3 Min(const Vec3& v1, const Vec3& v2)
{
  Vec3 nrvo(std::min(v1[0],v2[0]),
            std::min(v1[1],v2[1]),
            std::min(v1[2],v2[2]));
  return nrvo;
}

Vec3 Max(const Vec3& v1, const Vec3& v2)
{
  Vec3 nrvo(std::max(v1[0],v2[0]),
            std::max(v1[1],v2[1]),
            std::max(v1[2],v2[2]));
  return nrvo;
}

Real  Distance(const Vec3& p1, const Vec3& p2)
{
  return Length(p1-p2);
}

} // ns
