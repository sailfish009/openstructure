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

#include <ost/gfx/impl/map_octree.hh>

#include <ost/img/image_handle.hh>
#include <ost/img/image_factory.hh>

#define BOOST_TEST_DYN_LINK
#include <boost/test/unit_test.hpp>


using boost::unit_test_framework::test_suite;
using namespace ost;
using namespace ost::gfx::impl;

namespace {

struct Pow2Vis {
  Pow2Vis():
    leaf_count(0), node_count(0)
  { }
  bool VisitNode(const OctreeNode& node, uint8_t level, 
                 const img::Extent& ext)
  {
    BOOST_CHECK_EQUAL(level, 0);
    BOOST_CHECK_EQUAL(node.GetMax(), 64.0f);
    BOOST_CHECK_EQUAL(node.GetMin(), 0.5f);    
    node_count++;
    return true;
  }
  void VisitLeaf(const MapOctree& o, img::RealSpatialImageState* map, 
                 const img::Point& point) 
  {
    leaf_count++;
  }
  
  int leaf_count;
  int node_count;
};

struct LeafCounter {
  LeafCounter(): leaf_count(0) { }
  
  void VisitLeaf(const MapOctree& o, img::RealSpatialImageState* map, 
                 const img::Point& point) 
  {
    leaf_count++;
  }
  
  bool VisitNode(const OctreeNode& node, uint8_t level, 
                 const img::Extent& ext)
  {
    return true;
  }
  int leaf_count;
};

struct NonPow2Vis {
  NonPow2Vis():
    leaf_count(0), node_count(0)
  { }
  bool VisitNode(const OctreeNode& node, uint8_t level, 
                 const img::Extent& ext)
  {
    node_count++;    
    if (level==0) {
      BOOST_CHECK_EQUAL(node.GetMax(), 64.0f);
      BOOST_CHECK_EQUAL(node.GetMin(), 0.0f);      
      return true;
    } 
    if (level==1) {
      switch(node_count) {
        case 2:
          BOOST_CHECK_EQUAL(ext, img::Extent(img::Point(0,0,0),
                                              img::Point(3,1,1)));
          break;
        case 5:
          BOOST_CHECK_EQUAL(ext, img::Extent(img::Point(4,0,0),
                                              img::Point(4,1,1)));
          break;
        default:
          BOOST_FAIL("unexpected node");
      }
    }
    return true;
  }
  void VisitLeaf(const MapOctree& o, img::RealSpatialImageState* map, 
                 const img::Point& point) 
  {
    leaf_count++;
    // z-coordinate of pixels change fastest, followed by y and then x.
    switch (leaf_count) {
      case 1:
        BOOST_CHECK_EQUAL(point, img::Point(0,0,0));
        break;
      case 2:
        BOOST_CHECK_EQUAL(point, img::Point(0,0,1));
        break;
      case 3:
        BOOST_CHECK_EQUAL(point, img::Point(0,1,0));
        break;
      case 4:
        BOOST_CHECK_EQUAL(point, img::Point(0,1,1));
        break;
      case 5:
        BOOST_CHECK_EQUAL(point, img::Point(1,0,0));
        break;
      case 6:
        BOOST_CHECK_EQUAL(point, img::Point(1,0,1));
        break;
      case 7:
        BOOST_CHECK_EQUAL(point, img::Point(1,1,0));
        break;
      case 8:
        BOOST_CHECK_EQUAL(point, img::Point(1,1,1));
        break;
      case 9:
        BOOST_CHECK_EQUAL(point, img::Point(2,0,0));
        break;
      case 10:
        BOOST_CHECK_EQUAL(point, img::Point(2,0,1));
        break;
      case 11:
        BOOST_CHECK_EQUAL(point, img::Point(2,1,0));
        break;
      case 12:
        BOOST_CHECK_EQUAL(point, img::Point(2,1,1));
        break;
      case 13:
        BOOST_CHECK_EQUAL(point, img::Point(3,0,0));
        break;
      case 14:
        BOOST_CHECK_EQUAL(point, img::Point(3,0,1));
        break;
      case 15:
        BOOST_CHECK_EQUAL(point, img::Point(3,1,0));
        break;
      case 16:
        BOOST_CHECK_EQUAL(point, img::Point(3,1,1));
        break;
      case 17:
        BOOST_CHECK_EQUAL(point, img::Point(4,0,0));
        break;
      case 18:
        BOOST_CHECK_EQUAL(point, img::Point(4,0,1));
        break;
      case 19:
        BOOST_CHECK_EQUAL(point, img::Point(4,1,0));
        break;
      case 20:
        BOOST_CHECK_EQUAL(point, img::Point(4,1,1));
        break;
      default:
        BOOST_FAIL("should never get here");
    }
  }
  int leaf_count;
  int node_count;
};

}

BOOST_AUTO_TEST_SUITE(gfx)

BOOST_AUTO_TEST_CASE(octree_power_of_two) 
{
  img::ImageHandle img=img::CreateImage(img::Size(2, 2, 2));
  img.SetReal(img::Point(0, 0, 0), 1.0f);
  img.SetReal(img::Point(1, 0, 0), 2.0f);
  img.SetReal(img::Point(1, 1, 0), 4.0f);  
  img.SetReal(img::Point(1, 0, 1), 8.0f); 
  img.SetReal(img::Point(1, 1, 1), 16.0f);
  img.SetReal(img::Point(0, 1, 1), 32.0f);   
  img.SetReal(img::Point(0, 1, 0), 64.0f);
  img.SetReal(img::Point(0, 0, 1), 0.5f);
  Pow2Vis v;
  MapOctree octree(img, false);
  octree.Initialize();
  octree.VisitDF(v);  
  BOOST_CHECK_EQUAL(v.leaf_count, 8);
  BOOST_CHECK_EQUAL(v.node_count, 1);
}

