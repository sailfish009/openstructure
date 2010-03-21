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
  Author: Marco Biasini
 */

#include <fstream>
#include <boost/test/unit_test.hpp>
#include <ost/io/load_entity.hh>
#include <ost/gfx/scene.hh>
#include <ost/gfx/entity.hh>

using boost::unit_test_framework::test_suite;
using namespace ost;
using namespace ost::gfx;

namespace {

boost::shared_ptr<Entity> prepare_object(gfx::RenderMode::Type mode)
{
  mol::EntityHandle ent=io::LoadEntity("testfiles/helix.pdb");
  boost::shared_ptr<Entity> gfx_ent(new Entity("Helix", mode, ent));
  Scene::Instance().Add(gfx_ent);
  Scene::Instance().CenterOn(gfx_ent);
  return gfx_ent;
}

bool compare_files(const String& test, const String& gold_standard)
{
  std::ifstream test_stream(test.c_str());
  std::ifstream gold_stream(gold_standard.c_str());
  String test_line, gold_line;
  while (true) {
    bool test_end=std::getline(test_stream, test_line);
    bool gold_end=std::getline(gold_stream, gold_line);
    if (!(test_end || gold_end)) {
      return true;
    }
    if (!test_end) {
      std::cerr << gold_standard << " contains additional line(s):"
                << std::endl << gold_line;
      return false;
    }
    if (!gold_end) {
      std::cerr << test << " contains additional line(s):"
                << std::endl << test_line;
      return false;
    }
    if (gold_line!=test_line) {
      std::cerr << "line mismatch:" << std::endl << "test: " << test_line 
                << std::endl << "gold: " << gold_line;
      return false;
    }
  }
  return true;
}

bool compare_sphere_cyl_entries(const String& test, 
                                const String& gold_standard)
{
  std::vector<String> gold_spheres;
  std::vector<String> test_spheres;
  std::vector<String> test_cyls;
  std::vector<String> gold_cyls;
  std::ifstream test_stream(test.c_str());
  std::ifstream gold_stream(gold_standard.c_str());
  String test_line, gold_line;
  while (true) {
    bool test_end=std::getline(test_stream, test_line);
    bool gold_end=std::getline(gold_stream, gold_line);
    if (!(test_end && gold_end)) {
      break;
    }
    if (gold_line.find(" sphere")==0) {
      gold_spheres.push_back(gold_line);
    } else if (gold_line.find(" cylinder")==0) {
      gold_cyls.push_back(gold_line);
    }
    if (test_line.find(" sphere")==0) {
      test_spheres.push_back(test_line);
    } else if (gold_line.find(" cylinder")==0) {
      test_cyls.push_back(test_line);
    }
  }

  BOOST_CHECK_EQUAL(gold_spheres.size(), test_spheres.size());
  BOOST_CHECK_EQUAL(test_cyls.size(), gold_cyls.size());

  if (gold_spheres.size()!=test_spheres.size()) {
    return false;
  }
  if (gold_cyls.size()!=test_cyls.size()) {
    return false;
  }
  std::sort(gold_spheres.begin(), gold_spheres.end());
  std::sort(gold_cyls.begin(), gold_cyls.end());
  std::sort(test_spheres.begin(), test_spheres.end());
  std::sort(test_cyls.begin(), test_cyls.end());  
  for (size_t i=0;i<gold_cyls.size(); ++i) {
    BOOST_CHECK_EQUAL(gold_cyls[i], test_cyls[i]);
  }
  for (size_t i=0;i<gold_spheres.size(); ++i) {
    BOOST_CHECK_EQUAL(gold_spheres[i], test_spheres[i]);
  }
  return true;
}

}
BOOST_AUTO_TEST_SUITE(gfx)

BOOST_AUTO_TEST_CASE(pov_export_simple)
{
  Scene::Instance().SetOffscreenMode();
  boost::shared_ptr<Entity> obj=prepare_object(RenderMode::SIMPLE);    
  Scene::Instance().ExportPov("pov_simple_test", ".");
  compare_sphere_cyl_entries("pov_simple_test.inc", 
                             "testfiles/pov_simple_std.inc");
  Scene::Instance().Remove(obj);
}

BOOST_AUTO_TEST_CASE(pov_export_custom)
{
  Scene::Instance().SetOffscreenMode();
  boost::shared_ptr<Entity> obj=prepare_object(RenderMode::CUSTOM);
  Scene::Instance().ExportPov("pov_custom_test", ".");
  compare_sphere_cyl_entries("pov_custom_test.inc", 
                            "testfiles/pov_custom_std.inc");
  Scene::Instance().Remove(obj);
}

BOOST_AUTO_TEST_CASE(pov_export_cartoon)
{
  Scene::Instance().SetOffscreenMode();
  boost::shared_ptr<Entity> obj=prepare_object(RenderMode::HSC);
  Scene::Instance().ExportPov("pov_cartoon_test", ".");
  BOOST_CHECK(compare_files("pov_cartoon_test.inc", 
                            "testfiles/pov_cartoon_std.inc"));
  Scene::Instance().Remove(obj);
}

BOOST_AUTO_TEST_CASE(pov_export_cpk)
{
  Scene::Instance().SetOffscreenMode();
  boost::shared_ptr<Entity> obj=prepare_object(RenderMode::CPK);
  Scene::Instance().ExportPov("pov_cpk_test", ".");
  compare_sphere_cyl_entries("pov_cpk_test.inc", 
                            "testfiles/pov_cpk_std.inc");
  Scene::Instance().Remove(obj);
}

BOOST_AUTO_TEST_CASE(pov_export_trace)
{
  Scene::Instance().SetOffscreenMode();
  boost::shared_ptr<Entity> obj=prepare_object(RenderMode::TRACE);
  Scene::Instance().ExportPov("pov_trace_test", ".");
  BOOST_CHECK(compare_files("pov_trace_test.inc", 
                            "testfiles/pov_trace_std.inc"));
  Scene::Instance().Remove(obj);
}

BOOST_AUTO_TEST_CASE(pov_export_line_trace)
{
  Scene::Instance().SetOffscreenMode();
  boost::shared_ptr<Entity> obj=prepare_object(RenderMode::LINE_TRACE);
  Scene::Instance().ExportPov("pov_line_trace_test", ".");
  BOOST_CHECK(compare_files("pov_line_trace_test.inc", 
                            "testfiles/pov_line_trace_std.inc"));
  Scene::Instance().Remove(obj);
}

BOOST_AUTO_TEST_CASE(pov_export_sline)
{
  Scene::Instance().SetOffscreenMode();
  boost::shared_ptr<Entity> obj=prepare_object(RenderMode::SLINE);
  Scene::Instance().ExportPov("pov_sline_test", ".");
  BOOST_CHECK(compare_files("pov_sline_test.inc", 
                            "testfiles/pov_sline_std.inc"));
  Scene::Instance().Remove(obj);
}

BOOST_AUTO_TEST_SUITE_END()
