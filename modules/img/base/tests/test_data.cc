//------------------------------------------------------------------------------
// This file is part of the OpenStructure project <www.openstructure.org>
//
// Copyright (C) 2008-2015 by the OpenStructure authors
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

#include "test_data.hh"

#include <ost/img/data.hh>
#include <ost/img/null_data.hh>
#include <ost/img/data_observer.hh>
#include <ost/img/observable.hh>

using namespace ost::img;

namespace test_data {


void Observer()
{
}

} // namespace 

test_suite* CreateDataTest()
{
  using namespace test_data;
  test_suite* ts=BOOST_TEST_SUITE("Data Test");

  ts->add(BOOST_TEST_CASE(&Observer));

  return ts;
}
