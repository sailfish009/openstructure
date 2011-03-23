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

#include <ost/img/image_factory.hh>
#include <ost/img/function.hh>
#include <ost/img/image.hh>

using namespace ost::img;

BOOST_AUTO_TEST_SUITE(ost_img_base)


BOOST_AUTO_TEST_CASE(image_factory_generate)
{
  class MyComplexFunc: public ComplexFunction {
  public:
    MyComplexFunc():
      ComplexFunction(FREQUENCY)
    {}

    virtual Complex Func(const Vec3& v) const {
      return Complex(v[0],v[1]);
    }
  };


  MyComplexFunc cf;
  Extent ext(Point(-10,-10),Point(10,10));
  cf.SetExtent(ext);

  ImageHandle g1 = GenerateImage(cf);

  BOOST_CHECK(g1.GetType()==COMPLEX);
  BOOST_CHECK(g1.GetDomain()==FREQUENCY);
  BOOST_CHECK(g1.GetExtent()==ext);
  
  for(ExtentIterator it(ext); !it.AtEnd(); ++it) {
    Point p(it);
    BOOST_REQUIRE(g1.GetComplex(p) == cf.GetComplex(p));
    BOOST_REQUIRE(g1.GetComplex(p) == Complex(Real(p[0]),Real(p[1])));
  }

}

BOOST_AUTO_TEST_SUITE_END()