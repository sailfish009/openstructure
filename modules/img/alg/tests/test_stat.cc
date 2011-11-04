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
#include <ost/img/alg/stat_accumulator.hh>

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
  BOOST_CHECK_CLOSE(stat.GetMean(),Real(5.0),Real(0.0001));
  BOOST_CHECK_CLOSE(stat.GetStandardDeviation(),Real(2.58198889747),Real(0.0001));
  BOOST_CHECK_CLOSE(stat.GetSkewness()+0.5,Real(0.5),Real(0.0001));
  BOOST_CHECK_CLOSE(stat.GetKurtosis(),Real(1.77),Real(0.0001));

  // check for rounding errors
  im+=10000.0;
  im.Apply(stat);
  BOOST_CHECK_CLOSE(stat.GetMean(),Real(10005.0),Real(0.0001));
  BOOST_CHECK_CLOSE(stat.GetStandardDeviation(),Real(2.58198889747),Real(0.01));
  BOOST_CHECK_CLOSE(stat.GetSkewness()+0.5,Real(0.5),Real(0.01));
  BOOST_CHECK_CLOSE(stat.GetKurtosis(),Real(1.77),Real(0.01));


  StatAccumulator<> acc;
  for(int u=0;u<3;++u) {
    for(int v=0;v<3;++v) {
      acc(val[u][v]);
    }
  }
  BOOST_CHECK_CLOSE(acc.GetMean(),Real(5.0),Real(0.0001));
  BOOST_CHECK_CLOSE(acc.GetStandardDeviation(),Real(2.58198889747),Real(0.0001));
  BOOST_CHECK_CLOSE(acc.GetSkewness()+0.5,Real(0.5),Real(0.0001));
  BOOST_CHECK_CLOSE(acc.GetKurtosis(),Real(1.77),Real(0.0001));

  StatAccumulator<> acc1,acc2,acc3;
  for(int u=0;u<3;++u) {
    acc1(val[u][0]);
    acc2(val[u][1]);
    acc3(val[u][2]);
  }
  StatAccumulator<> acc_c=acc1+acc2+acc3;
  BOOST_CHECK_CLOSE(acc_c.GetMean(),Real(5.0),Real(0.0001));
  BOOST_CHECK_CLOSE(acc_c.GetStandardDeviation(),Real(2.58198889747),Real(0.0001));
  BOOST_CHECK_CLOSE(acc_c.GetSkewness()+0.5,Real(0.5),Real(0.0001));
  BOOST_CHECK_CLOSE(acc_c.GetKurtosis(),Real(1.77),Real(0.0001));
}

} // namespace

test_suite* CreateStatTest()
{
  using namespace test_stat;
  test_suite* ts=BOOST_TEST_SUITE("Stat Alg Test");

  ts->add(BOOST_TEST_CASE(&test));

  return ts;
}


