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

#include <sstream>

#include <boost/mpl/vector.hpp>

#define BOOST_TEST_DYN_LINK
#include <boost/test/unit_test.hpp>
using boost::unit_test_framework::test_suite;

#include <ost/img/extent.hh>
#include <ost/img/extent_iterator.hh>
#include <ost/img/size.hh>
#include <ost/img/image_state/image_state_impl.hh>
#include <ost/img/image_state/image_state_factory.hh>
#include <ost/img/image_state/image_state_def.hh>

#include <ost/img/alg/randomize.hh>

using namespace ost::img;
using namespace ost::img::image_state;

alg::Randomize randomizer;

bool is_clear(const ImageStateBase& b)
{
  for(ExtentIterator it(b.GetExtent()); !it.AtEnd(); ++it) {
    if(b.GetReal(it) != 0.0) return false;
  }
  return true;
}

BOOST_AUTO_TEST_SUITE(ost_img_base)


BOOST_AUTO_TEST_CASE(point_to_coord)
{
  RealSpatialImageState spatial(Extent(Size(10,10,10)),
                                PixelSampling(Vec3(2.0,2.0,2.0), SPATIAL, 
                                              Extent(Size(10,10, 10))));
  ComplexFrequencyImageState frequency(Extent(Size(10,10,10)),
                                       PixelSampling(Vec3(2.0,2.0,2.0), FREQUENCY, 
                                                     Extent(Size(10,10,10))));
  spatial.SetAbsoluteOrigin(Vec3(5.0,5.0,5.0));
  frequency.SetAbsoluteOrigin(Vec3(5.0,5.0,5.0));
  BOOST_CHECK(spatial.IndexToCoord(Point(2.0,2.0,2.0)) == Vec3(9.0,9.0,9.0));
  BOOST_CHECK(frequency.IndexToCoord(Point(2.0,2.0,2.0)) == Vec3(0.1,0.1,0.1));
  BOOST_CHECK(frequency.CoordToIndex(Vec3(0.2,0.2,0.2)) == Vec3(4.0,4.0,4.0));
  BOOST_CHECK(spatial.CoordToIndex(Vec3(16.0,16.0,16.0)) == Vec3(5.5,5.5,5.5));
}

BOOST_AUTO_TEST_CASE(real_spatial_init)
{

  Extent ext(Size(8,10,13));
  RealSpatialImageState st(ext,PixelSampling());
  
  BOOST_CHECK(st.GetSize()==Size(8,10,13));
  
  BOOST_CHECK(is_clear(st));
}

BOOST_AUTO_TEST_CASE(complex_half_freq_init_1d)
{
  std::ostringstream ostr;
  ComplexHalfFrequencyImageState st1( Extent(Size(15)),PixelSampling() );
  ostr.str("");
  ostr << "expected " << Extent(Point(0),Point(7)) << " but got " << st1.GetExtent();
  BOOST_CHECK_MESSAGE(st1.GetExtent()==Extent(Point(0),Point(7)),ostr.str());
  ostr.str("");
  ostr << "expected " << Extent(Point(-7),Point(7)) << " but got " << st1.GetLogicalExtent();
  BOOST_CHECK_MESSAGE(st1.GetLogicalExtent()==Extent(Point(-7),Point(7)),ostr.str());
  BOOST_CHECK(is_clear(st1));
}

BOOST_AUTO_TEST_CASE(complex_half_freq_init_2d)
{
  std::ostringstream ostr;
  ComplexHalfFrequencyImageState st2( Extent(Size(8,10)),PixelSampling() );
  ostr.str("");
  ostr << "expected " << Extent(Point(-3,0),Point(4,5)) << " but got " << st2.GetExtent();
  BOOST_CHECK_MESSAGE(st2.GetExtent()==Extent(Point(-3,0),Point(4,5)),ostr.str());
  ostr.str("");
  ostr << "expected " << Extent(Point(-3,-4),Point(4,5)) << " but got " << st2.GetLogicalExtent();
  BOOST_CHECK_MESSAGE(st2.GetLogicalExtent()==Extent(Point(-3,-4),Point(4,5)),ostr.str());
  BOOST_CHECK(is_clear(st2));
}

BOOST_AUTO_TEST_CASE(complex_half_freq_init_3d)
{
  std::ostringstream ostr;
  ComplexHalfFrequencyImageState st3( Extent(Size(8,10,13)),PixelSampling() );
  ostr.str("");
  ostr << "expected " << Extent(Point(-3,-4,0),Point(4,5,6)) << " but got " << st3.GetExtent();
  BOOST_CHECK_MESSAGE(st3.GetExtent()==Extent(Point(-3,-4,0),Point(4,5,6)),ostr.str());
  ostr.str("");
  ostr << "expected " << Extent(Point(-3,-4,-6),Point(4,5,6)) << " but got " << st3.GetLogicalExtent();
  BOOST_CHECK_MESSAGE(st3.GetLogicalExtent()==Extent(Point(-3,-4,-6),Point(4,5,6)),ostr.str());
  BOOST_CHECK(is_clear(st3));
}

BOOST_AUTO_TEST_CASE(complex_half_freq_conjugate_access_1d)
{
  ComplexHalfFrequencyImageState st1( Extent(Size(15)),PixelSampling() );
  st1.ApplyIP(randomizer);
  st1.SetComplex(Point(0),Complex(0,0));
  for(int u=1;u<=7;++u) {
    BOOST_REQUIRE(st1.GetComplex(Point(u))==std::conj(st1.GetComplex(Point(-u))));
  }
}
BOOST_AUTO_TEST_CASE(complex_half_freq_conjugate_access_2d)
{
  std::ostringstream ostr;
  ComplexHalfFrequencyImageState st2( Extent(Size(4,7)),PixelSampling() );
  st2.ApplyIP(randomizer);
  st2.SetComplex(Point(0,0),Complex(0,0));
  for(int v=1;v<=3;++v) {
    for(int u=-1;u<=1;++u) {
      ostr.str("");
      ostr << "@" << Point(u,v) << " expected conjugate value of " << st2.GetComplex(Point(u,v));
      BOOST_REQUIRE_MESSAGE(st2.GetComplex(Point(u,v))==std::conj(st2.GetComplex(Point(-u,-v))),ostr.str());
    }
  }
}

BOOST_AUTO_TEST_CASE(complex_half_freq_conjugate_access_3d)
{
  std::ostringstream ostr;
  ComplexHalfFrequencyImageState st3( Extent(Size(3,4,5)),PixelSampling() );
  st3.ApplyIP(randomizer);
  st3.SetComplex(Point(0,0),Complex(0,0));
  for(int w=1;w<=2;++w) {
    for(int v=-1;v<=1;++v) {
      for(int u=-1;u<=1;++u) {
        ostr.str("");
        ostr << "@" << Point(u,v,w) << " expected conjugate value of " 
             << st3.GetComplex(Point(u,v,w));
        BOOST_REQUIRE_MESSAGE(st3.GetComplex(Point(u,v,w))==std::conj(st3.GetComplex(Point(-u,-v,-w))),ostr.str());
      }
    }
  }
}

BOOST_AUTO_TEST_SUITE_END()
