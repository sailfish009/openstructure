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
  Author: Ansgar Philippsen
*/

#include "test_image_factory.hh"

#include <ost/iplt/image_factory.hh>
#include <ost/iplt/function.hh>
#include <ost/iplt/image.hh>

using namespace ost::iplt;

namespace test_image_factory {

void test_Generate()
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

} // namespace 

test_suite* CreateImageFactoryTest()
{
  using namespace test_image_factory;
  test_suite* ts=BOOST_TEST_SUITE("ImageFactory Test");

  ts->add(BOOST_TEST_CASE(&test_Generate));

  return ts;
}
