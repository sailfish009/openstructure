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
  MapOctree(const img::ImageHandle& map, bool wrap_around);
  explicit MapOctree(bool wrap_around);
  void Initialize();
  uint32_t GetNumNodesForLevel(uint8_t level) const;
  void SetNewMap(const img::ImageHandle& ih);
  void SetVisibleExtent(const img::Extent& extent) { 
    extent_=extent; 
  }
  const img::Extent& GetVisibleExtent() const { return extent_; }
  static bool IsMapManageable (const img::ImageHandle ih);
  
  /// \brief depth-first visit of octree nodes
  template <typename F>
  void VisitDF(F& f, bool ignore_extent=false) const
  {
    if (levels_.empty()) {
      return;
    }
    img::RealSpatialImageState* map=NULL;
    map=dynamic_cast<img::RealSpatialImageState*>(map_.ImageStatePtr().get());
    assert(levels_[0].size()==1);
    if (f.VisitNode(levels_[0][0], 0, real_extent_)) {
      this->VisitDFRec<F>(levels_[0][0], f, 1, real_extent_, map, 
                          ignore_extent);
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
    img::RealSpatialImageState* map=NULL;
    map=dynamic_cast<img::RealSpatialImageState*>(map_.ImageStatePtr().get());
    img::Extent extent=extent_;
    this->DispatchVisit(f, map, img::Point(0, 0, 0), extent, 0);
    extent_=extent;
  }
  
  const img::Extent& GetRealExtent() const { return real_extent_; }
  
  const img::Point& GetOffset() const { return offset_; }
protected:
  
  template <typename F>
  void DispatchVisit(F& func, img::RealSpatialImageState*, 
                     const img::Point& offset, 
                     const img::Extent& extent, int axis);
  
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
                  bool ignore_extent) const;
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
  bool                          wrap_around_;
  img::Extent                   real_extent_;
  img::Point                    offset_;
};


template <typename F>
void MapOctree::DispatchVisit(F& f, img::RealSpatialImageState* map, 
                              const img::Point& offset, 
                              const img::Extent& extent, int axis)
{
  img::Point start=extent.GetStart();
  img::Point end=extent.GetEnd();
  img::Extent map_extent=map->GetExtent();
  img::Size size=map_extent.GetSize();
  img::Point map_start=map_extent.GetStart();
  int start_off=0, start_mod=0;
  int end_off=0, end_mod=0;
  if (start[axis]-map_start[axis]<0) {
    std::div_t d=std::div(start[axis]-map_start[axis], 
                          static_cast<int>(size[axis]));
    start_off=d.rem ? d.quot-1 : d.quot;
    if (d.rem) {
      start_mod=size[axis]+d.rem;
    }

  } else {
    start_off=(start[axis]-map_start[axis])/static_cast<int>(size[axis]);
    start_mod=(start[axis]-map_start[axis]) % size[axis];
  }
  if (end[axis]-map_start[axis]<0) {
    std::div_t d=std::div(end[axis]-map_start[axis],
                          static_cast<int>(size[axis]));
    end_off=d.rem ? d.quot-1 : d.quot;
    if (d.rem) {
      end_mod=size[axis]+d.rem;
    }
  } else {
    end_off=(end[axis]-map_start[axis])/static_cast<int>(size[axis]);
    end_mod=(end[axis]-map_start[axis]) % size[axis];
  }
  if (start_off!=end_off) {
    // split into two parts
    img::Point new_offset_a=offset;
    new_offset_a[axis]=start_off*size[axis];
    img::Point extent_start=start;
    extent_start[axis]=start_mod+map_start[axis];
    img::Point extent_end=end;
    extent_end[axis]=map_extent.GetEnd()[axis];
    img::Extent extent_a=img::Extent(extent_start, extent_end);
    
    img::Point new_end=end;
    new_end[axis]=end_mod+map_start[axis];
    img::Point new_start_b=start;
    new_start_b[axis]=map_start[axis];
    img::Extent extent_b=img::Extent(new_start_b,
                                     new_end);
    img::Point new_offset_b=offset;
    new_offset_b[axis]=end_off*size[axis];
    if (axis<2) {
      this->DispatchVisit(f, map, new_offset_a, extent_a, axis+1);
      this->DispatchVisit(f, map, new_offset_b, extent_b, axis+1);
    } else {
      extent_=extent_a;
      offset_=new_offset_a;
      if (f.VisitNode(levels_[0][0], 0, real_extent_)) {
        this->VisitDFRec<F>(levels_[0][0], f, 1, real_extent_, map, false);
      }      
      extent_=extent_b;
      offset_=new_offset_b;
      if (f.VisitNode(levels_[0][0], 0, real_extent_)) {
        this->VisitDFRec<F>(levels_[0][0], f, 1, real_extent_, map, false);
      }
    }

  } else {
    // all is good. no reason to split
    img::Point new_start(start);
    new_start[axis]-=start_off*size[axis];
    img::Point new_end(end);
    new_end[axis]=end_mod;
    img::Extent new_extent(new_start, new_end);
    img::Point new_offset=offset;
    new_offset[axis]=start_off*size[axis];
    if (axis<2) {
      this->DispatchVisit(f, map, new_offset, new_extent, axis+1);
    } else {
      extent_=new_extent;
      offset_=new_offset;
      if (f.VisitNode(levels_[0][0], 0, real_extent_)) {
        this->VisitDFRec<F>(levels_[0][0], f, 1, real_extent_, map, false);
      }      
    }

  }
}


template <typename F>
void MapOctree::VisitDFRec(const OctreeNode& node, F& f, uint8_t level, 
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
        if (range_x[i][0]==range_x[i][1] && range_y[j][0]==range_y[j][1] && 
            range_z[k][0]==range_z[k][1]) {
              img::Point point(range_x[i][0], range_y[j][0], range_z[k][0]);
          if (ignore_extent || extent_.Contains(point)) {
            f.VisitLeaf(*this, map, point);
          }
         } else {
          const OctreeNode& cn=levels_[level][c];
          img::Extent ext(img::Point(range_x[i][0], range_y[j][0], 
                                     range_z[k][0]),
                           img::Point(range_x[i][1], range_y[j][1], 
                                      range_z[k][1]));
          if (ignore_extent || img::HasOverlap(extent_, ext)) {
            if (f.VisitNode(cn, level, ext)) {
              this->VisitDFRec<F>(cn, f, level+1, ext, map, ignore_extent);
            }
          }
        }
      }
    }    
  }    
}

}}}

#endif
