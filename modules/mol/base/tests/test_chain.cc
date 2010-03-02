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
/*
 *  Authors: Marco Biasini, Juergen Haas
 */
#include <ost/mol/mol.hh>
#include <ost/message.hh>
#define BOOST_TEST_DYN_LINK
#include <boost/test/unit_test.hpp>

using namespace ost;
using namespace ost::mol;

void find_and_check_res(ChainHandle chain, const ResNum& n) {
  ResidueHandle fa=chain.FindResidue(n);
  BOOST_CHECK_MESSAGE(fa.IsValid(), "residue " << n << " not found");
  if (!fa.IsValid())
    return;
  BOOST_CHECK_MESSAGE(fa.GetNumber()==n,  
                      "residue " << n << " expected, but "
                      << fa.GetNumber() << " found");             
}

void no_find_res(ChainHandle chain, const ResNum& n) {
  ResidueHandle fa=chain.FindResidue(n);
  BOOST_CHECK_MESSAGE(fa.IsValid()==false, "residue "
                      << " found while looking for " << n
                      << " (should be invalid)");    
}

void test_res_pos() {
  EntityHandle eh=CreateEntity();
  XCSEditor e=eh.RequestXCSEditor();  
  ChainHandle ch1=e.InsertChain("A");
  e.AppendResidue(ch1, "A");
  e.AppendResidue(ch1, "B");
  e.AppendResidue(ch1, "C", ResNum(5));
  e.AppendResidue(ch1, "D");  
  e.AppendResidue(ch1, "E", ResNum(9));
  e.AppendResidue(ch1, "F");  
  e.AppendResidue(ch1, "E", ResNum(12));  
  e.AppendResidue(ch1, "G");    
  find_and_check_res(ch1, ResNum(1));
  find_and_check_res(ch1, ResNum(2));  
  no_find_res(ch1, ResNum(3));
  no_find_res(ch1, ResNum(4));  
  find_and_check_res(ch1, ResNum(5));    
  find_and_check_res(ch1, ResNum(6));    
  no_find_res(ch1, ResNum(7));    
  no_find_res(ch1, ResNum(8));    
  find_and_check_res(ch1, ResNum(9));
  find_and_check_res(ch1, ResNum(10));  
  no_find_res(ch1, ResNum(11));    
  find_and_check_res(ch1, ResNum(12));    
  find_and_check_res(ch1, ResNum(13));  
}

BOOST_AUTO_TEST_SUITE( mol_base )

BOOST_AUTO_TEST_CASE(test_comparison) 
{
  EntityHandle eh=CreateEntity();
  XCSEditor e=eh.RequestXCSEditor();  
  ChainHandle ch1=e.InsertChain("A");
  ChainHandle ch2=e.InsertChain("B");
  ChainHandle cc=ch1;
  BOOST_CHECK(ch1!=ch2);
  BOOST_CHECK(ch1==ch1);
  BOOST_CHECK(cc==ch1);
}


BOOST_AUTO_TEST_CASE(res_pos) 
{
  EntityHandle eh=CreateEntity();
  XCSEditor e=eh.RequestXCSEditor();  
  ChainHandle ch1=e.InsertChain("A");
  e.AppendResidue(ch1, "A");
  e.AppendResidue(ch1, "B");
  e.AppendResidue(ch1, "C", ResNum(5));
  e.AppendResidue(ch1, "D");  
  e.AppendResidue(ch1, "E", ResNum(9));
  e.AppendResidue(ch1, "F");  
  e.AppendResidue(ch1, "E", ResNum(12));  
  e.AppendResidue(ch1, "G");    
  find_and_check_res(ch1, ResNum(1));
  find_and_check_res(ch1, ResNum(2));  
  no_find_res(ch1, ResNum(3));
  no_find_res(ch1, ResNum(4));  
  find_and_check_res(ch1, ResNum(5));    
  find_and_check_res(ch1, ResNum(6));    
  no_find_res(ch1, ResNum(7));    
  no_find_res(ch1, ResNum(8));    
  find_and_check_res(ch1, ResNum(9));
  find_and_check_res(ch1, ResNum(10));  
  no_find_res(ch1, ResNum(11));    
  find_and_check_res(ch1, ResNum(12));    
  find_and_check_res(ch1, ResNum(13));
}


BOOST_AUTO_TEST_CASE(res_pos_with_insertion_code) 
{
  EntityHandle eh=CreateEntity();
  XCSEditor e=eh.RequestXCSEditor();  
  ChainHandle ch1=e.InsertChain("A");
  e.AppendResidue(ch1, "A");
  e.AppendResidue(ch1, "B");
  // test for normal insertion
  e.AppendResidue(ch1, "C", ResNum(5, 'A'));
  e.AppendResidue(ch1, "D", ResNum(5, 'B'));  
  e.AppendResidue(ch1, "E", ResNum(6));  
  e.AppendResidue(ch1, "E", ResNum(7));
  // test for insertion plus gap right after
  e.AppendResidue(ch1, "E", ResNum(7, 'B'));  
  e.AppendResidue(ch1, "E", ResNum(9));      
  find_and_check_res(ch1, ResNum(1));
  find_and_check_res(ch1, ResNum(2));  
  no_find_res(ch1, ResNum(3));
  no_find_res(ch1, ResNum(4));  
  find_and_check_res(ch1, ResNum(5, 'A'));    
  find_and_check_res(ch1, ResNum(5, 'B'));    
  find_and_check_res(ch1, ResNum(6));        

  find_and_check_res(ch1, ResNum(7));      
  find_and_check_res(ch1, ResNum(7, 'B'));   
  no_find_res(ch1, ResNum(8));  
  find_and_check_res(ch1, ResNum(9));        
  no_find_res(ch1, ResNum(10));
}

BOOST_AUTO_TEST_CASE(prev_next) 
{
  EntityHandle eh=CreateEntity();
  XCSEditor e=eh.RequestXCSEditor();  
  ChainHandle ch1=e.InsertChain("A");
  e.AppendResidue(ch1, "A");
  e.AppendResidue(ch1, "B");
  e.AppendResidue(ch1, "C");  
  
  BOOST_REQUIRE(ch1.GetPrev(ch1.FindResidue(ResNum(2))).IsValid());
  BOOST_CHECK(ch1.GetPrev(ch1.FindResidue(ResNum(2))).GetKey()=="A");  
  BOOST_REQUIRE(ch1.GetNext(ch1.FindResidue(ResNum(2))).IsValid());
  BOOST_CHECK(ch1.GetNext(ch1.FindResidue(ResNum(2))).GetKey()=="C");
  BOOST_CHECK(!ch1.GetPrev(ch1.FindResidue(ResNum(1))).IsValid());  
  BOOST_CHECK(!ch1.GetNext(ch1.FindResidue(ResNum(3))).IsValid());    
  BOOST_CHECK(!ch1.GetPrev(ResidueHandle()).IsValid());
  BOOST_CHECK(!ch1.GetNext(ResidueHandle()).IsValid());
}

BOOST_AUTO_TEST_SUITE_END()