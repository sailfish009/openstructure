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
#ifndef GEOM_VEC3_H
#define GEOM_VEC3_H

#include <cstddef> // for size_t
#include <ostream>

#include <boost/operators.hpp>

#include <ost/geom/module_config.hh>
#include <ost/config.hh>

namespace geom {

// fw decl
class Vec2;
class Vec4;


/// \brief Three dimensional vector class, using Real precision.
class DLLEXPORT_OST_GEOM Vec3:
    private boost::equality_comparable<Vec3>,
    private boost::additive<Vec3>,
    private boost::additive<Vec3, Real>,
    private boost::multiplicative<Vec3, Real>
{
public:
  //! Default initialization, all components are set to zero
  Vec3();

  //! Initialization with x, y and z component
  Vec3(Real x, Real y, Real z);

  //! copy ctor
  Vec3(const Vec3& v);

  //! (implicit) initialization with 2D vector
  Vec3(const Vec2& v);

  //! explicit initialization with a 4D homogeneous vector
  /*!
    a 4D (homogeneous) vector can be explicitely converted
    to a 3D vector, resulting in (x/w,y/w,z/w)
  */
  explicit Vec3(const Vec4& v);

  //! explicit initialization with an array of doubles
  explicit Vec3(const Real[3]);

#if OST_DOUBLE_PRECISION
  //! explicit initialization with an array of floats
  explicit Vec3(const float[3]);
#endif
  //! assignement op
  Vec3& operator=(const Vec3& v);

  //! comparable
  bool operator==(const Vec3& rhs) const;

  //! element access
  Real& operator[](std::size_t indx);
  //! const element access
  const Real& operator[](std::size_t indx) const;
  //! element access
  Real GetX() const;
  Real GetY() const;
  Real GetZ() const;
  void SetX(Real v);
  void SetY(Real v);
  void SetZ(Real v);

  //! addable op
  Vec3& operator+=(const Vec3& rhs);
  Vec3& operator+=(Real d);
  //! subtractable op
  Vec3& operator-=(const Vec3& rhs);
  Vec3& operator-=(Real d);
  //! negateable
  Vec3 operator-() const;
  //! multipliable
  Vec3& operator*=(Real d);
  //! dividable
  Vec3& operator/=(Real d);

  Real* Data() {return data_;}
  const Real* Data() const {return data_;}

private:
  Real data_[3];

  void set(Real x, Real y, Real z);
};

DLLEXPORT_OST_GEOM Vec3 operator/(Real, const Vec3& v);

DLLEXPORT_OST_GEOM std::ostream& operator<<(std::ostream&, const Vec3&);

} // namespace geom


# endif
