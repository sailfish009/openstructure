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

#include "tests.hh"

#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE ost_img_alg
#include <boost/test/unit_test.hpp>

#include <ost/img/image.hh>
#include <ost/img/alg/randomize.hh>
#include <ost/img/alg/clear.hh>



using namespace ost::img;
using namespace ost::img::alg;

BOOST_AUTO_TEST_SUITE(ost_img_alg)

BOOST_AUTO_TEST_CASE(alg_clear)
{
  alg::Randomize rnd;
  alg::Clear clear;
  ImageHandle i=CreateImage(Size(10));
  i.ApplyIP(rnd);
  for(ExtentIterator it(i.GetExtent());!it.AtEnd();++it) {
    BOOST_REQUIRE(i.GetReal(it)>0.0);
  }
  i.ApplyIP(clear);
  for(ExtentIterator it(i.GetExtent());!it.AtEnd();++it) {
    BOOST_REQUIRE(i.GetReal(it)==0.0);
  }
}


BOOST_AUTO_TEST_SUITE_END()