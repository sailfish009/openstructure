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
#include <ost/dyn_cast.hh>
#include "xtal_map.hh"

namespace ost { namespace img {



XtalExtentIterator::XtalExtentIterator(const Extent& b, const XtalMapPtr& map):
  start_(b.GetStart()), end_(b.GetEnd()+Point(1,1,1)), 
  current_(start_), map_(map) 
{
  map_->FindSym(current_, sym_, asu_pos_);
}

void XtalExtentIterator::Inc()
{
  ++current_[0];
  asu_pos_+=map_->symdeltas_[sym_].du;
  if (!map_->asu_ext_.Contains(asu_pos_) || 
      map_->asu_[map_->asu_ext_.Point2Offset(asu_pos_)]!=0) {
    map_->FindSym(current_, sym_, asu_pos_);
  }
  if (current_[0]>=end_[0]) {
    current_[0]=start_[0];
    if(++current_[1]>=end_[1]) {
      current_[1]=start_[1];
      if(++current_[2]>=end_[2]) {
        current_=end_;
      }
    } 
    map_->FindSym(current_, sym_, asu_pos_);
  }
}

Real XtalExtentIterator::GetReal() const
{
  assert(map_->asu_[  map_->asu_ext_.Point2Offset(asu_pos_)]==0);
  return map_->map_->Value(asu_pos_);
}

void XtalExtentIterator::SetReal(Real value)
{
  assert(map_->asu_[  map_->asu_ext_.Point2Offset(asu_pos_)]==0);
  map_->map_->Value(asu_pos_)=value;
}

void XtalMap::Init()
{
  asu_ext_=map_->GetExtent();
  SymmetryPtr sym=unit_cell_.GetSymmetry();
  symops_.clear();
  symdeltas_.clear();
  symops_.reserve(sym->GetSymops().size());
  symdeltas_.reserve(sym->GetSymops().size());
  for (SymopList::const_iterator i=sym->GetSymops().begin(), 
       e=sym->GetSymops().end(); i!=e; ++i) {
    symops_.push_back(ISymop(*i, uc_ext_.GetSize()));
    symdeltas_.push_back(SymDelta(Point(symops_.back().Rot()(0,0),
                                        symops_.back().Rot()(1,0),
                                        symops_.back().Rot()(2,0)),
                                  Point(symops_.back().Rot()(0,1),
                                        symops_.back().Rot()(1,1),
                                        symops_.back().Rot()(2,1)),
                                  Point(symops_.back().Rot()(0,2),
                                        symops_.back().Rot()(1,2),
                                        symops_.back().Rot()(2,2))));
  }
  // For speed purposes, we now flag all voxels that are part of the assymetric 
  // unit. For simple symmetries, all voxels in map_ will be part of the asu. 
  // however, for more complex symmetries, part of the asu map will be symmetry 
  // related to another voxel in the asu map. They will be flagged with 
  // symopnumber+1
  asu_.clear();
  asu_.resize(asu_ext_.GetVolume(), 255);
  int count=0;
  for (ExtentIterator i(asu_ext_); !i.AtEnd(); ++i) {
    uint index=asu_ext_.Point2Offset(i);
    size_t j;
    for (j=1; j<symops_.size(); ++j) {
      Point p=this->ToUnitCell(symops_[j].Transform(p));
      if (!asu_ext_.Contains(p)) {
        continue;
      }
      if (asu_[asu_ext_.Point2Offset(i)]==0) break;
    }
    if (j==symops_.size()) {
      count+=1;
      asu_[index]=0;
    }
  }
  for (ExtentIterator i(asu_ext_); !i.AtEnd(); ++i) {
    uint index=asu_ext_.Point2Offset(i);
    if (asu_[index]!=255) {
      continue;
    }
    size_t j;
    for (j=0; j<symops_.size(); ++j) {
      Point p=this->ToUnitCell(symops_[j].Transform(p));
      if (!asu_ext_.Contains(p)) {
        continue;
      }
      if (asu_[asu_ext_.Point2Offset(i)]==0) break;
    }
    asu_[index]=1+j;
  }
}


void XtalMap::FindSym(const Point& p, int& sym, Point& wp) const
{
  uint index;
  wp=this->ToUnitCell(p);
  if (asu_ext_.Contains(wp)) {
    index=asu_ext_.Point2Offset(wp);
    if (asu_[index]==0) {
      sym=0;
      return;
    }
    sym=asu_[index]-1;
    index=asu_ext_.Point2Offset(symops_[sym].Transform(wp));
    return;
  }
  for (sym=1; sym<static_cast<int>(symops_.size()); ++sym) {
    wp=symops_[sym].Transform(p);
    wp=this->ToUnitCell(wp);
    if (!asu_ext_.Contains(wp)) {
      continue;
    }
    index=asu_ext_.Point2Offset(wp);
    if (asu_[index]==0) {
      return;
    }
  }
  LOG_ERROR("XtalMap::FindSym Internal Error: Could not find symmetry "
            "equivalent for " << p);
}


ImageHandle XtalMap::Extract(const Extent& extent)
{
  ImageHandle ex=CreateImage(extent);
  ex.SetSpatialSamplingMat(map_->GetSampling().GetSpatialSamplingMat());
  ex.SetAbsoluteOrigin(map_->GetAbsoluteOrigin());
  ImageStateBasePtr base_ptr=ex.ImageStatePtr();
  RealSpatialImageStatePtr ex_state=dyn_cast<RealSpatialImageState>(base_ptr);
  XtalExtentIterator iter=this->Iter(extent);
  for (; !iter.AtEnd(); ++iter) {
    ex_state->Value(iter)=iter.GetReal();
  }
  return ex;
}

}}
