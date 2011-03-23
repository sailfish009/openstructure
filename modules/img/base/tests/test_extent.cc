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
  Author: Ansgar Philippsen
*/

#include <iostream>

#define BOOST_TEST_DYN_LINK
#include <boost/test/unit_test.hpp>
using boost::unit_test_framework::test_suite;

#include <ost/dyn_cast.hh>
#include <ost/img/extent.hh>
#include <ost/img/extent_iterator.hh>
#include <ost/img/image_handle.hh>
#include <ost/img/image_state.hh>
using namespace ost::img;
using namespace ost;

BOOST_AUTO_TEST_SUITE(ost_img_base)

BOOST_AUTO_TEST_CASE(extent_init)
{
  // using explicit start and end points
  Extent e1(Point(-1,-2,-3),Point(3,2,1));
  BOOST_CHECK(e1.GetStart()==Point(-1,-2,-3));
  BOOST_CHECK(e1.GetEnd()==Point(3,2,1));
  BOOST_CHECK(e1.GetSize()==Size(5,5,5));

  // using just size -> implicit origin
  Extent e2(Size(8,10,13));
  BOOST_CHECK(e2.GetStart()==Point(0,0,0));
  BOOST_CHECK(e2.GetEnd()==Point(7,9,12));
  BOOST_CHECK(e2.GetSize()==Size(8,10,13));

  // size and start point
  Extent e3(Point(-3,1,3),Size(5,7,2));
  BOOST_CHECK(e3.GetStart()==Point(-3,1,3));
  BOOST_CHECK(e3.GetEnd()==Point(1,7,4));
  BOOST_CHECK(e3.GetSize()==Size(5,7,2));

  // size and origin point 1
  Extent e4(Size(7,10,6),Point(2,-1,3));
  BOOST_CHECK(e4.GetStart()==Point(-1,-5,1));
  BOOST_CHECK(e4.GetEnd()==Point(5,4,6));
  BOOST_CHECK(e4.GetSize()==Size(7,10,6));

  // size and origin point 2
  Extent e5(Size(7,10,6),Point(0,0,0));
  BOOST_CHECK(e5.GetStart()==Point(-3,-4,-2));
  BOOST_CHECK(e5.GetEnd()==Point(3,5,3));
  BOOST_CHECK(e5.GetSize()==Size(7,10,6));

  // check automatic reordering
  Extent e6(Point(3,-1,5),Point(-2,4,1));
  BOOST_CHECK(e6.GetStart()==Point(-2,-1,1));
  BOOST_CHECK(e6.GetEnd()==Point(3,4,5));

  // start and size
  Extent e7(Point(-2,1,4),Size(4,2,3));
  BOOST_CHECK(e7.GetStart()==Point(-2,1,4));
  BOOST_CHECK(e7.GetEnd()==Point(1,2,6));
  BOOST_CHECK(e7.GetSize()==Size(4,2,3));
}

BOOST_AUTO_TEST_CASE(extent_contains)
{
  Extent e(Point(-1,-2,-3),Point(0,1,2));
  BOOST_CHECK(e.Contains(Point(0,0,0)));
  BOOST_CHECK(e.Contains(Point(-1,-2,-3)));
  BOOST_CHECK(e.Contains(Point(0,1,2)));

  BOOST_CHECK(!e.Contains(Point(-2,0,0)));
  BOOST_CHECK(!e.Contains(Point(0,-3,0)));
  BOOST_CHECK(!e.Contains(Point(0,0,-4)));
  BOOST_CHECK(!e.Contains(Point(1,0,0)));
  BOOST_CHECK(!e.Contains(Point(0,2,0)));
  BOOST_CHECK(!e.Contains(Point(0,0,3)));
  BOOST_CHECK(!e.Contains(Point(-2,-5,10)));
}

BOOST_AUTO_TEST_CASE(extent_wrap_around)
{
  Extent e(Point(-1,-2,-3),Point(1,3,3));
  Point p1(0,0,0);
  Point p1c(0,0,0);
  Point p1w(e.WrapAround(p1));
  Point p2(-3,-4,5);
  Point p2c(0,2,-2);
  Point p2w(e.WrapAround(p2));
  
  std::ostringstream msg;
  msg << p1c << " != " << p1w;
  BOOST_CHECK_MESSAGE(p1c==p1w,msg.str());
  msg.str("");
  msg << p2c << " != " << p2w;
  BOOST_CHECK_MESSAGE(p2c==p2w,msg.str());
}

