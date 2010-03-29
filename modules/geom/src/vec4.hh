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
#ifndef GEOM_VEC4_H
#define GEOM_VEC4_H

#include <cstddef> // for size_t
#include <ostream>

#include <boost/operators.hpp>

#include <ost/module_config.hh>
#include <ost/config.hh>

namespace geom {

// fw decl
class Vec2;
class Vec3;
class Mat4;

/*
  Four dimensional, homogeneous vector class, using Real precision.
*/
class DLLEXPORT_OST_GEOM Vec4:
    private boost::equality_comparable<Vec4>,
    private boost::additive<Vec4>,
    private boost::additive<Vec4, Real>,
    private boost::multiplicative<Vec4, Real>
{
public:
  //! Default initialization, all components are set to zero
  Vec4();

  //! Initialization with x, y and z component
  Vec4(Real x, Real y, Real z, Real w);

  //! copy ctor
  Vec4(const Vec4& v);

  //! (implicit) initialization with 2D vector
  Vec4(const Vec2& v);

  //! (implicit) initialization with 3D vector
  Vec4(const Vec3& v);

  //! explicit initialization with an array of doubles
  explicit Vec4(const Real[4]);

#if OST_DOUBLE_PRECISION
  //! explicit initialization with an array of floats
  explicit Vec4(const float[4]);
#endif

  //! assignement op
  Vec4& operator=(const Vec4& v);

  //! comparable
  bool operator==(const Vec4& rhs) const;

  //! element access
  Real& operator[](std::size_t indx);
  //! const element access
  const Real& operator[](std::size_t indx) const;

  //! addable op
  Vec4& operator+=(const Vec4& rhs);
  Vec4& operator+=(Real d);
  //! subtractable op
  Vec4& operator-=(const Vec4& rhs);
  Vec4& operator-=(Real d);
  //! negateable
  Vec4 operator-() const;
  //! multipliable
  Vec4& operator*=(Real d);
  //! dividable
  Vec4& operator/=(Real d);

  Real* Data() {return data_;}
  const Real* Data() const {return data_;}

private:
  Real data_[4];

  void set(Real x, Real y, Real z, Real w);
};

DLLEXPORT_OST_GEOM Vec4 operator/(Real, const Vec4& v);

DLLEXPORT_OST_GEOM std::ostream& operator<<(std::ostream&, const Vec4&);

} // namespace geom

# endif
