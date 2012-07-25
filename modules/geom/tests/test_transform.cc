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
/*
  Authors: Tobias Schmidt
 */
 #define BOOST_TEST_DYN_LINK
#include <boost/test/unit_test.hpp>
#include <boost/test/auto_unit_test.hpp>
#include <ost/geom/geom.hh>

using namespace geom;

BOOST_AUTO_TEST_SUITE( geom_base );

BOOST_AUTO_TEST_CASE(test_transform)
{
  Transform tf;

  BOOST_CHECK_EQUAL(tf.GetMatrix(), geom::Mat4());
  BOOST_CHECK_EQUAL(tf.GetRot(), geom::Mat3());
  BOOST_CHECK_EQUAL(tf.GetTrans(), geom::Vec3());
  BOOST_CHECK_EQUAL(tf.GetCenter(), geom::Vec3());

  geom::Mat4 mat = geom::Mat4(1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16);
  tf.SetMatrix(mat);
  BOOST_CHECK_EQUAL(tf.GetMatrix(), mat);
  BOOST_CHECK_EQUAL(tf.GetRot(), geom::Mat3(1,2,3,5,6,7,9,10,11));
  BOOST_CHECK_EQUAL(tf.GetCenter(), geom::Vec3(4,8,12));
  BOOST_CHECK_EQUAL(tf.GetTrans(), geom::Vec3(13,14,15));
}

BOOST_AUTO_TEST_SUITE_END();
