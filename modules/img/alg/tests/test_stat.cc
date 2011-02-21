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

#include <ost/img/image.hh>

#include <ost/img/alg/stat.hh>

namespace test_stat {

using namespace ost::img;
using namespace ost::img::alg;

void test() {
  Real val[3][3] = {
    {3.0,1.0,5.0},{6.0,9.0,2.0},{4.0,7.0,8.0}
  };
  ImageHandle im = CreateImage(Extent(Point(0,0),Size(3,3)));

  for(int u=0;u<3;++u) {
    for(int v=0;v<3;++v) {
      im.SetReal(Point(u,v),val[u][v]);
    }
  }

  Stat stat;
  im.Apply(stat);

  std::ostringstream msg;

  msg << "expected 5.0 as mean but found " << stat.GetMean();
  BOOST_CHECK_MESSAGE(stat.GetMean()==5.0,msg.str());
  msg.str("");
  msg << "expected 2.73861 as stdev but found " << stat.GetStandardDeviation();
  BOOST_CHECK_MESSAGE((stat.GetStandardDeviation()-2.73861)<1e-5,msg.str());
  
}

} // namespace

test_suite* CreateStatTest()
{
  using namespace test_stat;
  test_suite* ts=BOOST_TEST_SUITE("Stat Alg Test");

  ts->add(BOOST_TEST_CASE(&test));

  return ts;
}


