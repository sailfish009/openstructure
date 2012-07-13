//------------------------------------------------------------------------------
// This file is part of the OpenStructure project <www.openstructure.org>
//
// Copyright (C) 2008-2011 by the OpenStructure authors
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

#include <ost/gfx/color.hh>

#define BOOST_TEST_DYN_LINK
#include <boost/test/unit_test.hpp>
#include <boost/test/floating_point_comparison.hpp>

using boost::unit_test_framework::test_suite;

using namespace ost;
using namespace ost::gfx;

namespace {
  bool compare_colors(const Color& c1, const Color& c2, float tol=1e-6) {
    return std::fabs(c1.GetRed()-c2.GetRed()<tol) &&
      std::fabs(c1.GetGreen()-c2.GetGreen()<tol) &&
      std::fabs(c1.GetBlue()-c2.GetBlue()<tol) &&
      std::fabs(c1.GetAlpha()-c2.GetAlpha()<tol) &&
      std::fabs(c1.GetHue()-c2.GetHue()<tol) &&
      std::fabs(c1.GetSat()-c2.GetSat()<tol) &&
      std::fabs(c1.GetVal()-c2.GetVal()<tol);
  }
}

BOOST_AUTO_TEST_SUITE(gfx)

BOOST_AUTO_TEST_CASE(default_color)
{
  Color c;
  BOOST_CHECK_CLOSE(c.GetRed(),1.0,1e-4);
  BOOST_CHECK_CLOSE(c.GetGreen(),1.0,1e-4);
  BOOST_CHECK_CLOSE(c.GetBlue(),1.0,1e-4);
  BOOST_CHECK_CLOSE(c.GetAlpha(),1.0,1e-4);
  BOOST_CHECK_CLOSE(c.GetHue(),0.0,1e-4);
  BOOST_CHECK_CLOSE(c.GetSat(),0.0,1e-4);
  BOOST_CHECK_CLOSE(c.GetVal(),1.0,1e-4);
}

BOOST_AUTO_TEST_CASE(set_rgb)
{
  Color c;
  c = RGB(1.0,0.0,0.0); // red
  BOOST_CHECK_CLOSE(c.GetAlpha(),1.0,1e-4);
  BOOST_CHECK_CLOSE(c.GetRed(),1.0,1e-4);
  BOOST_CHECK_CLOSE(c.GetGreen(),0.0,1e-4);
  BOOST_CHECK_CLOSE(c.GetBlue(),0.0,1e-4);
  BOOST_CHECK_CLOSE(c.GetHue(),0.0/6.0,1e-4);
  BOOST_CHECK_CLOSE(c.GetSat(),1.0,1e-4);
  BOOST_CHECK_CLOSE(c.GetVal(),1.0,1e-4);
  c = RGB(1.0,1.0,0.0); // yellow
  BOOST_CHECK_CLOSE(c.GetRed(),1.0,1e-4);
  BOOST_CHECK_CLOSE(c.GetGreen(),1.0,1e-4);
  BOOST_CHECK_CLOSE(c.GetBlue(),0.0,1e-4);
  BOOST_CHECK_CLOSE(c.GetHue(),1.0/6.0,1e-4);
  BOOST_CHECK_CLOSE(c.GetSat(),1.0,1e-4);
  BOOST_CHECK_CLOSE(c.GetVal(),1.0,1e-4);
  c = RGB(0.0,1.0,0.0); // green
  BOOST_CHECK_CLOSE(c.GetRed(),0.0,1e-4);
  BOOST_CHECK_CLOSE(c.GetGreen(),1.0,1e-4);
  BOOST_CHECK_CLOSE(c.GetBlue(),0.0,1e-4);
  BOOST_CHECK_CLOSE(c.GetHue(),2.0/6.0,1e-4);
  BOOST_CHECK_CLOSE(c.GetSat(),1.0,1e-4);
  BOOST_CHECK_CLOSE(c.GetVal(),1.0,1e-4);
  c = RGB(0.0,1.0,1.0); // cyan
  BOOST_CHECK_CLOSE(c.GetRed(),0.0,1e-4);
  BOOST_CHECK_CLOSE(c.GetGreen(),1.0,1e-4);
  BOOST_CHECK_CLOSE(c.GetBlue(),1.0,1e-4);
  BOOST_CHECK_CLOSE(c.GetHue(),3.0/6.0,1e-4);
  BOOST_CHECK_CLOSE(c.GetSat(),1.0,1e-4);
  BOOST_CHECK_CLOSE(c.GetVal(),1.0,1e-4);
  c = RGB(0.0,0.0,1.0); // blue
  BOOST_CHECK_CLOSE(c.GetRed(),0.0,1e-4);
  BOOST_CHECK_CLOSE(c.GetGreen(),0.0,1e-4);
  BOOST_CHECK_CLOSE(c.GetBlue(),1.0,1e-4);
  BOOST_CHECK_CLOSE(c.GetHue(),4.0/6.0,1e-4);
  BOOST_CHECK_CLOSE(c.GetSat(),1.0,1e-4);
  BOOST_CHECK_CLOSE(c.GetVal(),1.0,1e-4);
  c = RGB(1.0,0.0,1.0); // purple
  BOOST_CHECK_CLOSE(c.GetRed(),1.0,1e-4);
  BOOST_CHECK_CLOSE(c.GetGreen(),0.0,1e-4);
  BOOST_CHECK_CLOSE(c.GetBlue(),1.0,1e-4);
  BOOST_CHECK_CLOSE(c.GetHue(),5.0/6.0,1e-4);
  BOOST_CHECK_CLOSE(c.GetSat(),1.0,1e-4);
  BOOST_CHECK_CLOSE(c.GetVal(),1.0,1e-4);
}

