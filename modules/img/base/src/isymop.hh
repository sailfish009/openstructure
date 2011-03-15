//------------------------------------------------------------------------------
// This file is part of the OpenStructure project <www.openstructure.org>
//
// Copyright (C) 2008-2011 by the OpenStructure authors
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
#ifndef OST_IMG_BASE_ISYMOP_HH
#define OST_IMG_BASE_ISYMOP_HH

#include <cassert>
#include <ost/symop.hh>
#include "size.hh"
#include "point.hh"

namespace ost { namespace img {

class DLLEXPORT Mat3i {
public:
  //! Default initialization, identity matrix  
  Mat3i()
  {
    this->set(1,0,0, 0,1,0, 0,0,1);
  }
  //! In with 9 values in row-major order
  /*!
    row-major order means that the matrix

    | a b c |
    | d e f |
    | g h i |

    is initialized with (a,b,c, d,e,f, g,h,i)
  */
  Mat3i(int i00, int i01, int i02,
        int i10, int i11, int i12,
        int i20, int i21, int i22)
  {
    this->set(i00,i01,i02,i10,i11,i12,i20,i21,i22);
  }

  Mat3i(const Mat3i& m)
  {
    this->set(m(0,0),m(0,1),m(0,2),m(1,0),m(1,1),m(1,2),m(2,0),m(2,1),m(2,2));
  }
  //! element access
  int& operator()(std::size_t r, std::size_t c)
  {
    assert(r<=2 && c<=2);
    return data_[r][c];
  }
  //! const element access
  const int& operator()(std::size_t r, std::size_t c) const
  {
    assert(r<=2 && c<=2);
    return data_[r][c];
  }

  Mat3i& operator=(const Mat3i& m)
  {
    if(&m!=this) {
      this->set(m(0,0),m(0,1),m(0,2),m(1,0),m(1,1),m(1,2),m(2,0),m(2,1),m(2,2));
    }
    return *this;
  }

  static Mat3i Identity() 
  {
    static Mat3i i(1, 0, 0, 0, 1, 0,0, 0, 1);
    return i;
  }

  bool operator==(const Mat3i& rhs) const
  {
    return data_[0][0] ==  rhs.data_[0][0] &&
      data_[1][0] == rhs.data_[1][0] &&
      data_[2][0] == rhs.data_[2][0] &&
      data_[0][1] == rhs.data_[0][1] &&
      data_[1][1] ==  rhs.data_[1][1] &&
      data_[2][1] ==  rhs.data_[2][1] &&
      data_[0][2] ==  rhs.data_[0][2] &&
      data_[1][2] ==  rhs.data_[1][2] &&
      data_[2][2] ==  rhs.data_[2][2];
  }

  Mat3i& operator+=(const Mat3i& rhs)
  {
    data_[0][0]+=rhs(0,0);
    data_[0][1]+=rhs(0,1);
    data_[0][2]+=rhs(0,2);
    data_[1][0]+=rhs(1,0);
    data_[1][1]+=rhs(1,1);
    data_[1][2]+=rhs(1,2);
    data_[2][0]+=rhs(2,0);
    data_[2][1]+=rhs(2,1);
    data_[2][2]+=rhs(2,2);
    return *this;
  }
  Mat3i& operator-=(const Mat3i& rhs)
  {
    data_[0][0]-=rhs(0,0);
    data_[0][1]-=rhs(0,1);
    data_[0][2]-=rhs(0,2);
    data_[1][0]-=rhs(1,0);
    data_[1][1]-=rhs(1,1);
    data_[1][2]-=rhs(1,2);
    data_[2][0]-=rhs(2,0);
    data_[2][1]-=rhs(2,1);
    data_[2][2]-=rhs(2,2);
    return *this;
  }
  Mat3i& operator*=(const int d)
  {
    data_[0][0]*=d;
    data_[0][1]*=d;
    data_[0][2]*=d;
    data_[1][0]*=d;
    data_[1][1]*=d;
    data_[1][2]*=d;
    data_[2][0]*=d;
    data_[2][1]*=d;
    data_[2][2]*=d;
    return *this;
  }
  Mat3i& operator/=(const int d)
  {
    data_[0][0]/=d;
    data_[0][1]/=d;
    data_[0][2]/=d;
    data_[1][0]/=d;
    data_[1][1]/=d;
    data_[1][2]/=d;
    data_[2][0]/=d;
    data_[2][1]/=d;
    data_[2][2]/=d;
    return *this;
  }

  Mat3i& operator*=(const Mat3i& m)
  {
    (*this)=Mat3i((*this)(0,0)*m(0,0)+(*this)(0,1)*m(1,0)+(*this)(0,2)*m(2,0),
                 (*this)(0,0)*m(0,1)+(*this)(0,1)*m(1,1)+(*this)(0,2)*m(2,1),
                 (*this)(0,0)*m(0,2)+(*this)(0,1)*m(1,2)+(*this)(0,2)*m(2,2),
                 (*this)(1,0)*m(0,0)+(*this)(1,1)*m(1,0)+(*this)(1,2)*m(2,0),
                 (*this)(1,0)*m(0,1)+(*this)(1,1)*m(1,1)+(*this)(1,2)*m(2,1),
                 (*this)(1,0)*m(0,2)+(*this)(1,1)*m(1,2)+(*this)(1,2)*m(2,2),                                   
                 (*this)(2,0)*m(0,0)+(*this)(2,1)*m(1,0)+(*this)(2,2)*m(2,0),
                 (*this)(2,0)*m(0,1)+(*this)(2,1)*m(1,1)+(*this)(2,2)*m(2,1),
                 (*this)(2,0)*m(0,2)+(*this)(2,1)*m(1,2)+(*this)(2,2)*m(2,2));
    return *this;
  }

  int* Data() {return &data_[0][0];}
  const int* Data() const {return &data_[0][0];}
  
private:
  int data_[3][3];

  void set(int i00, int i01, int i02,
           int i10, int i11, int i12,
           int i20, int i21, int i22)
  {
    data_[0][0]=i00; data_[0][1]=i01; data_[0][2]=i02;
    data_[1][0]=i10; data_[1][1]=i11; data_[1][2]=i12;
    data_[2][0]=i20; data_[2][1]=i21; data_[2][2]=i22;
  }
};
/// \brief an integral version of a symmetry operation
///
/// These symmetry operations are used to transform points
///
/// FIXME: This class should probably also live in the base or geom module, but 
///        for now it has to be here because of the interoperation with Point and 
///        Size
class DLLEXPORT_OST_IMG_BASE ISymop {
public:
  /// \brief create an integral version of the symmetry operation
  ISymop(const Symop& symop, const Size& size);
  
  const Mat3i& Rot() const { return rot_; }
  
  const Point& Trans() const { return trans_; }
  
  
  bool operator==(const ISymop& rhs) const
  {
    return rot_==rhs.rot_ && trans_==rhs.trans_;
  }
  
  bool operator!=(const ISymop& rhs) const
  {
    return !this->operator==(rhs);
  }
  Point Transform(const Point& p) const
  {
    return Point(p[0]*rot_(0,0)+p[1]*rot_(0,1)+p[2]*rot_(0,2)+trans_[0],
                 p[0]*rot_(1,0)+p[1]*rot_(1,1)+p[2]*rot_(1,2)+trans_[1],
                 p[0]*rot_(2,0)+p[1]*rot_(2,1)+p[2]*rot_(2,2)+trans_[2]);
  }
private:
  Mat3i  rot_;
  Point  trans_;
};

}}

#endif
