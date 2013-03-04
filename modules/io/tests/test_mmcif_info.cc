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
#include <boost/test/auto_unit_test.hpp>
#include <boost/test/floating_point_comparison.hpp>

using namespace ost;
using namespace ost::io;

BOOST_AUTO_TEST_SUITE( io );

BOOST_AUTO_TEST_CASE(mmcif_info_obsolete)
{
  BOOST_MESSAGE("  Running mmcif_info_obsolete tests...");

  MMCifInfoObsolete obs = MMCifInfoObsolete();

  obs.SetDate("2011-08-31");
  obs.SetID(StringRef("OBSLTE", 6));
  obs.SetPDBID("1FOO");
  obs.SetReplacedPDBID("1BAR");

  BOOST_CHECK(obs.GetDate() == "2011-08-31");
  BOOST_CHECK(obs.GetID() == "Obsolete");
  BOOST_CHECK(obs.GetPDBID() == "1FOO");
  BOOST_CHECK(obs.GetReplacedPDBID() == "1BAR");

  obs.SetID(StringRef("SPRSDE", 6));
  BOOST_CHECK(obs.GetID() == "Supersede");

  BOOST_MESSAGE("  done.");
}

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

BOOST_AUTO_TEST_CASE(mmcif_info_biounit)
{
  BOOST_MESSAGE("  Running mmcif_info_biounit tests...");

  MMCifInfoBioUnit bu = MMCifInfoBioUnit();

  bu.SetDetails("author_defined_assembly");
  bu.AddChain("A");

  BOOST_CHECK(bu.GetDetails() == "author_defined_assembly");
  BOOST_CHECK(bu.GetChainList().back() == "A");

  std::vector<String> chains;
  chains.push_back("B");
  bu.SetChainList(chains);
  BOOST_CHECK(bu.GetChainList().back() == "B");

  MMCifInfo info = MMCifInfo();
  info.AddBioUnit(bu);
  std::vector<MMCifInfoBioUnit> biounits = info.GetBioUnits();
  BOOST_CHECK(biounits.size() == 1);
  BOOST_CHECK(biounits.back() == bu);

  BOOST_MESSAGE("  done.");
}

BOOST_AUTO_TEST_CASE(mmcif_info_transoperation)
{
  BOOST_MESSAGE("  Running mmcif_info_transoperation tests...");

  MMCifInfoTransOpPtr op(new MMCifInfoTransOp);

  op->SetID("1");
  op->SetType("identity operation");
  op->SetVector(1, 2, 3);
  op->SetMatrix(1, 2, 3, 4, 5, 6, 7 ,8, 9);

  BOOST_CHECK(op->GetID() == "1");
  BOOST_CHECK(op->GetVector() == geom::Vec3(1, 2, 3));
  BOOST_CHECK(op->GetMatrix() == geom::Mat3(1, 2, 3, 4, 5, 6, 7, 8, 9));
  BOOST_CHECK(op->GetType() == "identity operation");

  MMCifInfo info = MMCifInfo();
  info.AddOperation(op);
  BOOST_CHECK(info.GetOperations().back() == op);

  std::vector<MMCifInfoTransOpPtr> ops;
  ops.push_back(*(info.GetOperations().begin()));
  MMCifInfoBioUnit bu = MMCifInfoBioUnit();
  bu.AddOperations(ops);
  BOOST_CHECK((*(bu.GetOperations().begin()->begin())) == op);

  BOOST_MESSAGE("  done.");
}