BOOST_AUTO_TEST_CASE(set_hsv)
{
  Color c;
  c = HSV(0.0/6.0,1.0,1.0); // red
  BOOST_CHECK_CLOSE(c.GetAlpha(),1.0,1e-4);
  BOOST_CHECK_CLOSE(c.GetRed(),1.0,1e-4);
  BOOST_CHECK_CLOSE(c.GetGreen(),0.0,1e-4);
  BOOST_CHECK_CLOSE(c.GetBlue(),0.0,1e-4);
  BOOST_CHECK_CLOSE(c.GetHue(),0.0/6.0,1e-4);
  BOOST_CHECK_CLOSE(c.GetSat(),1.0,1e-4);
  BOOST_CHECK_CLOSE(c.GetVal(),1.0,1e-4);
  c = HSV(0.0/6.0,0.5,1.0); // light red
  BOOST_CHECK_CLOSE(c.GetRed(),1.0,1e-4);
  BOOST_CHECK_CLOSE(c.GetGreen(),0.5,1e-4);
  BOOST_CHECK_CLOSE(c.GetBlue(),0.5,1e-4);
  BOOST_CHECK_CLOSE(c.GetHue(),0.0/6.0,1e-4);
  BOOST_CHECK_CLOSE(c.GetSat(),0.5,1e-4);
  BOOST_CHECK_CLOSE(c.GetVal(),1.0,1e-4);
  c = HSV(1.0/6.0,1.0,1.0); // yellow
  BOOST_CHECK_CLOSE(c.GetRed(),1.0,1e-4);
  BOOST_CHECK_CLOSE(c.GetGreen(),1.0,1e-4);
  BOOST_CHECK_CLOSE(c.GetBlue(),0.0,1e-4);
  BOOST_CHECK_CLOSE(c.GetHue(),1.0/6.0,1e-4);
  BOOST_CHECK_CLOSE(c.GetSat(),1.0,1e-4);
  BOOST_CHECK_CLOSE(c.GetVal(),1.0,1e-4);
  c = HSV(2.0/6.0,1.0,1.0); // green
  BOOST_CHECK_CLOSE(c.GetRed(),0.0,1e-4);
  BOOST_CHECK_CLOSE(c.GetGreen(),1.0,1e-4);
  BOOST_CHECK_CLOSE(c.GetBlue(),0.0,1e-4);
  BOOST_CHECK_CLOSE(c.GetHue(),2.0/6.0,1e-4);
  BOOST_CHECK_CLOSE(c.GetSat(),1.0,1e-4);
  BOOST_CHECK_CLOSE(c.GetVal(),1.0,1e-4);
  c = HSV(2.0/6.0,0.5,1.0); // light green
  BOOST_CHECK_CLOSE(c.GetRed(),0.5,1e-4);
  BOOST_CHECK_CLOSE(c.GetGreen(),1.0,1e-4);
  BOOST_CHECK_CLOSE(c.GetBlue(),0.5,1e-4);
  BOOST_CHECK_CLOSE(c.GetHue(),2.0/6.0,1e-4);
  BOOST_CHECK_CLOSE(c.GetSat(),0.5,1e-4);
  BOOST_CHECK_CLOSE(c.GetVal(),1.0,1e-4);
  c = HSV(3.0/6.0,1.0,1.0); // cyan
  BOOST_CHECK_CLOSE(c.GetRed(),0.0,1e-4);
  BOOST_CHECK_CLOSE(c.GetGreen(),1.0,1e-4);
  BOOST_CHECK_CLOSE(c.GetBlue(),1.0,1e-4);
  BOOST_CHECK_CLOSE(c.GetHue(),3.0/6.0,1e-4);
  BOOST_CHECK_CLOSE(c.GetSat(),1.0,1e-4);
  BOOST_CHECK_CLOSE(c.GetVal(),1.0,1e-4);
  c = HSV(4.0/6.0,1.0,1.0); // blue
  BOOST_CHECK_CLOSE(c.GetRed(),0.0,1e-4);
  BOOST_CHECK_CLOSE(c.GetGreen(),0.0,1e-4);
  BOOST_CHECK_CLOSE(c.GetBlue(),1.0,1e-4);
  BOOST_CHECK_CLOSE(c.GetHue(),4.0/6.0,1e-4);
  BOOST_CHECK_CLOSE(c.GetSat(),1.0,1e-4);
  BOOST_CHECK_CLOSE(c.GetVal(),1.0,1e-4);
  c = HSV(4.0/6.0,0.5,1.0); // light blue
  BOOST_CHECK_CLOSE(c.GetRed(),0.5,1e-4);
  BOOST_CHECK_CLOSE(c.GetGreen(),0.5,1e-4);
  BOOST_CHECK_CLOSE(c.GetBlue(),1.0,1e-4);
  BOOST_CHECK_CLOSE(c.GetHue(),4.0/6.0,1e-4);
  BOOST_CHECK_CLOSE(c.GetSat(),0.5,1e-4);
  BOOST_CHECK_CLOSE(c.GetVal(),1.0,1e-4);
  c = HSV(5.0/6.0,1.0,1.0); // purple
  BOOST_CHECK_CLOSE(c.GetRed(),1.0,1e-4);
  BOOST_CHECK_CLOSE(c.GetGreen(),0.0,1e-4);
  BOOST_CHECK_CLOSE(c.GetBlue(),1.0,1e-4);
  BOOST_CHECK_CLOSE(c.GetHue(),5.0/6.0,1e-4);
  BOOST_CHECK_CLOSE(c.GetSat(),1.0,1e-4);
  BOOST_CHECK_CLOSE(c.GetVal(),1.0,1e-4);
}

BOOST_AUTO_TEST_CASE(set_char)
{
  BOOST_CHECK(compare_colors(RGBb(255,0,0),RGB(1.0,0.0,0.0)));
  BOOST_CHECK(compare_colors(RGBAb(0,127,0,127),RGBA(0.0,0.5,0.0,0.5)));
}

BOOST_AUTO_TEST_CASE(set_int)
{
  BOOST_CHECK(compare_colors(RGBi(65535,65535,0),RGB(1.0,1.0,0.0)));
  BOOST_CHECK(compare_colors(RGBAi(0,32767,0,32767),RGBA(0.0,0.5,0.0,0.5)));
}

BOOST_AUTO_TEST_SUITE_END()
