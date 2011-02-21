//------------------------------------------------------------------------------
// This file is part of the OpenStructure project <www.openstructure.org>
//
// Copyright (C) 2008-2011 by the OpenStructure authors
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
  value holder for image state

  Author: Ansgar Philippsen
*/

#ifndef VALUE_HOLDER_H
#define VALUE_HOLDER_H

#include <ost/base.hh>
#include <ost/img/module_config.hh>
#include <ost/img/data_types.hh>
#include <ost/img/size.hh>

#include "type_fw.hh"

namespace value_holder_test {

template <typename V>
void ReleaseAndReconstruct();

}

namespace ost { namespace img { namespace image_state {

/*
  Value Holder

  Provide a dynamically allocated 3D array with
  optimized access, based on a zero based Index, which
  in turn encapsulates an unsigned integer triplet

  Should provide reasonable exception safety, ie an exception
  during construction will not cause a memory leak.
*/
template <typename V>
class TEMPLATE_EXPORT ValueHolder {
public:
  typedef V* VPtr;
  typedef VPtr* VPtrPtr;

  /*! @name Construction, Release, etc
   */
  //@{
  //! initialization with explicit width, height, and depth
  ValueHolder(unsigned int wi, unsigned int he, unsigned int de);

  //! initialization with size
  ValueHolder(const Size& s);

  //! initialization with size and physical size
  ValueHolder(const Size& s, const Size& ps);

  //! copy ctor provides full copy!
  ValueHolder(const ValueHolder<V>& h);

  //! initialize with a size and an existing value holder, grabbing the memory
  template <typename U>
  ValueHolder(const Size& s, ValueHolder<U>& v):
    width_(s[0]),
    height_(s[1]),
    depth_(s[2]),
    volume_(width_*height_*depth_),
    volume2_(v.GetPhysicalSize()*sizeof(U)/sizeof(V)),
    data_(reinterpret_cast<V*>(v.ReleaseData())),
    row_(0),
    slice_(0)
  {
    try {
      setup();
    } catch(...) {
      delete []data_;
    }
  }


  //! assignement provides full copy!
  /*!
    Not exception safe, ie the state of value holder is
    undefined if something goes wrong here. This is on
    purpose to allow memory efficient assignement!
  */
  ValueHolder& operator=(const ValueHolder<V>& h);

  //! free allocated memory upon destruction
  ~ValueHolder();

  //! release the held data, invalidates holder!
  /*!
    the data area is returned, but no longer belongs
    to the value holder. it now belongs to the caller, who 
    must eiter free it explicitely, or use it in a
    constructor argument to a new value holder.
   */
  VPtr ReleaseData();

  //! swap data with another value holder
  void Swap(ValueHolder& vh);
  //@}

  //! @name Property access
  //@{

  // retrieve width, at least 1
  unsigned int GetWidth() const {return width_;}
  // retrieve height, at least 1
  unsigned int GetHeight() const {return height_;}
  // retrieve depth, at least 1
  unsigned int GetDepth() const {return depth_;}
  // retrieve overall size (width*height*depth), at least 1
  unsigned int GetVolume() const {return volume_;}
  // retrieve dimensions as Size object
  Size GetSize() const;

  static DataType GetDataType();

  unsigned int GetPhysicalSize() const {return volume2_;}

  long MemSize() const;

  //@}

  /*! @name Data Access
    access is either via interface that uses the internal 
    row and slice pointers, or these pointers may be
    retrieved directly.
  */

  //! return direct r/w access to the value without boundary check
  /*!
    The lookup is based on an integer triplet encapsulated within
    Index. Efficient lookup using helper tables.
  */
  V& Value(const Index& i);

  //! return direct ro access to the value without boundary check
  /*!
    The lookup is based on an integer triplet encapsulated within
    Index. Efficient lookup using helper tables.
  */
  const V& Value(const Index& i) const;

  //! return direct r/w access to the value without boundary check
  /*!
    DEPRECATED! Use GetData()[i]
  */
  V& Value(unsigned int i);

  //! return direct ro access to the value without boundary check
  /*!
    DEPRECATED! Use GetData()[i]
  */
  const V& Value(unsigned int i) const;


  //! return pointer to raw data
  VPtr GetData() {return data_;}
  //! const version of GetData()
  const VPtr GetData() const {return data_;}

  //! return number of data items DEPRECATED
  int DataCount() const {return volume_;}

  //! return number of data items
  /*!
    this is the volume: width*height*depth
  */
  int GetDataCount() const {return volume_;}

  const VPtr GetEnd() const {return &data_[volume_];}

  //! return pointer to row pointers
  VPtr* GetRows() {return row_;}
  //! const version of GetRows()
  const VPtr* GetRows() const {return row_;}
  //! return number of row pointers
  /*!
    this is width*height
  */
  int GetRowCount() const {return width_*height_;}

  //! return pointer to slice pointers
  VPtrPtr* GetSlices() {return slice_;}
  //! const version of GetSlices()
  const VPtrPtr* GetSlices() const {return slice_;}
  //! return number of slices
  /*!
    equals the width
  */
  int GetSliceCount() const {return width_;}

  //@}
private:
  unsigned int width_, height_, depth_, volume_;
  // this is a hack to get padding for fftw to work...
  unsigned int volume2_;

  // actual data storage
  V* data_;
  // optimization 
  V** row_;
  // optimization
  V*** slice_;

  void setup();
  void clear();
};

}}} // namespaces

#endif
