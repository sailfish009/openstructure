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
#ifndef GEOM_VEC2_H
#define GEOM_VEC2_H

#include <cstddef> // for size_t
#include <ostream>
#include <vector>
#include <boost/operators.hpp>

#include <ost/geom/module_config.hh>
#include <ost/config.hh>

namespace geom {

// fw decl
class Vec3;
class Vec4;

/*
  Two dimensional vector class, using Real precision.
*/
class DLLEXPORT_OST_GEOM Vec2:
    private boost::equality_comparable<Vec2>,
    private boost::additive<Vec2>,
    private boost::additive<Vec2, Real>,
    private boost::multiplicative<Vec2, Real>
{
public:
  //! Default initialization, all components are set to zero
  Vec2();

  //! Initialization with x, y and z component
  Vec2(Real x, Real y);

  //! copy ctor
  Vec2(const Vec2& v);

  //! explicit initialization with 3D vector
  explicit Vec2(const Vec3& v);

  //! explicit initialization with a 4D homogeneous vector
  explicit Vec2(const Vec4& v);

  //! explicit initialization with an array of doubles
  explicit Vec2(const Real[2]);

#if OST_DOUBLE_PRECISION
  //! explicit initialization with an array of floats
  explicit Vec2(const float[2]);
#endif
  //! assignement op
  Vec2& operator=(const Vec2& v);

  //! element access
  Real& operator[](std::size_t indx);
  //! const element access
  const Real& operator[](std::size_t indx) const;
  //! element access
  Real GetX() const;
  Real GetY() const;
  void SetX(Real v);
  void SetY(Real v);

  //! comparable
  bool operator==(const Vec2& rhs) const;

  //! addable op
  Vec2& operator+=(const Vec2& rhs);
  Vec2& operator+=(Real d);
  //! subtractable op
  Vec2& operator-=(const Vec2& rhs);
  Vec2& operator-=(Real d);
  //! negateable
  Vec2 operator-() const;
  //! multipliable
  Vec2& operator*=(Real d);
  //! dividable
  Vec2& operator/=(Real d);

  Real* Data() {return data_;}
  const Real* Data() const {return data_;}

private:
  Real data_[2];

  void set(Real x, Real y);
};

DLLEXPORT_OST_GEOM Vec2 operator/(Real d, const Vec2& v);

DLLEXPORT_OST_GEOM std::ostream& operator<<(std::ostream&, const Vec2&);

typedef std::vector<Vec2> Vec2List;

} // namespace geom

# endif
