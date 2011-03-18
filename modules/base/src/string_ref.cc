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

#include <ost/string_ref.hh>

namespace ost {
  
std::pair<bool, int> StringRef::to_int() const
{
  int n=0;
  bool empty=true;
  int sig=1;
  // skip empty characters... at beginning
  for (const char* c=begin_; c!=end_; ++c) {
    if (*c=='-' && empty) {
      empty=false;
      sig=-1;
      continue;
    }
    if (*c>='0' && *c<='9') {
      empty=false;
      n=n*10+int(*c-'0');
      continue;
    }
    return std::make_pair(false, 0);
  }
  if (empty) {
    return std::make_pair(false, 0);
  }
  return std::make_pair(true, sig*n);
}

std::pair<bool, float> StringRef::to_float() const
{
  float n=0;
  bool empty=true;
  int sig=1;
  bool after_dot=false;
  float factor=0.1;
  for (const char* c=begin_; c!=end_; ++c) {
    if (*c=='-' && empty) {
      empty=false;
      sig=-1;
      continue;
    }
    if (*c=='.') {
      if (after_dot==true) {
        return std::make_pair(false, 0.0f);
      }
      after_dot=true;
      continue;
    }
    if (*c>='0' && *c<='9') {
      empty=false;
      if (after_dot==true) {
        n+=factor*int(*c-'0');
        factor*=0.1;
      } else {
        n=n*10+int(*c-'0');
      }
      continue;
    }
    return std::make_pair(false, 0.0f);
  }
  if (empty) {
    return std::make_pair(false, 0.0f);
  }
  return std::make_pair(true, sig*n);
}

std::pair<bool, float> StringRef::parse_rational() const
{
  const_iterator delim_pos=this->find('/');
  if (delim_pos==this->end()) {
    return std::make_pair(false, 0.0f);
  }
  StringRef nom_str(begin_, delim_pos-begin_);
  StringRef denom_str(delim_pos+1, end_-delim_pos-1);
  std::pair<bool, int> nom=nom_str.rtrim().to_int();
  std::pair<bool, int> denom=denom_str.ltrim().to_int();
  if (!nom.first || !denom.first) {
    return std::make_pair(false, 0.0f);
  }
  if (denom.second==0) {
    return std::make_pair(false, 0.0f);
  }
  return std::make_pair(true, float(nom.second)/float(denom.second));
}

std::ostream& operator<<(std::ostream& stream, const StringRef& strref)
{
  if (strref.empty()) {
    return stream;
  }
  stream.write(strref.begin(), strref.size());
  return stream;
}

std::vector<StringRef> StringRef::split(char p) const
{
  std::vector<StringRef> result;
  const char* s=begin_;
  const char* l=begin_;
  while (s!=end_) {
    if (*s==p) {
      if (l!=s) {
        result.push_back(StringRef(l, s-l));
      }
      l=s+1;
    }
    ++s;
  }
  if (l!=s) {
    result.push_back(StringRef(l, s-l));
  }
  return result;
}

bool StringRef::tokenize(std::vector<StringRef>& parts, bool clear) const
{
  if (clear) {
    parts.clear();    
  }
  const char* s=this->begin();
  while (s!=this->end()) {
    while (isspace(*s)) {
      ++s;
      if (s==this->end()) {
        return true;
      }
    }
    if (*s=='\'' || *s=='"') {
      char delim=*s;
      const char* start=++s;
      while (s!=this->end()) {
        ++s;
        if (delim==*(s-1) && (s==this->end() || isspace(*s))) {
          break;
        }
      }
      parts.push_back(StringRef(start, s-start-1));
    } else {
      const char* start=s;
      while (s!=this->end() && !isspace(*s)) {
        ++s;
      }
      if (s-start) {       
        parts.push_back(StringRef(start, s-start));
      } else {
        return false;
      }
    }
  }
  return true;
}

}
