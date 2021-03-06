//------------------------------------------------------------------------------
// This file is part of the OpenStructure project <www.openstructure.org>
//
// Copyright (C) 2008-2020 by the OpenStructure authors
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
#include <ost/img/alg/randomize.hh>
#include <ost/img/alg/fft.hh>
#include <ost/img/alg/power_spectrum.hh>

namespace {

using namespace ost::img;

void test_real() 
{
  ImageHandle im = CreateImage(Size(8,9),REAL,SPATIAL);
  im.ApplyIP(alg::Randomize());
  ImageHandle im_fft = im.Apply(alg::FFT());
  ImageHandle im_ps = im.Apply(alg::PowerSpectrum());

  std::ostringstream msg;

  BOOST_REQUIRE(im_ps.GetExtent()==im_fft.GetExtent());

  for(ExtentIterator it(im_ps.GetExtent());!it.AtEnd();++it) {
    msg.str("");
    Real val1=im_ps.GetReal(it);
    Real val2=std::abs(im_fft.GetComplex(it));
    msg << "@ " << Point(it) << " " << val1 << " != " << val2; 
    BOOST_REQUIRE_MESSAGE(std::fabs(val1-val2)<1e-10,msg.str());
  }
}

void test_cmplx() 
{
  ImageHandle im = CreateImage(Size(8,9),COMPLEX,SPATIAL);
  im.ApplyIP(alg::Randomize());
  ImageHandle im_fft = im.Apply(alg::FFT());
  ImageHandle im_ps = im.Apply(alg::PowerSpectrum());

  std::ostringstream msg;

  BOOST_REQUIRE(im_ps.GetExtent()==im_fft.GetExtent());

  for(ExtentIterator it(im_ps.GetExtent());!it.AtEnd();++it) {
    msg.str("");
    Real val1=im_ps.GetReal(it);
    Real val2=std::abs(im_fft.GetComplex(it));
    msg << "@ " << Point(it) << " " << val1 << " != " << val2; 
    BOOST_REQUIRE_MESSAGE(std::fabs(val1-val2)<1e-10,msg.str());
  }
}

} // ns

test_suite* CreatePowerSpectrumTest()
{
  test_suite* ts=BOOST_TEST_SUITE("Power Spectrum Test");

  ts->add(BOOST_TEST_CASE(&test_real));
  ts->add(BOOST_TEST_CASE(&test_cmplx));

  return ts;
}