BOOST_AUTO_TEST_CASE(extent_mirror)
{
  Extent e(Point(-3,-4,2),Point(5,-1,8));
  BOOST_CHECK(e.Mirror(Plane::YZ)==Extent(Point(-5,-4,2),Point(3,-1,8)));
  BOOST_CHECK(e.Mirror(Plane::XZ)==Extent(Point(-3,1,2),Point(5,4,8)));
  BOOST_CHECK(e.Mirror(Plane::XY)==Extent(Point(-3,-4,-8),Point(5,-1,-2)));
}

BOOST_AUTO_TEST_CASE(extent_iterator)
{
  Point p1(-3,4,-1);
  Point p2(-1,7,3);

  Extent e(p1,p2);

  ExtentIterator it(e);
  BOOST_CHECK(it.AtStart());

  it.ToEnd();
  BOOST_CHECK(it.AtEnd());

  it.ToStart();
  BOOST_CHECK(it.AtStart());

  std::ostringstream msg;

  for(int u=p1[0];u<=p2[0];++u) {
    for(int v=p1[1];v<=p2[1];++v) {
      for(int w=p1[2];w<=p2[2];++w) {
        msg.str("");
        msg << it << " != " << Point(u,v,w);
        BOOST_CHECK_MESSAGE( (Point)it == Point(u,v,w),msg.str());
        ++it;
      }
    }
  }
  BOOST_CHECK(it.AtEnd());

  --it.ToEnd();

  for(int u=p2[0];u>=p1[0];--u) {
    for(int v=p2[1];v>=p1[1];--v) {
      for(int w=p2[2];w>=p1[2];--w) {
        msg.str("");
        msg << it << " != " << Point(u,v,w);
        BOOST_CHECK_MESSAGE( (Point)it == Point(u,v,w),msg.str());
        --it;
      }
    }
  }

  BOOST_CHECK(it.AtStart());

  int count=0;
  for(it.ToEnd();!it.AtStart();--it) {
    ++count;
  }
  BOOST_CHECK(count==e.GetVolume());
}

BOOST_AUTO_TEST_CASE(extent_overlap)
{
  Extent e1(Point(-10,5),Point(8,12));
  Extent e2(Point(-1,2),Point(12,7));
  Extent e3(Point(0,0),Point(1,1));

  BOOST_CHECK(Overlap(e1,e2)==Extent(Point(-1,5),Point(8,7)));
  BOOST_CHECK_THROW(Overlap(e1,e3), InvalidExtentException);
}

BOOST_AUTO_TEST_CASE(extent_point2offset)
{
  // check if Point2Offset and ValueHolder use the same convention for 
  // converting a point to a offset into the data array
  
  ImageHandle im=CreateImage(Size(4,4,4));
  int count=0;
  for (size_t i=0; i<4; ++i) {
    for (size_t j=0; j<4; ++j) {
      for (size_t k=0; k<4; ++k) {
        im.SetReal(Point(i,j,k), count);
        count+=1;
      }
    }
  }
  RealSpatialImageStatePtr s=dyn_cast<RealSpatialImageState>(im.ImageStatePtr());
  Real* data=s->Data().GetData();
  count=0;
  Extent ext=im.GetExtent();
  for (size_t i=0; i<4; ++i) {
    for (size_t j=0; j<4; ++j) {
      for (size_t k=0; k<4; ++k) {
        BOOST_CHECK_EQUAL(static_cast<int>(data[ext.Point2Offset(Point(i,j,k))]), 
                          count);
        count+=1;
        
      }
    }
  }
}


BOOST_AUTO_TEST_CASE(extent_has_overlap)
{
  Extent e1(Point(1, 2), Point(2, 3));
  Extent e2(Point(1,2), Point(1,2));  
  BOOST_CHECK(HasOverlap(e1, e1));
  BOOST_CHECK(HasOverlap(e2, e2));

  BOOST_CHECK(HasOverlap(e1, e2));  
  Extent e3(Point(-1,3), Point(2,5));
  Extent e4(Point(0,-5), Point(1,10));
  BOOST_CHECK(HasOverlap(e3, e4));
  BOOST_CHECK(HasOverlap(e4, e3));
  
  Extent e5(Point(-10,-5), Point(-8, -3));
  
  BOOST_CHECK(!HasOverlap(e3, e5));
}


BOOST_AUTO_TEST_SUITE_END()