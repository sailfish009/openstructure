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
#ifndef GEOM_VECMAT2_OP_HH
#define GEOM_VECMAT2_OP_HH

#include "constants.hh"

#include <ost/geom/module_config.hh>

namespace geom {

class Vec2;
class Mat2;

//! returns length of vector
Real DLLEXPORT_OST_GEOM Length(const Vec2& v);

//! returns squared length of vector
Real DLLEXPORT_OST_GEOM Length2(const Vec2& v);

//! return true if components differ not more than ephilon
bool DLLEXPORT_OST_GEOM Equal(const Vec2& v1, const Vec2& v2, Real ephilon=EPSILON);
//! return true if components differ not more than ephilon
bool DLLEXPORT_OST_GEOM Equal(const Mat2& m1, const Mat2& m2, Real ephilon=EPSILON);

//! vector dot product
Real DLLEXPORT_OST_GEOM Dot(const Vec2& v1, const Vec2& v2);

//! Normalize vector
Vec2 DLLEXPORT_OST_GEOM Normalize(const Vec2& v);

//! multiply each component of v1 with that of v2
Vec2 DLLEXPORT_OST_GEOM CompMultiply(const Vec2& v1, const Vec2& v2);

//! divide each component of v1 by that of v2
Vec2 DLLEXPORT_OST_GEOM CompDivide(const Vec2& v1, const Vec2& v2);

//! vector matrix multiplication
/*!
  This corresponds to

          |a b|
  (x,y) * |c d| = (ax+cy, bx+dy)
*/
Vec2 DLLEXPORT_OST_GEOM operator*(const Vec2& v,const Mat2& m);

//! vector matrix multiplication
/*!
  This corresponds to

   |a b|   (x)   (ax+by)
   |c d| * (y)  =(cx+dy)
*/
Vec2 DLLEXPORT_OST_GEOM operator*(const Mat2& m, const Vec2& v);

//! determinant
Real DLLEXPORT_OST_GEOM Det(const Mat2& m);
//! Transpose
Mat2 DLLEXPORT_OST_GEOM Transpose(const Mat2& m);

//! Matrix inversion
Mat2 DLLEXPORT_OST_GEOM Invert(const Mat2& m);

//! angle beetwen two vectors
Real DLLEXPORT_OST_GEOM Angle(const Vec2& v1, const Vec2& v2);
//! angle beetwen two vectors (honors sign)
Real  DLLEXPORT_OST_GEOM SignedAngle(const Vec2& v1, const Vec2& v2);

//! matrix matrix multiplication
Mat2 DLLEXPORT_OST_GEOM operator*(const Mat2& m1, const Mat2& m2);

//! returns std::min of each component
Vec2 DLLEXPORT_OST_GEOM Min(const Vec2& v1, const Vec2& v2);

//! returns std::max of each component
Vec2 DLLEXPORT_OST_GEOM Max(const Vec2& v1, const Vec2& v2);

//! Rotation
DLLEXPORT Vec2 Rotate(const Vec2& v,Real ang);

} // namespace geom

#endif
