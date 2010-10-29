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
#include <ost/mol/mol.hh>
#include <ost/conop/builder.hh>
#define BOOST_TEST_DYN_LINK
#include <boost/test/unit_test.hpp>


// using boost::unit_test_framework::test_suite;
using namespace ost;
using namespace ost::conop;
using namespace ost::mol;

BOOST_AUTO_TEST_SUITE( conop )


BOOST_AUTO_TEST_CASE( test_builder )
{
  BOOST_CHECK_EQUAL(Builder::GuessAtomElement("CA", false), "C");
  BOOST_CHECK_EQUAL(Builder::GuessAtomElement("CB", false), "C");
  BOOST_CHECK_EQUAL(Builder::GuessAtomElement("N", false), "N");
  BOOST_CHECK_EQUAL(Builder::GuessAtomElement("O", false), "O");
  BOOST_CHECK_EQUAL(Builder::GuessAtomElement("CG1", false), "C");
  BOOST_CHECK_EQUAL(Builder::GuessAtomElement("CG2", false), "C");
  BOOST_CHECK_EQUAL(Builder::GuessAtomElement("OG1", false), "O");
  BOOST_CHECK_EQUAL(Builder::GuessAtomElement("SG", false), "S");
  BOOST_CHECK_EQUAL(Builder::GuessAtomElement("1HA", false), "H");
  BOOST_CHECK_EQUAL(Builder::GuessAtomElement("1HB", false), "H");
  BOOST_CHECK_EQUAL(Builder::GuessAtomElement("1DA", false), "D");
  BOOST_CHECK_EQUAL(Builder::GuessAtomElement("1DB", false), "D");
  BOOST_CHECK_EQUAL(Builder::GuessAtomElement("1HA", true), "H");
  BOOST_CHECK_EQUAL(Builder::GuessAtomElement("1HB", true), "H");
  BOOST_CHECK_EQUAL(Builder::GuessAtomElement("1DA", true), "D");
  BOOST_CHECK_EQUAL(Builder::GuessAtomElement("1DB", true), "D");  
  BOOST_CHECK_EQUAL(Builder::GuessAtomElement("CA", true), "CA");
  BOOST_CHECK_EQUAL(Builder::GuessAtomElement("11", true), "");
  BOOST_CHECK_EQUAL(Builder::GuessAtomElement("11", false), "");  
}

BOOST_AUTO_TEST_SUITE_END()
