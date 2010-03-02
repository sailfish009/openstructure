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
#define BOOST_TEST_DYN_LINK
#include <boost/test/unit_test.hpp>

#include <ost/seq/sequence_handle.hh>
#include <ost/seq/invalid_sequence.hh>
#include <ost/seq/sequence_op.hh>

using namespace ost;
using namespace ost::seq;

BOOST_AUTO_TEST_SUITE( seq )

BOOST_AUTO_TEST_CASE(seq_triv) 
{
  BOOST_CHECK_NO_THROW(CreateSequence("S1", "-afbcdefghijkLMNOPQRSTUV-"));
  BOOST_CHECK_THROW(CreateSequence("S1", "1"), InvalidSequence);
  BOOST_CHECK_THROW(CreateSequence("S1", "."), InvalidSequence);
  BOOST_CHECK_THROW(CreateSequence("S1", " "), InvalidSequence);
}

BOOST_AUTO_TEST_CASE(seq_getnum) 
{
  SequenceHandle s=CreateSequence("S1", "-afc--de-f");
  BOOST_CHECK_THROW(s.GetResidueIndex(0), Error);
  BOOST_CHECK(s.GetResidueIndex(1)==1-1);
  BOOST_CHECK(s.GetResidueIndex(2)==2-1);
  BOOST_CHECK(s.GetResidueIndex(3)==3-1);
  BOOST_CHECK(s.GetResidueIndex(6)==4-1);
  BOOST_CHECK(s.GetResidueIndex(7)==5-1);
  BOOST_CHECK(s.GetResidueIndex(9)==6-1);
  BOOST_CHECK_THROW(s.GetResidueIndex(-1), Error);
  BOOST_CHECK_THROW(s.GetResidueIndex(10), Error);
}

BOOST_AUTO_TEST_CASE(seq_getpos) 
{
  SequenceHandle s=CreateSequence("S1", "-afc--de-f");

  BOOST_CHECK(s.GetPos(0)==1);
  BOOST_CHECK(s.GetPos(1)==2);
  BOOST_CHECK(s.GetPos(2)==3);
  BOOST_CHECK(s.GetPos(3)==6);
  BOOST_CHECK(s.GetPos(4)==7);
  BOOST_CHECK(s.GetPos(5)==9);
}

BOOST_AUTO_TEST_SUITE_END()
