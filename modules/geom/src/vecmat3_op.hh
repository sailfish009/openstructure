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
#ifndef GEOM_VECMAT3_OP_HH
#define GEOM_VECMAT3_OP_HH

#include <ostream>
#include "constants.hh"

#include <ost/geom/module_config.hh>
#include <ost/geom/vec3.hh>
#include <ost/geom/mat3.hh>

namespace geom {


//! returns squared length of vector
inline Real Length2(const Vec3& v)
{
  return v[0]*v[0]+v[1]*v[1]+v[2]*v[2];
}

//! returns length of vector
inline Real Length(const Vec3& v)
{
  return std::sqrt(Length2(v));
}

//! return true if components differ not more than epsilon
inline bool Equal(const Vec3& v1, const Vec3& v2, 
                  Real ephilon=EPSILON)
{
  return std::fabs(v1[0]-v2[0])<ephilon &&
    std::fabs(v1[1]-v2[1])<ephilon &&
    std::fabs(v1[2]-v2[2])<ephilon;
}

//! return true if components differ not more than ephilon
inline bool Equal(const Mat3& m1, const Mat3& m2, 
                  Real ephilon=EPSILON)
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

//! vector dot product
inline Real Dot(const Vec3& v1, const Vec3& v2)
{
  return v1[0]*v2[0]+v1[1]*v2[1]+v1[2]*v2[2];
}

//! Normalize
inline Vec3 Normalize(const Vec3& v)
{
  Real l=Length(v);
  if(l==0.0) {
    return v;
  }
  return v/l;
}

//! vector cross product
inline Vec3 Cross(const Vec3& v1, const Vec3& v2)
{
  Vec3 nrvo(v1[1]*v2[2]-v2[1]*v1[2],
      v1[2]*v2[0]-v2[2]*v1[0],
      v1[0]*v2[1]-v2[0]*v1[1]);
  return nrvo;
}

//! multiply each component of v1 with that of v2
inline Vec3 CompMultiply(const Vec3& v1, const Vec3& v2)
{
  Vec3 nrvo(v1[0]*v2[0],v1[1]*v2[1],v1[2]*v2[2]);
  return nrvo;
}

//! divide each component of v1 by that of v2
inline Vec3 CompDivide(const Vec3& v1, const Vec3& v2)
{
  Vec3 nrvo(v1[0]/v2[0],v1[1]/v2[1],v1[2]/v2[2]);
  return nrvo;
}

//! vector matrix multiplication
inline Vec3 operator*(const Vec3& v,const Mat3& m)
{
  Vec3 nrvo(v[0]*m(0,0)+v[1]*m(1,0)+v[2]*m(2,0),
      v[0]*m(0,1)+v[1]*m(1,1)+v[2]*m(2,1),
      v[0]*m(0,2)+v[1]*m(1,2)+v[2]*m(2,2));
  return nrvo;
}

//! vector matrix multiplication
inline Vec3 operator*(const Mat3& m, const Vec3& v)
{
  Vec3 nrvo(v[0]*m(0,0)+v[1]*m(0,1)+v[2]*m(0,2),
      v[0]*m(1,0)+v[1]*m(1,1)+v[2]*m(1,2),
      v[0]*m(2,0)+v[1]*m(2,1)+v[2]*m(2,2));
  return nrvo;
}

//! matrix matrix multiplication
inline Mat3 operator*(const Mat3& m1, const Mat3& m2)
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

Mat3 DLLEXPORT_OST_GEOM Invert(const Mat3& m);
Mat3 DLLEXPORT_OST_GEOM Transpose(const Mat3& m);

// algebraic complement
Real DLLEXPORT_OST_GEOM Comp(const Mat3& m, unsigned int i, unsigned int j);

// minor
Real DLLEXPORT_OST_GEOM Minor(const Mat3& m, unsigned int i, unsigned int j);

// determinant
Real DLLEXPORT_OST_GEOM Det(const Mat3& m);

// angle between zwo vectors
Real DLLEXPORT_OST_GEOM Angle(const Vec3& v1, const Vec3& v2);

Mat3 DLLEXPORT_OST_GEOM EulerTransformation(Real theta, Real phi, Real xi);

Mat3 DLLEXPORT_OST_GEOM AxisRotation(const Vec3& axis, Real angle);

/// \brief get arbitrary vector orthogonal to axis
/// 
/// The returned vector is of length 1
Vec3 DLLEXPORT_OST_GEOM OrthogonalVector(const Vec3& axis);


Real DLLEXPORT_OST_GEOM DihedralAngle(const Vec3& p1, const Vec3& p2, 
                                      const Vec3& p3, const Vec3&p4);

//! returns std::min of each component
inline Vec3 Min(const Vec3& v1, const Vec3& v2)
{
  Vec3 nrvo(std::min(v1[0],v2[0]),
            std::min(v1[1],v2[1]),
            std::min(v1[2],v2[2]));
  return nrvo;
}

//! returns std::max of each component
inline Vec3 Max(const Vec3& v1, const Vec3& v2)
{
  Vec3 nrvo(std::max(v1[0],v2[0]),
            std::max(v1[1],v2[1]),
            std::max(v1[2],v2[2]));
  return nrvo;
}

//! returns the distance between two points
inline Real Distance(const Vec3& p1, const Vec3& p2)
{
    return Length(p1-p2);
}

} // ns

#endif
