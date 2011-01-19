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
#ifndef OST_STRING_REF_HH
#define OST_STRING_REF_HH

/*
  Author: Marco Biasini
 */
#include <ctype.h>
#include <cassert>
#include <iostream>
#include <ost/base.hh>
#include <string.h>
#include <vector>
#include <ost/module_config.hh>


namespace ost {

/// \brief convenient datatype for referencing character data
/// \note it is not in general safe to store a StringRef as it does not 
///    guarantee that the character data it points to stays alive.
class DLLEXPORT_OST_BASE StringRef {
public:
  
  typedef char* iterator;
  typedef const char* const_iterator;
  typedef char value_type;

  StringRef(const char* begin, size_t len): begin_(begin), end_(begin+len) { }
  StringRef(): begin_(NULL), end_(NULL) { }


  const char* begin() const { return begin_; }
  const char* end() const { return end_; }
  const char* data() const { return begin_; }

  size_t size() const { return end_-begin_; }
  size_t length() const { return this->size(); }

  char front() const 
  { 
    assert(!this->empty());
    return *begin_; 
  }

  const_iterator find(char p) const {
    const char* s=begin_;
    while (s!=end_) {
      if (*s==p) {
        return s;
      }
      ++s;
    }
    return s;
  }
  StringRef substr(size_t pos, size_t n=std::string::npos) const
  {
    if (n==std::string::npos) {
      assert(pos>=0 && begin_+pos<=end_);      
      return StringRef(begin_+pos, this->length()-pos);
    } else {
      assert(pos>=0 && begin_+pos+n<=end_);      
      return StringRef(begin_+pos, n);
    }
  }
  std::string str() const
  {
    return std::string(begin_, end_-begin_);
  }
  char back() const
  {
    assert(!this->empty());
    return *(end_-1);
  }

  const char& operator[](int index) const { return begin_[index]; } 

  bool operator==(const StringRef& rhs) const {
    return this->length()==rhs.length() && 
           !memcmp(rhs.data(), this->data(), this->size());
  }

  bool operator!=(const StringRef& rhs) const {
    return !this->operator==(rhs);
  }
  /// \brief strip space characters on the right
  StringRef rtrim() const {
    const char* s=end_;
    while(--s>begin_ && isspace(*s)) {
    }
    return StringRef(begin_, s+1-begin_);
  }
  /// \brief strip space characters on the left  
  StringRef ltrim() const {
    const char* s=begin_;
    while(s<end_ && isspace(*s)) {
      ++s;
    }
    return StringRef(s, end_-s);
  }
  /// \brief strip space characters on the left and the right
  StringRef trim() const {
    return this->rtrim().ltrim();
  }
  
  /// \brief convert to integer
  /// \return a tuple with \c first  set to true and \c second set to 
  ///     the int if the conversion succeeds. If the conversion fails,
  ///     \c first is set to false.
  std::pair<bool, int> to_int() const;
  
  /// \brief convert to float
  /// \return a tuple with \c first  set to true and \c second set to 
  ///     the float if the conversion succeeds. If the conversion fails,
  ///     \c first is set to false.
  std::pair<bool, float> to_float() const;
  bool empty() const { return begin_==end_; }

  std::pair<bool, float> parse_rational() const;
  /// \brief split string into chunks delimited by \p p
  std::vector<StringRef> split(char p) const;
  
  /// \brief tokenizes the string
  /// 
  /// Values can be grouped by using quotes:
  /// 
  /// 'a b' c is split into 'a b' and 'c', whereas a b c is split into 
  /// 'a', 'b', 'c'
  bool tokenize(std::vector<StringRef>& parts, bool clear=true) const;
private:
  const char* begin_;
  const char* end_;  
  

};
//std::stringstream& operator<<(std::stringstream& stream, const StringRef& strref);

DLLEXPORT_OST_BASE std::ostream& operator<<(std::ostream& stream, const StringRef& strref);

}
#endif
