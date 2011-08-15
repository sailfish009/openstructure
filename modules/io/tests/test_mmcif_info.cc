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

#include <ost/io/io_exception.hh>
#include <ost/io/mol/mmcif_info.hh>

#define BOOST_AUTO_TEST_DYN_LINK
#include <boost/test/unit_test.hpp>


using namespace ost;
using namespace ost::io;

BOOST_AUTO_TEST_SUITE( io );

BOOST_AUTO_TEST_CASE(mmcif_info_citation)
{
  BOOST_MESSAGE("  Running mmcif_info_citation tests...");

  MMCifInfoCitation cit = MMCifInfoCitation();
  std::vector<String> author_list;

  author_list.push_back("Kabel, H.");

  cit.SetID("ID");
  cit.SetCAS("FOO");
  cit.SetISBN("0-0-0-0-0");
  cit.SetPublishedIn("Journal of Uncanny Science");
  cit.SetVolume("3");
  cit.SetPageFirst("1");
  cit.SetPageLast("10");
  cit.SetDOI("HERE");
  cit.SetPubMed(815);
  cit.SetYear(815);
  cit.SetTitle("Foo");
  cit.SetAuthorList(author_list);
  author_list.clear();

  BOOST_CHECK(cit.GetID() == "ID");
  BOOST_CHECK(cit.GetCAS() == "FOO");
  BOOST_CHECK(cit.GetISBN() == "0-0-0-0-0");
  BOOST_CHECK(cit.GetPublishedIn() == "Journal of Uncanny Science");
  BOOST_CHECK(cit.GetVolume() == "3");
  BOOST_CHECK(cit.GetPageFirst() == "1");
  BOOST_CHECK(cit.GetPageLast() == "10");
  BOOST_CHECK(cit.GetDOI() == "HERE");
  BOOST_CHECK(cit.GetPubMed() == 815);
  BOOST_CHECK(cit.GetYear() == 815);
  BOOST_CHECK(cit.GetTitle() == "Foo");
  author_list = cit.GetAuthorList();
  BOOST_CHECK(author_list.back() == "Kabel, H.");

  BOOST_MESSAGE("  done.");
  BOOST_MESSAGE("  trying to add everything to an info object");
  MMCifInfo info = MMCifInfo();
  info.AddCitation(cit);
  std::vector<MMCifInfoCitation> citations = info.GetCitations();
  BOOST_CHECK(citations.size() == 1);
  BOOST_CHECK(citations.back() == cit);
  BOOST_CHECK_THROW(info.AddAuthorsToCitation(StringRef("Foo", 3),
                                              author_list),
                    IOException);
  BOOST_MESSAGE("  done.");
}

BOOST_AUTO_TEST_SUITE_END();
