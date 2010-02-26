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

namespace geom {

class Vec3;
class Mat3;

//! returns length of vector
Real DLLEXPORT_OST_GEOM Length(const Vec3& v);

//! returns squared length of vector
Real DLLEXPORT_OST_GEOM Length2(const Vec3& v);

//! return true if components differ not more than ephilon
bool DLLEXPORT_OST_GEOM Equal(const Vec3& v1, const Vec3& v2, Real ephilon=EPSILON);

//! return true if components differ not more than ephilon
bool DLLEXPORT_OST_GEOM Equal(const Mat3& m1, const Mat3& m2, Real ephilon=EPSILON);

//! vector dot product
Real DLLEXPORT_OST_GEOM Dot(const Vec3& v1, const Vec3& v2);

//! Normalize
Vec3 DLLEXPORT_OST_GEOM Normalize(const Vec3& v);

//! vector cross product
Vec3 DLLEXPORT_OST_GEOM Cross(const Vec3& v1, const Vec3& v2);

//! multiply each component of v1 with that of v2
Vec3 DLLEXPORT_OST_GEOM CompMultiply(const Vec3& v1, const Vec3& v2);

//! divide each component of v1 by that of v2
Vec3 DLLEXPORT_OST_GEOM CompDivide(const Vec3& v1, const Vec3& v2);

//! vector matrix multiplication
Vec3 DLLEXPORT_OST_GEOM operator*(const Vec3& v,const Mat3& m);
//! vector matrix multiplication
Vec3 DLLEXPORT_OST_GEOM operator*(const Mat3& m, const Vec3& v);

//! matrix matrix multiplication
Mat3 DLLEXPORT_OST_GEOM operator*(const Mat3& m1, const Mat3& m2);

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

//! returns std::min of each component
Vec3 DLLEXPORT_OST_GEOM Min(const Vec3& v1, const Vec3& v2);

//! returns std::max of each component
Vec3 DLLEXPORT_OST_GEOM Max(const Vec3& v1, const Vec3& v2);

//! returns the distance between two points
Real DLLEXPORT_OST_GEOM Distance(const Vec3& p1, const Vec3& p2);

} // ns

#endif
