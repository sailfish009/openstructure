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

#include <ost/unit_cell.hh>
#define BOOST_TEST_DYN_LINK
#include <boost/test/unit_test.hpp>

using namespace ost;


#define CHECK_VEC3_CLOSE(vec_a,vec_b) do { \
     geom::Vec3 va=vec_a; \
     geom::Vec3 vb=vec_b; \
     BOOST_CHECK(static_cast<double>(std::fabs(va.x-vb.x))<1e-6); \
     BOOST_CHECK(static_cast<double>(std::fabs(va.y-vb.y))<1e-6); \
     BOOST_CHECK(static_cast<double>(std::fabs(va.z-vb.z))<1e-6); \
   } while(0);


BOOST_AUTO_TEST_SUITE(base)

BOOST_AUTO_TEST_CASE(test_default_ctor)
{
  UnitCell cell;
  Real pi_half=static_cast<Real>(M_PI/2);
  // angles first
  BOOST_CHECK(std::fabs(cell.GetAlpha()-pi_half)<static_cast<Real>(1e-6));
  BOOST_CHECK(std::fabs(cell.GetBeta()-pi_half)<static_cast<Real>(1e-6));
  BOOST_CHECK(std::fabs(cell.GetGamma()-pi_half)<static_cast<Real>(1e-6));
  
  CHECK_VEC3_CLOSE(cell.GetA(), geom::Vec3(1,0,0));
  CHECK_VEC3_CLOSE(cell.GetB(), geom::Vec3(0,1,0));
  CHECK_VEC3_CLOSE(cell.GetC(), geom::Vec3(0,0,1));
}

BOOST_AUTO_TEST_CASE(test_from_vec)
{
  geom::Vec3 a_vec(2,0,0);
  geom::Vec3 b_vec(cos(M_PI/4), sin(M_PI/4), 0.0);
  geom::Vec3 c_vec(0,0,3);
  UnitCell cell(a_vec, b_vec, c_vec);
  Real pi_half=static_cast<Real>(M_PI/2);
  Real pi_quarter=static_cast<Real>(M_PI/4);
  // angles first
  BOOST_CHECK(std::fabs(cell.GetAlpha()-pi_half)<static_cast<Real>(1e-6));
  BOOST_CHECK(std::fabs(cell.GetBeta()-pi_half)<static_cast<Real>(1e-6));
  BOOST_CHECK(std::fabs(cell.GetGamma()-pi_quarter)<static_cast<Real>(1e-6));
  
  CHECK_VEC3_CLOSE(cell.GetA(), a_vec);
  CHECK_VEC3_CLOSE(cell.GetB(), b_vec);
  CHECK_VEC3_CLOSE(cell.GetC(), c_vec);
}


BOOST_AUTO_TEST_CASE(test_from_angle_ortho)
{
  Real pi_half=static_cast<Real>(M_PI/2);
  UnitCell cell(1.0, 1.0, 1.0, pi_half, pi_half, pi_half);

  BOOST_CHECK(std::fabs(cell.GetAlpha()-pi_half)<static_cast<Real>(1e-6));
  BOOST_CHECK(std::fabs(cell.GetBeta()-pi_half)<static_cast<Real>(1e-6));
  BOOST_CHECK(std::fabs(cell.GetGamma()-pi_half)<static_cast<Real>(1e-6));
}

BOOST_AUTO_TEST_CASE(test_from_angle_rombic)
{
  Real pi_half=static_cast<Real>(M_PI/2);  
  Real pi_quarter=static_cast<Real>(M_PI/4);
  UnitCell cell(1.0, 1.0, 1.0, pi_half, pi_half, pi_quarter);

  BOOST_CHECK(std::fabs(cell.GetAlpha()-pi_half)<static_cast<Real>(1e-6));
  BOOST_CHECK(std::fabs(cell.GetBeta()-pi_half)<static_cast<Real>(1e-6));
  BOOST_CHECK(std::fabs(cell.GetGamma()-pi_quarter)<static_cast<Real>(1e-6));
}

BOOST_AUTO_TEST_CASE(fract_to_ortho)
{
  Real pi_half=static_cast<Real>(M_PI/2);  
  Real pi_quarter=static_cast<Real>(M_PI/4);
  UnitCell cell(1.0, 2.0, 4.0, pi_half, pi_half, pi_quarter);
  geom::Vec3 a=cell.GetOrthoTF()*geom::Vec3(1,0,0);
  geom::Vec3 b=cell.GetOrthoTF()*geom::Vec3(0,1,0);
  geom::Vec3 c=cell.GetOrthoTF()*geom::Vec3(0,0,1);
  std::cout << a << b << c << std::endl ;
  CHECK_VEC3_CLOSE(a, geom::Vec3(1,0,0));
  CHECK_VEC3_CLOSE(b, geom::Vec3(cos(pi_quarter)*2, sin(pi_quarter)*2, 0));
  CHECK_VEC3_CLOSE(c, geom::Vec3(0, 0, 4));
}


BOOST_AUTO_TEST_SUITE_END()
