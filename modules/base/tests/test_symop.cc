//------------------------------------------------------------------------------
// This file is part of the OpenStructure project <www.openstructure.org>
//
// Copyright (C) 2008-2010 by the OpenStructure authors
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

#include <ost/symop.hh>
#define BOOST_TEST_DYN_LINK
#include <boost/test/unit_test.hpp>

using namespace ost;

BOOST_AUTO_TEST_SUITE(base)

BOOST_AUTO_TEST_CASE(test_parse)
{
  BOOST_CHECK_NO_THROW(Symop::FromString("x,y,z"));
  BOOST_CHECK_NO_THROW(Symop::FromString("y,z,x"));
  BOOST_CHECK_NO_THROW(Symop::FromString("z,y,x"));
  BOOST_CHECK_NO_THROW(Symop::FromString("y,x,z"));
  BOOST_CHECK_NO_THROW(Symop::FromString("X,Y,Z"));

  BOOST_CHECK_NO_THROW(Symop::FromString("X+1/2,Y-1/2,Z+3/9"));

  BOOST_CHECK_THROW(Symop::FromString("x,y"), SymopParseError);
  BOOST_CHECK_THROW(Symop::FromString("x"), SymopParseError);
  BOOST_CHECK_THROW(Symop::FromString("x,y,z,a"), SymopParseError);

  BOOST_CHECK_THROW(Symop::FromString("a,b,c"), SymopParseError)
}

BOOST_AUTO_TEST_CASE(symop_rotation)
{
  Symop op1=Symop::FromString("x,y,z");
  BOOST_CHECK_EQUAL(op1.Rot(),geom::Mat3());
  BOOST_CHECK_EQUAL(op1.Trans(), geom::Vec3());
  
  Symop op2=Symop::FromString("-x,y,-z");
  BOOST_CHECK_EQUAL(op2.Rot(),geom::Mat3(-1,0,0, 0,1,0, 0,0,-1));
  BOOST_CHECK_EQUAL(op2.Trans(), geom::Vec3());
}

BOOST_AUTO_TEST_CASE(symop_translation)
{
  Symop op1=Symop::FromString("x+1/2,y+1/4,z-1/8");
  BOOST_CHECK_EQUAL(op1.Rot(),geom::Mat3());
  BOOST_CHECK_EQUAL(op1.Trans(), geom::Vec3(0.5,0.25,-0.125));
}

BOOST_AUTO_TEST_SUITE_END()
