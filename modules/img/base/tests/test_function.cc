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

#include "test_function.hh"

#include <ost/img/function.hh>
#include <ost/img/null_function.hh>
using namespace ost::img;

namespace test_function {

void test_NullFunc()
{
  NullFunction nf;

  BOOST_CHECK(nf.GetDomain()==SPATIAL);
  BOOST_CHECK(nf.GetReal(Point(-1,-4,-7))==0.0);
}

void test_RealFunc()
{
  class MyRealFunc: public RealFunction {
  public:
    MyRealFunc():
      RealFunction(SPATIAL)
    {}

    Real Func(const Vec3& v) const {return 1.5;}
  };

  MyRealFunc f;

  BOOST_CHECK(f.GetReal(Point())==1.5);
}

void test_ComplexFunc()
{
  class MyComplexFunc: public ComplexFunction {
  public:
    MyComplexFunc():
      ComplexFunction(SPATIAL)
    {}

    Complex Func(const Vec3& v) const {return Complex(-1.5,8.0);}
  };

  MyComplexFunc f;

  BOOST_CHECK(f.GetComplex(Point())==Complex(-1.5,8.0));
}

} // namespace 

test_suite* CreateFunctionTest()
{
  using namespace test_function;
  test_suite* ts=BOOST_TEST_SUITE("Function Test");

  ts->add(BOOST_TEST_CASE(&test_NullFunc));
  ts->add(BOOST_TEST_CASE(&test_RealFunc));
  ts->add(BOOST_TEST_CASE(&test_ComplexFunc));

  return ts;
}
