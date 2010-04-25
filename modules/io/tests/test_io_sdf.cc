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
#include <ost/io/mol/entity_io_sdf_handler.hh>
#define BOOST_TEST_DYN_LINK
#include <boost/test/unit_test.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/algorithm/string.hpp>

using namespace ost;
using namespace ost::io;

BOOST_AUTO_TEST_SUITE( io )


BOOST_AUTO_TEST_CASE(test_io_sdf) 
{
  const String fname("testfiles/test_in.sdf");

  mol::EntityHandle eh=mol::CreateEntity();
  EntityIOSDFHandler sdfh;

  // check format
  BOOST_CHECK(EntityIOSDFHandler::ProvidesImport("","sdf"));
  BOOST_CHECK(EntityIOSDFHandler::ProvidesImport(fname));
  BOOST_CHECK(EntityIOSDFHandler::ProvidesImport("test_in.SDF"));

  // check import
  sdfh.Import(eh,"testfiles/test_in.sdf");

  // check atoms/bonds
  BOOST_CHECK_EQUAL(eh.GetChainCount(), 4);
  BOOST_CHECK_EQUAL(eh.GetAtomCount(), 180);
  BOOST_CHECK_EQUAL(eh.GetBondCount(), 188);

  // check molecule name
  mol::ChainHandle ch=eh.FindChain("00003_Displayed atoms");
  BOOST_CHECK(ch.IsValid());

  // check properties
  BOOST_CHECK(ch.HasProp("r_i_glide_rmsd"));
  BOOST_CHECK_EQUAL(boost::lexical_cast<Real>(boost::trim_copy
                     (ch.GetStringProp("r_i_glide_rmsd"))),
                     0.543804f);
}

BOOST_AUTO_TEST_SUITE_END()
