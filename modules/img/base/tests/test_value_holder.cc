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

#include <boost/mpl/vector.hpp>

#define BOOST_TEST_DYN_LINK
#include <boost/test/unit_test.hpp>
using boost::unit_test_framework::test_suite;


#include <ost/img/image_state/value_holder.hh>
#include <ost/img/image_state/index.hh>
#include <ost/img/value_util.hh>

using namespace ost::img;
using namespace ost::img::image_state;

BOOST_AUTO_TEST_SUITE(ost_img_base)


template <typename V>
void value_holder_construct()
{
  ValueHolder<V> v(8,6,10);
  BOOST_CHECK (v.GetWidth()==8);
  BOOST_CHECK (v.GetHeight()==6);
  BOOST_CHECK (v.GetDepth()==10);
  BOOST_CHECK (v.GetVolume()==8*6*10);
}

template <typename V>
void value_holder_readwrite()
{
  int wi=4;
  int he=3;
  int de=5;
  ValueHolder<V> vh(wi,he,de);

  for(int u=0;u<wi;++u) {
    for(int v=0;v<he;++v) {
      for(int w=0;w<de;++w) {
	V val = Random<V>();
	int indx = w+de*(v+he*u);
	vh.Value(Index(u,v,w))=val;
	BOOST_REQUIRE(vh.Value(Index(u,v,w)) == val);
	BOOST_REQUIRE(vh.Value(indx) == val);
      }
    }
  }
}

template <typename V>
void value_holder_copy()
{
  int wi=4;
  int he=3;
  int de=5;

  ValueHolder<V> vh1(wi,he,de);

  for(int u=0;u<wi;++u) {
    for(int v=0;v<he;++v) {
      for(int w=0;w<de;++w) {
        V val = Random<V>();
        vh1.Value(Index(u,v,w))=val;
      }
    }
  }

  ValueHolder<V> vh2(vh1);
  for(int u=0;u<wi;++u) {
    for(int v=0;v<he;++v) {
      for(int w=0;w<de;++w) {
        BOOST_REQUIRE(vh2.Value(Index(u,v,w))==vh1.Value(Index(u,v,w)));
      }
    }
  }
}

BOOST_AUTO_TEST_CASE(value_holder_construct_real)
{
  value_holder_construct<Real>();
}

BOOST_AUTO_TEST_CASE(value_holder_construct_complex)
{
  value_holder_construct<Complex>();
}

BOOST_AUTO_TEST_CASE(value_holder_readwrite_real)
{
  value_holder_readwrite<Real>();
}

BOOST_AUTO_TEST_CASE(value_holder_readwrite_complex)
{
  value_holder_readwrite<Complex>();
}

BOOST_AUTO_TEST_CASE(value_holder_copy_ctor_real)
{
  value_holder_copy<Real>();
}

BOOST_AUTO_TEST_CASE(value_holder_copy_ctor_complex)
{
  value_holder_copy<Complex>();
}

BOOST_AUTO_TEST_SUITE_END()