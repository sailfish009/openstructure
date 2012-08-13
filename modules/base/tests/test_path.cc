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
  Author: Valerio Mariani
*/
#define BOOST_TEST_DYN_LINK
#include <boost/test/unit_test.hpp>
#include <boost/test/auto_unit_test.hpp>
#include <ost/path.hh>
#include <sys/stat.h>
#include <iostream>
#include <fstream>
#include <stdio.h>

using namespace ost;

BOOST_AUTO_TEST_SUITE( base );


BOOST_AUTO_TEST_CASE(test_constructors_and_implicit_conversion)
{
  Path test_path("path");
  String impl_conv(test_path);
  BOOST_CHECK(impl_conv=="path");
}

BOOST_AUTO_TEST_CASE(test_operators)
{
  String fullpath_base("this");
  fullpath_base += OST_DIRECTORY_SEPARATOR;
  fullpath_base += "is";
  String fullpath_end("test");
  fullpath_end += OST_DIRECTORY_SEPARATOR;
  fullpath_end += "path";
  String full_path1 = fullpath_base;
  full_path1 += OST_DIRECTORY_SEPARATOR;
  full_path1 += "a";
  full_path1 += OST_DIRECTORY_SEPARATOR;
  full_path1 += fullpath_end;
  String full_path2 = fullpath_base;
  full_path2 += OST_DIRECTORY_SEPARATOR;
  full_path2 += "another";
  full_path2 += OST_DIRECTORY_SEPARATOR;
  full_path2 += fullpath_end;
  Path test_path1("this");
  Path test_path1r = test_path1 / "is" / "a" / "test" / "path";
  BOOST_CHECK(test_path1r.GetFullPath()==full_path1);
  test_path1 /= "is";
  test_path1 /= "a";
  test_path1 /= "test";
  test_path1 /= "path";
  BOOST_CHECK(test_path1.GetFullPath()==full_path1);
  Path test_path2b("is");
  Path test_path2c("another");
  Path test_path2d("test");
  Path test_path2e("path");
  Path test_path2("this");
  Path test_path2r = test_path2 / test_path2b / test_path2c / test_path2d / test_path2e;
  BOOST_CHECK(test_path2r.GetFullPath()==full_path2);
  test_path2 /= test_path2b /= test_path2c /= test_path2d /= test_path2e;
  BOOST_CHECK(test_path2.GetFullPath()==full_path2);
}

BOOST_AUTO_TEST_CASE(test_path_disassembling_operators)
{
  Path test_path = RootPath();
  test_path /= "etc";
  test_path /= "rc.d";
  test_path /= "rc.conf";
  String dirname("");
  dirname += OST_DIRECTORY_SEPARATOR;
  dirname += "etc";
  dirname += OST_DIRECTORY_SEPARATOR;
  dirname += "rc.d";
  dirname += OST_DIRECTORY_SEPARATOR;
  BOOST_CHECK(test_path.GetDirName()==dirname);
  BOOST_CHECK(test_path.GetFileName()=="rc.conf");
  BOOST_CHECK(test_path.GetExtension()=="conf");
  Path test_path2("etc");
  test_path2 /= "rc.d";
  test_path2 /= "";
  String dirname2("etc");
  dirname2 += OST_DIRECTORY_SEPARATOR;
  dirname2 += "rc.d";
  dirname2 += OST_DIRECTORY_SEPARATOR;
  BOOST_CHECK(test_path2.GetDirName()==dirname2);
  BOOST_CHECK(test_path2.GetFileName()=="");
  BOOST_CHECK(test_path2.GetExtension()=="");
}


BOOST_AUTO_TEST_CASE(test_existence_and_accessibility)
{
  std::ofstream filestr;
  Path filename("testfiles");
  filename /= "path_test_file.txt";
  Path test_path("testfiles");
  test_path /= "path_test_file.txt";
  std::cout << filename.GetFullPath() << std::endl;
  filestr.open(filename.GetFullPath().c_str());
  filestr << "This file is created by the unitest for the Path class" << std::endl;
  filestr.close();
  BOOST_CHECK(test_path.Exists()==true);
  BOOST_CHECK(test_path.IsWritable()==true);

  #ifdef WIN32
  // Put windows code here
  #else
  chmod(test_path.GetFullPath().c_str(),S_IRUSR | S_IRGRP | S_IROTH);
  BOOST_CHECK(test_path.IsWritable()==false);
  chmod(test_path.GetFullPath().c_str(),S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
  remove(test_path.GetFullPath().c_str());
  BOOST_CHECK(test_path.Exists()==false);

  #endif
}

BOOST_AUTO_TEST_SUITE_END();
