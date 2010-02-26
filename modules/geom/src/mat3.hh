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
#ifndef GEOM_MAT3_HH
#define GEOM_MAT3_HH

#include <cstddef> // for size_t
#include <ostream>

#include <boost/operators.hpp>

#include <ost/geom/module_config.hh>

namespace geom {

class Mat2;
class Vec3;

class DLLEXPORT_OST_GEOM Mat3:
    private boost::equality_comparable<Mat3>,
    private boost::additive1<Mat3>,
    private boost::multiplicative2<Mat3, Real>
{
public:
  static Mat3 Identity();
  
  //! Default initialization, identity matrix
  Mat3();

  //! In with 9 values in row-major order
  /*!
    row-major order means that the matrix

    | a b c |
    | d e f |
    | g h i |

    is initialized with (a,b,c, d,e,f, g,h,i)
  */
  Mat3(Real i00, Real i01, Real i02,Real i10, Real i11, Real i12,Real i20, Real i21, Real i22);

  //! Copy ctor
  Mat3(const Mat3& m);

  // initialization with 2d matrix
  explicit Mat3(const Mat2& m);

  //! initialization from array
  explicit Mat3(const Real[9]);

  //! assignement op
  Mat3& operator=(const Mat3& m);

  //! comparable
  bool operator==(const Mat3& rhs) const;

  //! element access
  Real& operator()(std::size_t r, std::size_t c);
  //! const element access
  const Real& operator()(std::size_t r, std::size_t c) const;

  //! addable op
  Mat3& operator+=(const Mat3& rhs);
  //! subtractable op
  Mat3& operator-=(const Mat3& rhs);

  Mat3& operator*=(const Real d);
  Mat3& operator/=(const Real d);

  Mat3& operator*=(const Mat3& m);

  Real* Data() {return &data_[0][0];}
  const Real* Data() const {return &data_[0][0];}
  
  geom::Vec3 GetCol(int index) const;
  geom::Vec3 GetRow(int index) const;
private:
  Real data_[3][3];

  void set(Real i00, Real i01, Real i02,Real i10, Real i11, Real i12,Real i20, Real i21, Real i22);
};

DLLEXPORT_OST_GEOM std::ostream& operator<<(std::ostream& o, const Mat3& m);

} // ns geom


#endif
