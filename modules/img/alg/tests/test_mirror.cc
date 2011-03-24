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
#include <ost/img/alg/randomize.hh>
#include <ost/img/alg/alg_mirror.hh>

using namespace ost::img;
using namespace ost::img::alg;


BOOST_AUTO_TEST_SUITE(ost_img_alg)


BOOST_AUTO_TEST_CASE(alg_mirror)
{
  ImageHandle ih1 = CreateImage(Extent(Point(-2,-1,1),Size(4,3,2)));
  ih1.ApplyIP(alg::Randomize());

  ImageHandle ih2 = ih1.Apply(Mirror(Plane::YZ));

  Extent ext=ih1.GetExtent();
  for(ExtentIterator it(ext);!it.AtEnd(); ++it) {
    Point p(it);
    Point mp(-p[0],p[1],p[2]);
    BOOST_REQUIRE(ih2.GetReal(mp)==ih1.GetReal(p));
  }

}

BOOST_AUTO_TEST_SUITE_END()