BOOST_AUTO_TEST_CASE(mmcif_info_structdetails)
{
  BOOST_MESSAGE("  Running mmcif_info_structdetails tests...");

  MMCifInfoStructDetails sd = MMCifInfoStructDetails();

  sd.SetEntryID("1BAR");
  sd.SetTitle("More than a structure");
  sd.SetCASPFlag('Y');
  sd.SetDescriptor("ADENYLATE KINASE");
  sd.SetMass(1.0);
  sd.SetMassMethod("Good Guess");
  sd.SetModelDetails("Even more guessing");
  sd.SetModelTypeDetails("MINIMIZED AVERAGE");

  BOOST_CHECK(sd.GetEntryID() == "1BAR");
  BOOST_CHECK(sd.GetTitle() == "More than a structure");
  BOOST_CHECK(sd.GetCASPFlag() == 'Y');
  BOOST_CHECK(sd.GetDescriptor() == "ADENYLATE KINASE");
  #if OST_DOUBLE_PRECISION
  BOOST_CHECK_CLOSE(sd.GetMass(), 1.0, 0.001);
  #else
  BOOST_CHECK_CLOSE(sd.GetMass(), 1.0f, 0.001f);
  #endif
  BOOST_CHECK(sd.GetMassMethod() == "Good Guess");
  BOOST_CHECK(sd.GetModelDetails() == "Even more guessing");
  BOOST_CHECK(sd.GetModelTypeDetails() == "MINIMIZED AVERAGE");

  MMCifInfo info = MMCifInfo();
  info.SetStructDetails(sd);
  BOOST_CHECK(info.GetStructDetails() == sd);

  BOOST_MESSAGE("  done.");
}

BOOST_AUTO_TEST_CASE(mmcif_info_revisions)
{
  BOOST_MESSAGE("  Running mmcif_info_revisions tests...");

  MMCifInfoRevisions rev = MMCifInfoRevisions();

  BOOST_CHECK(rev.GetDateOriginal() == "?");

  rev.SetDateOriginal("2012-05-04");
  rev.AddRevision(1, "2012-05-04", "in preparation");
  rev.AddRevision(2, "2012-05-05", "full release");

  BOOST_CHECK(rev.GetSize() == 2);
  BOOST_CHECK(rev.GetDateOriginal() == "2012-05-04");
  BOOST_CHECK(rev.GetDate(0) == "2012-05-04");
  BOOST_CHECK(rev.GetNum(0) == 1);
  BOOST_CHECK(rev.GetStatus(0) == "in preparation");
  BOOST_CHECK(rev.GetDate(1) == rev.GetLastDate());
  BOOST_CHECK(rev.GetFirstRelease() == 2);
  BOOST_CHECK(rev.GetNum(1) == 2);
  BOOST_CHECK(rev.GetStatus(1) == "full release");  

  BOOST_MESSAGE("  done.");
}

BOOST_AUTO_TEST_CASE(mmcif_info)
{
  BOOST_MESSAGE("  Running mmcif_info tests...");

  MMCifInfo info = MMCifInfo();

  info.SetMethod("Cooking.");
  info.SetResolution(1.9f);

  BOOST_CHECK(info.GetMethod() == StringRef("Cooking.", 8));
  #if OST_DOUBLE_PRECISION
  BOOST_CHECK_CLOSE(info.GetResolution(), 1.9, 0.001);
  #else
  BOOST_CHECK_CLOSE(info.GetResolution(), 1.9f, 0.001f);
  #endif

  info.AddMMCifPDBChainTr("A", "B");
  BOOST_CHECK_THROW(info.AddMMCifPDBChainTr("A", "B"), IOException);
  BOOST_CHECK("B" == info.GetMMCifPDBChainTr("A"));
  BOOST_CHECK("" == info.GetMMCifPDBChainTr("C"));

  info.AddPDBMMCifChainTr("A", "B");
  BOOST_CHECK_THROW(info.AddPDBMMCifChainTr("A", "B"), IOException);
  BOOST_CHECK("B" == info.GetPDBMMCifChainTr("A"));
  BOOST_CHECK("" == info.GetPDBMMCifChainTr("C"));

  BOOST_CHECK(info.GetRevisions().GetSize() == 0);

  BOOST_MESSAGE("  done.");
}


BOOST_AUTO_TEST_SUITE_END();
