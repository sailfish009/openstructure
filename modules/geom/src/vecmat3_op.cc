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
  Real dot_product = Dot(Normalize(v1), Normalize(v2));
  dot_product=std::max(static_cast<Real>(-1.0),dot_product);
  dot_product=std::min(static_cast<Real>(1.0),dot_product);
  return std::acos(dot_product);
}

Real SignedAngle(const Vec3& v1, const Vec3& v2, const Vec3& ref_normal )
{
  Vec3 a=Normalize(v1);
  Vec3 b=Normalize(v2);
  Vec3 c=Normalize(ref_normal);
  return std::atan2(Dot(c,Cross(a,b)), Dot(a,b));
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


Real DihedralAngle(const Vec3& p1, const Vec3& p2, const Vec3& p3, 
                   const Vec3&p4)
{
  Vec3 r1=p2-p1;
  Vec3 r2=p3-p2;
  Vec3 r3=p4-p3;
  Vec3 r12cross = Cross(r1, r2);
  Vec3 r23cross = Cross(r2, r3);
  return atan2(Dot(r1*Length(r2), r23cross),
               Dot(r12cross, r23cross));
}

Mat3 Mat3FromAngles(Real alpha, Real beta, Real gamma)
{
  Real cos_alpha=cos(alpha);
  Real cos_beta=cos(beta);  
  Real sin_beta=sin(beta);

  Real cos_gamma=cos(gamma);
  Real sin_gamma=sin(gamma);
  Real cos_alpha_s=(cos_gamma*cos_beta-cos_alpha)/(sin_beta*sin_gamma);
  Real sin_alpha_s=sqrt(1.0-cos_alpha_s*cos_alpha_s);
  geom::Vec3 a_dir(1, 0, 0);
  geom::Vec3 b_dir(cos_gamma, sin_gamma, 0.0);
  geom::Vec3 c_dir(cos_beta, -sin_beta*cos_alpha_s, sin_beta*sin_alpha_s);
  return geom::Mat3(a_dir.x, b_dir.x, c_dir.x,
                    a_dir.y, b_dir.y, c_dir.y,
                    a_dir.z, b_dir.z, c_dir.z);
}

} // ns
