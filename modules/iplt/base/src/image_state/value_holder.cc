//------------------------------------------------------------------------------
// This file is part of the OpenStructure project <www.openstructure.org>
//
// Copyright (C) 2008-2010 by the OpenStructure authors
// Copyright (C) 2003-2010 by the IPLT authors
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
  value holder for image states

  Author: Ansgar Philippsen
*/

#ifndef IPLT_IMAGE_STATE_INST_H
#error "value_holder.cc must be included from image_state_inst.hh"
#endif

#include <cassert>
#include <algorithm>

#include <ost/iplt/size.hh>

#include "value_holder.hh"
#include "index.hh"

#define USE_ROW_ORDER 1

namespace ost { namespace iplt { namespace image_state {

namespace {

// helper function
template <class V>
V* ValueCopy(const V* src, size_t volume_)
{
  V* dest = new V[volume_]; // if this throws, nothing has happened yet
  try {
    std::copy(src,src+volume_,dest);
  } catch (...) {
    delete []dest; // remove already allocated data
    throw;
  }
  return dest;
}

template <typename V>
DataType GetValueType();

template <>
DataType GetValueType<Complex>() {return COMPLEX;}

template <typename V>
DataType GetValueType() {return REAL;}

}

template <typename V>
ValueHolder<V>::ValueHolder(unsigned int wi, unsigned int he, unsigned int de):
  width_( (wi<1) ? 1 : wi), 
  height_( (he<1) ? 1: he ),
  depth_( (de<1) ? 1: de ),
  volume_(width_*height_*depth_),
  volume2_(volume_),
  data_(0),
  row_(0),
  slice_(0)
{
  try {
    data_ = new V[volume2_];
    setup();
    clear();
  } catch(...) {
    delete []data_;
  }
}

template <typename V>
ValueHolder<V>::ValueHolder(const Size& s):
  width_(s[0]),
  height_(s[1]),
  depth_(s[2]),
  volume_(width_*height_*depth_),
  volume2_(volume_),
  data_(0),
  row_(0),
  slice_(0)
{
  try {
    data_ = new V[volume2_];
    setup();
    clear();
  } catch(...) {
    delete []data_;
  }
}

template <typename V>
ValueHolder<V>::ValueHolder(const Size& s, const Size& ps):
  width_(s[0]),
  height_(s[1]),
  depth_(s[2]),
  volume_(width_*height_*depth_),
  volume2_(ps.GetVolume()),
  data_(0),
  row_(0),
  slice_(0)
{
  try {
    data_ = new V[volume2_];
    setup();
    clear();
  } catch(...) {
    delete []data_;
  }
}

template <typename V>
ValueHolder<V>::ValueHolder(const ValueHolder<V>& h):
  width_(h.width_),
  height_(h.height_),
  depth_(h.depth_),
  volume_(h.volume_),
  volume2_(h.volume2_),
  data_(ValueCopy(h.data_,h.volume2_)), // exception safe call
  row_(0),
  slice_(0)
{
  try {
    setup();
  } catch(...) {
    delete []data_;
  }
}

template <typename V>
ValueHolder<V>& ValueHolder<V>::operator=(const ValueHolder<V>& h) 
{
  if(this!=&h) {
    delete []data_;
    delete []row_;
    delete []slice_;
    data_ = ValueCopy(h.data_, h.volume2_);
    width_=h.width_;
    height_=h.height_;
    depth_=h.depth_;
    volume_=h.volume_;
    volume2_=h.volume2_;
    try {
      setup();
    } catch (...) {
      throw;
    }
  }
  return *this;
}

template <typename V>
ValueHolder<V>::~ValueHolder()
{
  delete []slice_;
  delete []row_;
  delete []data_;
}

template <typename V>
void ValueHolder<V>::Swap(ValueHolder& vh)
{
  std::swap<>(width_,vh.width_);
  std::swap<>(height_,vh.height_);
  std::swap<>(depth_,vh.depth_);
  std::swap<>(volume_,vh.volume_);
  std::swap<>(volume2_,vh.volume2_);
  std::swap<>(data_,vh.data_);
  std::swap<>(row_,vh.row_);
  std::swap<>(slice_,vh.slice_);
}

template <typename V>
V* ValueHolder<V>::ReleaseData()
{
  V* ret = data_; data_=0;
  delete []slice_; slice_=0;
  delete []row_; row_=0;
  return ret;
}

template <typename V>
Size ValueHolder<V>::GetSize() const
{
  return Size(width_,depth_,height_);
}

template <typename V>
V& ValueHolder<V>::Value(const Index& i)
{
  assert(i.u<width_ && i.v<height_ && i.w<depth_);
#ifdef USE_ROW_ORDER
  //return ((slice_[i.u])[i.v])[i.w];
  return data_[i.u*height_*depth_ + i.v*depth_ +i.w];
#else
  //return ((slice_[i.w])[i.v])[i.u];
  return data_[i.w*width_*height_ + i.v*height_ +i.u];
#endif
}

template <typename V>
const V& ValueHolder<V>::Value(const Index& i) const
{
  assert(i.u<width_ && i.v<height_ && i.w<depth_);
#ifdef USE_ROW_ORDER
  //return ((slice_[i.u])[i.v])[i.w];
  return data_[i.u*height_*depth_ + i.v*depth_ +i.w];
#else
  //return ((slice_[i.w])[i.v])[i.u];
  return data_[i.w*width_*height_ + i.v*height_ +i.u];
#endif
}

template <typename V>
V& ValueHolder<V>::Value(unsigned int i) 
{
  assert(i<volume_);
  return data_[i];
}

template <typename V>
const V& ValueHolder<V>::Value(unsigned int i) const
{
  assert(i<volume_);
  return data_[i];
}

template <typename V>
DataType ValueHolder<V>::GetDataType() 
{
  return GetValueType<V>();
}

template <typename V>
long ValueHolder<V>::MemSize() const
{
  //return sizeof(V)*volume2_+sizeof(V*)*width_*height_+sizeof(V**)*width_+sizeof(*this);
  return sizeof(V)*volume2_;
}

template <typename V>
void ValueHolder<V>::setup()
{
  typedef V* VPtr;
  typedef VPtr* VPtrPtr;
  row_=0;
  slice_=0;
  try {
#if 0
#ifdef USE_ROW_ORDER
    row_ = new VPtr[width_*height_];
    slice_ = new VPtrPtr[width_];
    for(unsigned int u=0;u<width_;++u) {
      for(unsigned int v=0;v<height_;++v) {
	row_[u*height_+v]=&data_[u*height_*depth_+v*depth_];
      }
      slice_[u]=&row_[u*height_];
    }
#else
    row_ = new VPtr[height_*depth_];
    slice_ = new VPtrPtr[depth_];
    
    for(unsigned int w=0;w<depth_;++w) {
      for(unsigned int v=0;v<height_;++v) {
	row_[w*height_+v]=&data_[w*width_*height_+v*width_];
      }
      slice_[w]=&row_[w*height_];
    }
#endif
#endif
  } catch (...) {
    delete slice_;
    delete row_;
    throw;
  }
}

template <typename V>
void ValueHolder<V>::clear()
{
  for(unsigned int c=0;c<volume2_;++c) data_[c]=V();
}


}}} // namespaces
