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
#ifndef OST_GFX_IMPL_MAP_OCTREE_HH
#define OST_GFX_IMPL_MAP_OCTREE_HH

#include <ost/img/image_handle.hh>
#include <ost/img/image_state.hh>
#include <ost/gfx/module_config.hh>
#include <ost/stdint.hh>
/*
  Author: Marco Biasini
 */
namespace ost { namespace gfx { namespace impl {

typedef uint16_t OcRange;

struct OcRangeVector {
  OcRangeVector(uint16_t px, uint16_t py, uint16_t pz):
    x(px), y(py), z(pz)
  { }
  OcRange x;
  OcRange y;
  OcRange z; 
}; 

/// Octree node are stored in a one-dimensional array. The storage scheme is 
/// carefully choses such that child nodes are continuous in memory. This 
/// reduces the storage size of every OctreeNode considerably as we only have 
/// to store the index of the first child node instead of eight indices.
class OctreeNode {
public:
  OctreeNode(uint32_t first_child, bool branch_x, bool branch_y, bool branch_z):
    first_child_(first_child), branch_x_(branch_x), 
    branch_y_(branch_y), branch_z_(branch_z), is_leaf_(false), 
    min_(0.0), max_(0.0)
  { }
  OctreeNode(): first_child_(0), 
     branch_x_(false), branch_y_(false), branch_z_(false), is_leaf_(false),
     min_(0.0), max_(0.0)
  { }
  /// \brief whether the node branches in x direction
  bool BranchInX() const { return branch_x_; }
  /// \brief whether the node branches in y direction  
  bool BranchInY() const { return branch_y_; }  
  /// \brief whether the node branches in z direction  
  bool BranchInZ() const { return branch_z_; }
  
  /// \brief get number of children
  uint8_t GetChildCount() const { 
    uint8_t sum=(uint8_t(branch_x_)+uint8_t(branch_y_)+uint8_t(branch_z_));
    switch (sum) {
      case 0:
        return 0;
      case 1:
        return 2;
      case 2:
        return 4;
      case 3:
        return 8;
      default:
        assert(0 && "what the heck?");
    }
    return 0;
  }
  uint32_t GetFirstChild() const {
    return first_child_;
  }
  float GetMin() const { return min_; }
  float GetMax() const { return max_; }
  bool IsLeaf() const { return is_leaf_; }
  void SetLeaf() { is_leaf_=true; }
  void SetMin(float m) { min_=m; }
  void SetMax(float m) { max_=m; }  
private:
  uint32_t first_child_ : 28; // < index of left-most child
  bool     branch_x_    : 1;  // < whether to branch in x
  bool     branch_y_    : 1;  // < whether to branch in y
  bool     branch_z_    : 1;  // < whether to branch in z
  bool     is_leaf_     : 1;  // < whether the child nodes are leafs
  float    min_;              // < minimum value of voxels inside OctreeNode
  float    max_;              // < maximum value of voxels inside OctreeNode
};

typedef std::vector<OctreeNode> OcNodeEntryList;


/// \brief Octree datastructure for 3D images
/// 
/// This class implements a branch-on-need octree (BONO) which is particularly 
/// well suited for 3D images as they do not impose the typical power-of-two 
/// restriction on the map size. Each node in the tree stores the minimum and 
/// maximum value of the voxels it encloses.
class DLLEXPORT_OST_GFX MapOctree {
public:
  MapOctree(const img::ImageHandle& map);
  void Initialize();
  uint32_t GetNumNodesForLevel(uint8_t level) const;
  void SetNewMap(const img::ImageHandle& ih);
  void SetExtent(const img::Extent& extent) { 
    extent_=extent; 
  }
  const img::Extent& GetExtent() const { return extent_; }
  static bool IsMapManageable (const img::ImageHandle ih);
  
  /// \brief depth-first visit of octree nodes
  template <typename F>
  void VisitDF(F& f, bool ignore_extent=false) const
  {
    if (levels_.empty()) {
      return;
    }
    img::Extent ext=map_.GetExtent();
    img::RealSpatialImageState* map=NULL;
    map=dynamic_cast<img::RealSpatialImageState*>(map_.ImageStatePtr().get());
    assert(levels_[0].size()==1);
    if (f.VisitNode(levels_[0][0], 0, ext)) {
      this->VisitDFRec<F>(levels_[0][0], f, 1, ext, map, ignore_extent);
    }
  }
  
