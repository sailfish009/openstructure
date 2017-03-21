//------------------------------------------------------------------------------
// This file is part of the OpenStructure project <www.openstructure.org>
//
// Copyright (C) 2008-2017 by the OpenStructure authors
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

// the idea of this test is basically to check that we MessagePack is available
// to OST. Real testing of MessagePack should happen in that project itself.

#define BOOST_TEST_DYN_LINK
#include <boost/test/unit_test.hpp>
#include <boost/test/auto_unit_test.hpp>
#include <string>

#include <ost/string_ref.hh>
#include <ost/io/msgpack/msgpack.hpp>

using namespace ost;

BOOST_AUTO_TEST_SUITE( io );

BOOST_AUTO_TEST_CASE(msgpack_test)
{
  /* just trying if MsgPack works in general, mostly copied from the examples at
     http://msgpack.org/index.html */
  msgpack::type::tuple<int, bool, String> src(1, true, "example");

  // Serialize the object into the buffer.
  std::stringstream buffer;
  msgpack::pack(buffer, src);

  // send the buffer ...
  buffer.seekg(0);

  // deserialize the buffer into msgpack::object instance.
  String str(buffer.str());
  msgpack::object_handle oh = msgpack::unpack(str.data(), str.size());

  /* Deserialized object is valid during the msgpack::object_handle instance is
     alive. */
  msgpack::object deserialized = oh.get();

  /* Convert msgpack::object instance into the original type.
     If the type is mismatched, it throws msgpack::type_error exception. */
  msgpack::type::tuple<int, bool, String> dst;
  deserialized.convert(dst);
  BOOST_CHECK(msgpack::type::get<0>(dst) == 1);
  BOOST_CHECK(msgpack::type::get<1>(dst) == true);
  BOOST_CHECK(msgpack::type::get<2>(dst) == "example");
}

BOOST_AUTO_TEST_SUITE_END();
