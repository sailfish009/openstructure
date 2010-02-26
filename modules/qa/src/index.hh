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
#ifndef OST_QA_INDEX_HH
#define OST_QA_INDEX_HH

/*
  Author: Marco Biasini
 */ 

#include <ost/stdint.hh>

#include <cstring>
#include <boost/shared_ptr.hpp>
#include <ost/qa/module_config.hh>

namespace ost { namespace qa {

namespace impl {
template <uint32_t D>
class IndexBase {
public:
  enum { Dimension = D };
   IndexBase(const IndexBase& rhs) {
    memcpy(data_, rhs.data_, sizeof(uint32_t[D]));
  }
  IndexBase() {
    memset(data_, sizeof(uint32_t[D]), 0);
  }
  IndexBase& operator=(const IndexBase& rhs) {
    memcpy(data_, rhs.data_, sizeof(uint32_t[D]));
    return *this;
  }
  uint32_t operator[](uint32_t idx) const {
    assert(idx <= D);
    return data_[idx];
  }
  uint32_t& operator[](uint32_t idx) {
    assert(idx <= D);
    return data_[idx];
  }  
private:
  uint32_t data_[D];   
};

} // namespace impl

template <uint32_t D>
class Index;

template <>
class Index<1> : public impl::IndexBase<1> {
public:
  Index() : impl::IndexBase<1>() {}
  Index(uint32_t a) {
    (*this)[0]=a;
  }
};
template <>
class Index<2> : public impl::IndexBase<2> {
public:
  Index() : impl::IndexBase<2>() {}
  Index(uint32_t a, uint32_t b) {
    (*this)[0]=a;
    (*this)[1]=b;    
  }
};

template <>
class Index<3> : public impl::IndexBase<3> {
public:
  Index() : impl::IndexBase<3>() {}
  Index(uint32_t a, uint32_t b, uint32_t c) {
    (*this)[0]=a;
    (*this)[1]=b;    
    (*this)[2]=c;        
  }
};

template <>
class Index<4> : public impl::IndexBase<4> {
public:
  Index() : impl::IndexBase<4>() {}
  Index(uint32_t a, uint32_t b, uint32_t c, uint32_t d) {
    (*this)[0]=a;
    (*this)[1]=b;    
    (*this)[2]=c;        
    (*this)[3]=d;            
  }
};

template <>
class Index<5> : public impl::IndexBase<5> {
public:
  Index() : impl::IndexBase<5>() {}
  Index(uint32_t a, uint32_t b, uint32_t c, uint32_t d, uint32_t e) {
    (*this)[0]=a;
    (*this)[1]=b;    
    (*this)[2]=c;        
    (*this)[3]=d;            
    (*this)[4]=e;                
  }
};

template <>
class Index<6> : public impl::IndexBase<6> {
public:
  Index() : impl::IndexBase<6>() {}
  Index(uint32_t a, uint32_t b, uint32_t c, uint32_t d, uint32_t e, uint32_t f) {
    (*this)[0]=a;
    (*this)[1]=b;    
    (*this)[2]=c;        
    (*this)[3]=d;            
    (*this)[4]=e;
    (*this)[5]=f;    
  }
};

template <>
class Index<7> : public impl::IndexBase<7> {
public:
  Index() : impl::IndexBase<7>() {}
  Index(uint32_t a, uint32_t b, uint32_t c, uint32_t d, uint32_t e, uint32_t f, uint32_t g) {
    (*this)[0]=a;
    (*this)[1]=b;    
    (*this)[2]=c;        
    (*this)[3]=d;            
    (*this)[4]=e;
    (*this)[5]=f;    
    (*this)[6]=g;        
  }
};

template<uint32_t D>
class IndexIterator {
public:
  typedef Index<D>  IndexType;
  IndexIterator(const IndexType& s, const IndexType& e)
    : start_(s), end_(e), current_(s) {
    
  }  
  
  IndexIterator<D>& operator++() {
    uint32_t current_it=0;
    while (++current_[current_it] > end_[current_it]) {
      current_it++;
      if (current_it < D) {
        current_[current_it-1] = start_[current_it-1];
      } else {
        break;
      }
    }
    return *this;
  }
  const IndexType& operator *() const {
    return current_;
  }
  bool AtEnd() {
    return current_[D-1] > end_[D-1];
  }
private:
  IndexType  start_;
  IndexType  end_;
  IndexType  current_;

};

}}

#endif
