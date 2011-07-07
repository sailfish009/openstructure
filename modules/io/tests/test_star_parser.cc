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
  Author: Marco Biasini
 */
#include <fstream>
#include <ost/mol/mol.hh>
#include <ost/io/io_exception.hh>
#include <ost/io/mol/star_parser.hh>

#define BOOST_AUTO_TEST_DYN_LINK
#include <boost/test/unit_test.hpp>


using namespace ost;
using namespace ost::io;


class DataItemTestParser: public StarParser {
public:
  DataItemTestParser(std::istream& stream): StarParser(stream)
  { }
  virtual void OnDataItem(const StarDataItem& item)
  {
    BOOST_CHECK_EQUAL(item.GetCategory().str(), "data-item");
    if (item.GetName()==StringRef("s1", 2)) {
      s1=item.GetValue().str();
    }      
    if (item.GetName()==StringRef("s2", 2)) {
      s2=item.GetValue().str();
    }
    if (item.GetName()==StringRef("s3", 2)) {
      s3=item.GetValue().str();
    }
    if (item.GetName()==StringRef("s4", 2)) {
      s4=item.GetValue().str();
    }    
  }
  String s1;
  String s2;
  String s3;
  String s4;  
};

class LoopTestParser: public StarParser {
public:
  LoopTestParser(std::istream& stream): StarParser(stream), count_(0)
  { }
  virtual bool OnBeginLoop(const StarLoopDesc& header)
  {
    BOOST_CHECK_EQUAL(header.GetCategory(), "loop");
    return true;
  }  
  virtual void OnDataRow(const StarLoopDesc& header, 
                         const std::vector<StringRef>& columns) 
  {
    BOOST_CHECK_EQUAL(header.GetCategory(), "loop");
    BOOST_CHECK_EQUAL(columns.size(), size_t(3));
    lines[count_][0]=columns[0].str();
    lines[count_][1]=columns[1].str();
    lines[count_][2]=columns[2].str();
    count_++;            
  }
  String lines[5][3];
private:
  int count_;
};

class MultiDataParser : public StarParser {
public:
  MultiDataParser(std::istream& stream): StarParser(stream), visit_one(false),
     visit_two(false)
  { }
  
  virtual bool OnBeginData(const StringRef& data)
  {
    if (data==StringRef("one", 3)) {
      visit_one=true;
    }
    if (data==StringRef("two", 3)) {
      visit_two=true;
    }
    return true;
  }
  
  bool visit_one;
  bool visit_two;
};

BOOST_AUTO_TEST_SUITE( io );

BOOST_AUTO_TEST_CASE(star_split1)
{
  const char* line1="     ";
  StringRef r1(line1, strlen(line1));
  std::vector<StringRef> parts;
  BOOST_REQUIRE(StarParser::SplitLine(r1, parts));
  BOOST_REQUIRE(parts.size()==0);
}

BOOST_AUTO_TEST_CASE(star_split2)
{
  const char* line2="1 2";
  std::vector<StringRef> parts;  
  StringRef r2(line2, strlen(line2));
  BOOST_REQUIRE(StarParser::SplitLine(r2, parts));
  BOOST_REQUIRE(parts.size()==2);
  BOOST_CHECK_EQUAL(parts[0], StringRef("1", 1));
  BOOST_CHECK_EQUAL(parts[1], StringRef("2", 1));
}

BOOST_AUTO_TEST_CASE(star_split3)
{
  const char* line3="'1 2'  ";
  std::vector<StringRef> parts;  
  StringRef r3(line3, strlen(line3));
  BOOST_REQUIRE(StarParser::SplitLine(r3, parts));
  BOOST_REQUIRE(parts.size()==1);
  BOOST_CHECK_EQUAL(parts[0], StringRef("1 2", 3));
}
    
BOOST_AUTO_TEST_CASE(star_data_item)
{
  std::ifstream s("testfiles/data-item.cif");
  DataItemTestParser star_p(s);
  star_p.Parse();
  BOOST_CHECK_EQUAL(star_p.s1, "a");
  BOOST_CHECK_EQUAL(star_p.s2, "a b c");
  BOOST_CHECK_EQUAL(star_p.s3, "a\nb\nc");
  BOOST_CHECK_EQUAL(star_p.s4, "a'b");
}

BOOST_AUTO_TEST_CASE(star_multi)
{
  std::ifstream s("testfiles/multi-data.cif");
  MultiDataParser star_p(s);
  star_p.Parse();
  BOOST_CHECK(star_p.visit_one);
  BOOST_CHECK(star_p.visit_two);
}
  
BOOST_AUTO_TEST_CASE(star_loop)
{
  std::ifstream s("testfiles/loop.cif");
  LoopTestParser star_p(s);
  star_p.Parse();
  BOOST_CHECK_EQUAL(star_p.lines[0][0], "1");
  BOOST_CHECK_EQUAL(star_p.lines[0][1], "2");
  BOOST_CHECK_EQUAL(star_p.lines[0][2], "3");
  
  BOOST_CHECK_EQUAL(star_p.lines[1][0], "4 x");
  BOOST_CHECK_EQUAL(star_p.lines[1][1], "5");
  BOOST_CHECK_EQUAL(star_p.lines[1][2], "6");
  
  BOOST_CHECK_EQUAL(star_p.lines[2][0], "7");
  BOOST_CHECK_EQUAL(star_p.lines[2][1], "8");
  BOOST_CHECK_EQUAL(star_p.lines[2][2], "9");

  BOOST_CHECK_EQUAL(star_p.lines[3][0], "10");
  BOOST_CHECK_EQUAL(star_p.lines[3][1], "11");
  BOOST_CHECK_EQUAL(star_p.lines[3][2], "12");
    
  BOOST_CHECK_EQUAL(star_p.lines[4][0], "13");
  BOOST_CHECK_EQUAL(star_p.lines[4][1], "14");
  BOOST_CHECK_EQUAL(star_p.lines[4][2], "15");
}

BOOST_AUTO_TEST_CASE(star_missing_data)
{
  std::ifstream s("testfiles/missing_data.cif");
  LoopTestParser star_p(s);
  BOOST_CHECK_THROW(star_p.Parse(), IOException);
}
BOOST_AUTO_TEST_SUITE_END();

