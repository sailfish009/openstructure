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
  Author: Valerio Mariani
*/

#include <iostream>

#include "tests.hh"

#include <ost/iplt/image.hh>
#include <ost/iplt/alg/circular_mask.hh>
#include <ost/iplt/alg/randomize.hh>

namespace {

using namespace ost::iplt;
using namespace ost::iplt::alg;

void test() 
{

  ImageHandle im =  CreateImage(Size(400,400),REAL,SPATIAL);
  im.ApplyIP(Randomize()); 
  CircularMask cm;
  cm.SetMaskCenterOffset(::geom::Vec2(100,100));
  cm.SetFillValue(0.0);
  im.ApplyIP(cm);

  ImageHandle im2 =  CreateImage(Size(400,400),REAL,SPATIAL);
  im2.ApplyIP(Randomize()); 
  im2.SetSpatialOrigin(Point(100,100));
  cm.SetRadius(10);
  im2.ApplyIP(cm);


  ImageHandle im3 =  CreateImage(Size(400,400),REAL,SPATIAL);
  im3.ApplyIP(Randomize()); 
  cm.SetMaskCenterOffsetToDefault();
  cm.SetRadiusToDefault();
  cm.SetFillValueToDefault();
  cm.SetSigma(6.0);
  im3.ApplyIP(cm);
}

} // ns

test_suite* CreateCircularMaskTest()
{
  test_suite* ts=BOOST_TEST_SUITE("Circular Mask Test");

  ts->add(BOOST_TEST_CASE(&test));

  return ts;
}
