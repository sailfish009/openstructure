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
#ifndef GEOM_VECMAT4_OP_HH
#define GEOM_VECMAT4_OP_HH

#include "constants.hh"

#include <ost/geom/module_config.hh>

namespace geom {

class Vec4;
class Mat4;

//! returns length of vector
Real DLLEXPORT_OST_GEOM Length(const Vec4& v);

//! returns squared length of vector
Real DLLEXPORT_OST_GEOM Length2(const Vec4& v);

//! return true if components differ not more than ephilon
bool DLLEXPORT_OST_GEOM Equal(const Vec4& v1, const Vec4& v2, Real ephilon=EPSILON);

//! return true if components differ not more than ephilon
bool DLLEXPORT_OST_GEOM Equal(const Mat4& m1, const Mat4& v2, Real ephilon=EPSILON);

//! vector dot product
Real DLLEXPORT_OST_GEOM Dot(const Vec4& v1, const Vec4& v2);

Vec4 DLLEXPORT_OST_GEOM Normalize(const Vec4& v);

//! multiply each component of v1 with that of v2
Vec4 DLLEXPORT_OST_GEOM CompMultiply(const Vec4& v1, const Vec4& v2);

//! divide each component of v1 by that of v2
Vec4 DLLEXPORT_OST_GEOM CompDivide(const Vec4& v1, const Vec4& v2);

//! vector matrix multiplication
Vec4 DLLEXPORT_OST_GEOM operator*(const Vec4& v,const Mat4& m);
//! vector matrix multiplication
Vec4 DLLEXPORT_OST_GEOM operator*(const Mat4& m, const Vec4& v);

// algebraic complement
Real DLLEXPORT_OST_GEOM Comp(const Mat4& m, unsigned int i, unsigned int j);

// minor
Real DLLEXPORT_OST_GEOM Minor(const Mat4& m, unsigned int i, unsigned int j);

Real DLLEXPORT_OST_GEOM Det(const Mat4& m);
Mat4 DLLEXPORT_OST_GEOM Transpose(const Mat4& m);
Mat4 DLLEXPORT_OST_GEOM Invert(const Mat4& m);
Mat4 DLLEXPORT_OST_GEOM operator*(const Mat4& m1, const Mat4& m2);
Real DLLEXPORT_OST_GEOM Angle(const Vec4& v1, const Vec4& v2);

//! returns std::min of each component
Vec4 DLLEXPORT_OST_GEOM Min(const Vec4& v1, const Vec4& v2);

//! returns std::max of each component
Vec4 DLLEXPORT_OST_GEOM Max(const Vec4& v1, const Vec4& v2);

} // ns

#endif
