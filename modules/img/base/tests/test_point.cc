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

#define BOOST_TEST_DYN_LINK
#include <boost/test/unit_test.hpp>
using boost::unit_test_framework::test_suite;

#include <ost/img/point.hh>
using namespace ost::img;

BOOST_AUTO_TEST_SUITE(ost_img_base)

BOOST_AUTO_TEST_CASE(init_point)
{
  // default
  Point p0;
  BOOST_CHECK(p0[0]==0);
  BOOST_CHECK(p0[1]==0);
  BOOST_CHECK(p0[2]==0);

  // 1D
  Point p1(3);
  BOOST_CHECK(p1[0]==3);
  BOOST_CHECK(p1[1]==0);
  BOOST_CHECK(p1[2]==0);

  // 2D
  Point p2(-2,8);
  BOOST_CHECK(p2[0]==-2);
  BOOST_CHECK(p2[1]==8);
  BOOST_CHECK(p2[2]==0);

  // 3D
  Point p3(4,-1,5);
  BOOST_CHECK(p3[0]==4);
  BOOST_CHECK(p3[1]==-1);
  BOOST_CHECK(p3[2]==5);
  
  Point p4(geom::Vec2(4.0,5.0));
  BOOST_CHECK(p4[0]==4);
  BOOST_CHECK(p4[1]==5);
  BOOST_CHECK(p4[2]==0);
}

BOOST_AUTO_TEST_SUITE_END()