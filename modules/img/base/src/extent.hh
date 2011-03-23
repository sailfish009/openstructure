//------------------------------------------------------------------------------
// This file is part of the OpenStructure project <www.openstructure.org>
//
// Copyright (C) 2008-2011 by the OpenStructure authors
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
  Extent

  Author: Ansgar Philippsen
*/

#ifndef IMG_EXTENT_H
#define IMG_EXTENT_H

#include <ost/message.hh>
#include <iosfwd>
#include <ost/message.hh>

#include "point.hh"
#include "size.hh"

/*
  TODO:

  add CenterOrigin() -> return extent who's (0,0,0) is in the center
  add TLFOrigin()    -> return extent with start=(0,0,0)
  add operators      -> add/sub with point and extent
*/

namespace ost { namespace img {

struct DLLEXPORT InvalidExtentException: public Error
{
  InvalidExtentException(const String& m= String("") ):
    Error(String("an invalid extent occured ("+m+String(")")))
  {}
};

//! Defines lower and upper valid indices
/*!
  Is part of the basic characteristic of each Data instance.
  Encapsulates a start and end index. Size and Dimension are 
  directly extracted from Extent.
*/
class DLLEXPORT_OST_IMG_BASE Extent {
 public:
  //! default constructor
  Extent() {set(Point(),Point());}

  //! copy constructor
  Extent(const Extent &r) {set(r.start_,r.end_);}

  //! initialize with start and end point
  Extent(const Point& p1, const Point& p2) {set(p1,p2);}

  //! initialize with Size
  /*!
    The extent will go from -(size-1)/2 to (size-1)/2 for odd sizes,
    and from -(size/2-1) to size/2 for even images

    not explicit on purpose, facilitates easier creation code
  */
  Extent(const Size& s) { set(Point(0,0,0), s-Point(1,1,1)); }

  //! initialize with center point and size
  /*
    The given point will be the in the center of the
    extent, with the size according to the even/odd rule
    as described for Extent(const Size& s)
  */
  Extent(const Size& size, const Point& center)
  {
    Point st = -size.GetHalf() + center;
    set(st, size + st - Point(1,1,1));
  }

  //! initialize with Size and start point
  /*
    The extent will go from start to (size-1)+start
  */
  Extent(const Point& start, const Size& size)
  {
    set(start, size + start - Point(1,1,1));
  }

  //! Return lower/left/front corner
  const Point& GetStart() const { return start_; }
  //! Return upper/right/back corner
  const Point& GetEnd() const { return end_; }

  //! Set start point, changing size
  void SetStart(const Point& o)
  {
    set(o,end_);
  }
  //! Set end point, changing size
  void SetEnd(const Point& e)
  {
    set(start_,e);
  }

  //! Returns true if the given point is within extent
  bool Contains(const Point& p) const
  {
    return (p[0]>=start_[0] && p[0]<=end_[0] &&
            p[1]>=start_[1] && p[1]<=end_[1] &&
            p[2]>=start_[2] && p[2]<=end_[2]);
  }
  //! Returns true if the given exten is within extent
  bool Contains(const Extent& e) const
  {
    return Contains(e.GetStart()) && Contains(e.GetEnd());
  }

  //! return center
  Point GetCenter() const
  {
    return size_.GetHalf()+start_;
  }

  //! Return size of extent
  const Size& GetSize() const {return size_;}
  
  int GetWidth() const {return size_.GetWidth();}
  int GetHeight() const {return size_.GetHeight();}
  int GetDepth() const {return size_.GetDepth();}
  int GetVolume() const {return size_.GetVolume();}
  int GetDim() const {return dim_;}

  //! Wrap point around
  /* 
     Ensures that the given point lies within Extent, 
     using wrap around if necessary
   */
  Point WrapAround(const Point& p) const
  {
    Point r(p-start_);
    for(int i=0;i<3;i++) {
      r[i] = start_[i] + (r[i]<0 ? size_[i]+std::div(r[i],size_[i]).rem : std::div(r[i],size_[i]).rem);
    }
    return r;
  }

  //! Return new extent mirrored according to planes
  Extent Mirror(int planes)
  {
    Point new_start(planes & Plane::YZ ? -end_[0] : start_[0],
                    planes & Plane::XZ ? -end_[1] : start_[1],
                    planes & Plane::XY ? -end_[2] : start_[2]);
    return Extent(new_start,size_);
  }

  //! Generates a continues, uniqe index per point contained within extent
  unsigned int Point2Offset(const Point& p) const
  {
    if(this->Contains(p)) {
      Point d(p-start_);
      return d[2]+size_[2]*(d[1]+d[0]*size_[1]);
    } else {
      return 0;
    }
  }

  // operators
  bool operator==(const Extent& b) const {return equal(b);}
  bool operator!=(const Extent& b) const {return !equal(b);}
  
  void Shift(const Point& p)
  {
    set(start_+p,end_+p);
  }
  void AddBorder(int border)
  {
    this->set(start_-img::Point(border,border,border), 
              end_+img::Point(border,border,border));
  }
 private:
  Point start_,end_;
  Size size_;
  int dim_;

  void set(const Point& p1, const Point& p2)
  {
    // order them automatically
    for(int i=0;i<3;i++) {
      if(p1[i]<=p2[i]) {
        start_[i]=p1[i];
        end_[i]=p2[i];
      } else {
        start_[i]=p2[i];
        end_[i]=p1[i];
      }
    }
    size_ = Size(start_,end_);
    dim_ = size_.GetDim();
  }
  bool equal(const Extent& b) const
  {
    return (start_==b.start_ && end_==b.end_);
  }

};

// 'global' functions using extent

DLLEXPORT_OST_IMG_BASE bool HasOverlap(const Extent& e1, const Extent& e2);
DLLEXPORT_OST_IMG_BASE Extent Overlap(const Extent& e1, const Extent& e2);

DLLEXPORT_OST_IMG_BASE std::ostream& operator<<(std::ostream& os, const img::Extent &b);

}} // namespace img


#endif
