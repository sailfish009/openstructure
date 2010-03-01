//------------------------------------------------------------------------------
// This file is part of the OpenStructure project <www.openstructure.org>
//
// Copyright (C) 2008-2010 by the OpenStructure authors
// Copyright (C) 2003-2010 by the IPLT authors
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

/*
  integer triplet based point

  Author: Ansgar Philippsen
*/

/*
  TODO: use boost operator
*/

#ifndef IMG_POINT_H
#define IMG_POINT_H

#include <vector>
#include <iosfwd>

#include <ost/img/module_config.hh>
#include "vecmat.hh"

namespace ost { namespace img {

//fw decl
class Size;

//! class encapsulating 1D to 3D point
class DLLEXPORT_OST_IMG_BASE Point {
public:
  Point();
  Point(const Point &p);

  //! 1D int constructor
  explicit Point(int a);
  //! 2D int constructor
  Point(int a, int b);
  //! 3D int constructor
  Point(int a, int b, int c);
  //! conversion from Vec2
  explicit Point(const Vec2& v);

  //! conversion from Vec3
  explicit Point(const Vec3& v);

  //! conversion from Vec4 (normalization)
  explicit Point(const Vec4& v);

  //! (implicit) conversion of size to point
  Point(const Size& s);

  //! return mirror point according to planes
  Point Mirror(int planes);

  // operators
  int operator[](unsigned int index) const;

  int& operator[](unsigned int index);

  Point& operator=(const Point& p);

  Point& operator+=(const Point& p);

  Point& operator-=(const Point& p);

  Point operator-() const;

  bool operator==(const Point &p) const;

  bool operator!=(const Point &p) const;

  bool operator<(const Point &p) const;

  bool operator<=(const Point &p) const;

  bool operator>(const Point &p) const;

  bool operator>=(const Point &p) const;

  Point& operator+=(const Size& p);

  Point& operator-=(const Size& p);

  // conversion to vectors
  Vec2 ToVec2() const;
  Vec3 ToVec3() const;
  Vec4 ToVec4() const;

private:
  int data_[3];

  bool equal(const Point &p) const;
  bool less(const Point &p) const;
  Point neg() const;

  friend Point absolute(const Point&);

  Point absolute() const;
};

DLLEXPORT_OST_IMG_BASE Point operator+(const Point& p1, const Point& p2);
DLLEXPORT_OST_IMG_BASE Point operator-(const Point& p1, const Point& p2);

  /*
Point operator+(const Point& p, const Size& s);
Point operator+(const Size& s, const Point& p);

Point operator-(const Point& p, const Size& s);
Point operator-(const Size& s, const Point& p);
  */

DLLEXPORT_OST_IMG_BASE std::ostream& operator<<(std::ostream& os, const Point &p);


}} // namespace img


#endif