  /// \brief wrapper around VisitDF to work with with maps that have apparent 
  ///    infinite extensions, such as density maps coming from X-ray 
  ///    crystallography. We basically split the visit into several runs 
  ///    (up to two in each dimension)
  // desperately in need of a better name!
  template <typename F>
  void VisitDFFullExtent(F& f)
  {
    if (levels_.empty()) {
      return;
    }
    img::Extent map_extent=map_.GetExtent();
    img::RealSpatialImageState* map=NULL;
    map=dynamic_cast<img::RealSpatialImageState*>(map_.ImageStatePtr().get());
    img::Point start=extent_.GetStart();
    img::Point end=extent_.GetEnd();
    img::Extent extent=extent_;
    img::Size size=map_extent.GetSize();
    int start_x_off=start[0]/static_cast<int>(size[0]);
    int end_x_off=end[0]/static_cast<int>(size[0]);
    std::cout << extent << " " << map_extent << " " << start_x_off << " " << end_x_off << std::endl;
    if (start_x_off!=end_x_off) {
      // determine where to split:
      f.SetOffset(img::Point(start_x_off*size[0], 0, 0));
      img::Extent x_extent_a=img::Extent(img::Point(start[0]-start_x_off*size[0], start[1], start[2]),
                                         map_extent.GetEnd());
      extent_=x_extent_a;
      if (f.VisitNode(levels_[0][0], 0, map_extent)) {
        this->VisitDFRec<F>(levels_[0][0], f, 1, map_extent, map, false);
      }
      int true_end=end[0]-end_x_off*size[0];
      if (true_end<0) {
        true_end=size[0]-true_end;git st
      }
      std::cout << true_end << std::endl;
      img::Point new_end=img::Point(true_end, end[1], end[2]);
      img::Extent x_extent_b=img::Extent(img::Point(0, 0, 0),
                                         new_end);                                         
      //std::cout << "XB: " << x_extent_b << " "  << start_x_off << " " << end_x_off << std::endl;
      extent_=x_extent_b;
      f.SetOffset(img::Point(end_x_off*size[0], 0, 0));
      if (f.VisitNode(levels_[0][0], 0, map_extent)) {
        this->VisitDFRec<F>(levels_[0][0], f, 1, map_extent, map, false);
      }
    } else {
      img::Extent x_extent(img::Point(start[0]-start_x_off*size[0], start[1], start[2]),
                           img::Point(end[0]-end_x_off*size[0], end[1], end[2]));
      extent_=x_extent;
      f.SetOffset(img::Point(start_x_off*size[0], 0, 0));
      if (f.VisitNode(levels_[0][0], 0, map_extent)) {
        this->VisitDFRec<F>(levels_[0][0], f, 1, map_extent, map, false);
      }
    }
    assert(levels_[0].size()==1);
    extent_=extent;
  }
protected:
  static inline int LastSetBit(uint16_t ch)
  {
    for (char i=15; i>=0; --i) {
      uint16_t m=1<<i;
      if (ch & m) {
        return i;
      }
    }
    return -1;
  }

  template <typename F>
  void VisitDFRec(const OctreeNode& node, F& f, uint8_t level, 
                  const img::Extent& ext, 
                  img::RealSpatialImageState* map,
                  bool ignore_extent) const
  {
    if (node.GetChildCount()==0) { return; }
    uint32_t c=node.GetFirstChild();    
    int cx=1+int(node.BranchInX());
    int cy=1+int(node.BranchInY());
    int cz=1+int(node.BranchInZ());
    int range_x[2][2];
    int range_y[2][2];
    int range_z[2][2];
    // update ranges. this is basically the same code as in BuildOctreeRec()
    if (node.BranchInX()) {
      uint16_t ems=ext.GetEnd()[0]-ext.GetStart()[0];
      uint16_t bit_mask=1<<(LastSetBit(ems));
      range_x[0][0]=ext.GetStart()[0];
      range_x[0][1]=range_x[0][0]+bit_mask-1;
      range_x[1][0]=ext.GetEnd()[0]-(ems & ~bit_mask);
      range_x[1][1]=ext.GetEnd()[0];
    } else {
      range_x[0][0]=ext.GetStart()[0];
      range_x[0][1]=ext.GetEnd()[0];
    }
    if (node.BranchInY()) {
      uint16_t ems=ext.GetEnd()[1]-ext.GetStart()[1];
      uint16_t bit_mask=1<<(LastSetBit(ems));
      range_y[0][0]=ext.GetStart()[1];
      range_y[0][1]=range_y[0][0]+bit_mask-1;
      range_y[1][0]=ext.GetEnd()[1]-(ems & ~bit_mask);
      range_y[1][1]=ext.GetEnd()[1];
    } else {
      range_y[0][0]=ext.GetStart()[1];
      range_y[0][1]=ext.GetEnd()[1];
    }
    if (node.BranchInZ()) {
      uint16_t ems=ext.GetEnd()[2]-ext.GetStart()[2];
      uint16_t bit_mask=1<<(LastSetBit(ems));
      range_z[0][0]=ext.GetStart()[2];
      range_z[0][1]=range_z[0][0]+bit_mask-1;
      range_z[1][0]=ext.GetEnd()[2]-(ems & ~bit_mask);
      range_z[1][1]=ext.GetEnd()[2];
    } else {
      range_z[0][0]=ext.GetStart()[2];
      range_z[0][1]=ext.GetEnd()[2];
    }    
    for (int i=0; i<cx; ++i) {
      for (int j=0; j<cy; ++j) {
        for (int k=0; k<cz; ++k, ++c) {
          if (node.IsLeaf()) {
            f.VisitLeaf(map, img::Point(range_x[i][0], range_y[j][0], 
                                         range_z[k][0]));
           } else {
            const OctreeNode& cn=levels_[level][c];
            img::Extent ext(img::Point(range_x[i][0], range_y[j][0], 
                                         range_z[k][0]),
                             img::Point(range_x[i][1], range_y[j][1], 
                                         range_z[k][1]));
            if (ignore_extent || img::HasOverlap(ext, extent_)) {
              if (f.VisitNode(cn, level, ext)) {
                this->VisitDFRec<F>(cn, f, level+1, ext, map, ignore_extent);
              }
            }
          }
        }
      }    
    }    
  }
private:
  void BuildOctree();
  std::pair<float, float> BuildOctreeRec(const OcRangeVector& range_vec,
                                         uint16_t level,
                                         img::RealSpatialImageState* map,
                                         const img::Extent& ext,
                                         OctreeNode& parent);

  img::ImageHandle              map_;
  std::vector<OcNodeEntryList>  levels_;
  img::Extent                   extent_;
  bool                          built_;
};

}}}

#endif