BOOST_AUTO_TEST_CASE(octree_non_power_of_two)
{
  img::ImageHandle img=img::CreateImage(img::Size(5, 2, 2));
  img.SetReal(img::Point(0, 0, 0), 1.0f);
  img.SetReal(img::Point(1, 0, 0), 2.0f);
  img.SetReal(img::Point(1, 1, 0), 4.0f);  
  img.SetReal(img::Point(1, 0, 1), 8.0f); 
  img.SetReal(img::Point(1, 1, 1), 16.0f);
  img.SetReal(img::Point(0, 1, 1), 32.0f);   
  img.SetReal(img::Point(0, 1, 0), 64.0f);
  img.SetReal(img::Point(0, 0, 1), 0.5f);
  NonPow2Vis v;
  MapOctree octree(img, false);
  octree.Initialize();
  octree.VisitDF(v);  
}


BOOST_AUTO_TEST_CASE(octree_visit_df)
{
  for (int i=2;i<=7; ++i) {
    for (int j=2; j<=7; ++j) {
      for (int k=2; k<=7; ++k) {
        LeafCounter v;
        img::ImageHandle img=img::CreateImage(img::Size(i, j, k));
        MapOctree octree(img, false);
        octree.Initialize();
        octree.VisitDF(v, true);    
        BOOST_CHECK_MESSAGE(v.leaf_count==i*j*k, "leaf count for map of size " 
                            << i << "*" << j << "*" << k << " should be "
                            << (i*j*k) << ", not " << v.leaf_count);
      }
    }

  }
}


BOOST_AUTO_TEST_CASE(octree_visit_wrap_around_even)
{
  img::ImageHandle img=img::CreateImage(img::Size(4, 2, 2));
  LeafCounter v;
  MapOctree octree(img, true);

  octree.Initialize();
  BOOST_CHECK_EQUAL(octree.GetRealExtent(), 
                    img::Extent(img::Point(0, 0, 0), img::Point(4,2,2)));
  octree.VisitDFFullExtent(v);
  BOOST_CHECK_EQUAL(v.leaf_count, 16);
  v.leaf_count=0;
  octree.SetVisibleExtent(img::Extent(img::Point(1, 0, 0), 
                                      img::Point(4, 1, 1)));
  octree.VisitDFFullExtent(v);
  BOOST_CHECK_EQUAL(v.leaf_count, 16);
  v.leaf_count=0;

  octree.SetVisibleExtent(img::Extent(img::Point(2, 0, 0), 
                                      img::Point(5, 1, 1)));
  octree.VisitDFFullExtent(v);
  BOOST_CHECK_EQUAL(v.leaf_count, 16);
  v.leaf_count=0;
  
  octree.SetVisibleExtent(img::Extent(img::Point(3, 0, 0), 
                                      img::Point(6, 1, 1)));
  octree.VisitDFFullExtent(v);
  BOOST_CHECK_EQUAL(v.leaf_count, 16);
  v.leaf_count=0;
  
  octree.SetVisibleExtent(img::Extent(img::Point(4, 0, 0), 
                                      img::Point(7, 1, 1)));
  octree.VisitDFFullExtent(v);
  BOOST_CHECK_EQUAL(v.leaf_count, 16);
  v.leaf_count=0;
  
  octree.SetVisibleExtent(img::Extent(img::Point(-8, 0, 0), 
                                      img::Point(-5, 1, 1)));
  octree.VisitDFFullExtent(v);
  BOOST_CHECK_EQUAL(v.leaf_count, 16);
  v.leaf_count=0;
}


BOOST_AUTO_TEST_CASE(octree_visit_wrap_around_odd)
{
  img::ImageHandle img=img::CreateImage(img::Size(5, 5, 5));
  LeafCounter v;
  MapOctree octree(img, true);
  octree.Initialize();
  BOOST_CHECK_EQUAL(octree.GetRealExtent(), 
                    img::Extent(img::Point(0, 0, 0), img::Point(5, 5, 5)));
  octree.VisitDFFullExtent(v);
  BOOST_CHECK_EQUAL(v.leaf_count, 125);
  v.leaf_count=0;
  octree.SetVisibleExtent(img::Extent(img::Point(1, 1, 1), 
                                      img::Point(5, 5, 5)));
  octree.VisitDFFullExtent(v);
  BOOST_CHECK_EQUAL(v.leaf_count, 125);
  v.leaf_count=0;

  octree.SetVisibleExtent(img::Extent(img::Point(2, 2, 2), 
                                      img::Point(6, 6, 6)));
  octree.VisitDFFullExtent(v);
  BOOST_CHECK_EQUAL(v.leaf_count, 125);
  v.leaf_count=0;
  
  octree.SetVisibleExtent(img::Extent(img::Point(3, 3, 3), 
                                      img::Point(7, 7, 7)));
  octree.VisitDFFullExtent(v);
  BOOST_CHECK_EQUAL(v.leaf_count, 125);
  v.leaf_count=0;
  
  octree.SetVisibleExtent(img::Extent(img::Point(4, 4, 4), 
                                      img::Point(8, 8, 8)));
  octree.VisitDFFullExtent(v);
  BOOST_CHECK_EQUAL(v.leaf_count, 125);
  v.leaf_count=0;
  
  octree.SetVisibleExtent(img::Extent(img::Point(-5, -5, -5), 
                                      img::Point(-1, -1, -1)));
  octree.VisitDFFullExtent(v);
  BOOST_CHECK_EQUAL(v.leaf_count, 125);
  v.leaf_count=0;
}
BOOST_AUTO_TEST_SUITE_END()
