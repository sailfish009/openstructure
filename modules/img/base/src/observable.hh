//------------------------------------------------------------------------------
// This file is part of the OpenStructure project <www.openstructure.org>
//
// Copyright (C) 2008-2015 by the OpenStructure authors
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
  abstract observable concept

  Author: Ansgar Philippsen
*/

#ifndef IMG_OBSERVABLE_H
#define IMG_OBSERVABLE_H

#include <list>
#include "extent.hh" 

namespace ost { namespace img {

//! templated observable class
/*
  manages a list of observers, which must
  offer the methods ObserverRelease, ObserverInvalidate
  and ObserverUpdate
*/
template <class T>
class DLLEXPORT Observable {
  typedef std::list<T *> ObserverList;
  typedef typename ObserverList::iterator ObserverIter;
  typedef typename ObserverList::const_iterator ObserverConstIter;
public:
  Observable() {
    list_.clear();
  }

  /*
    copy logic: the observers are not copied
  */
  Observable(const Observable& o) {
    list_.clear();
  }

  ~Observable() {
    for(ObserverIter it=list_.begin();it!=list_.end();++it) {
      (*it)->ObserverInvalidate();
      (*it)->ObserverRelease();
    }
  }

  /*
    assignement logic: the observers are not copied
  */
  Observable& operator=(const Observable& o) {
    list_.clear();
    return *this;
  }

  void Attach(T* d) {
    list_.push_back(d);
  }

  void Detach(T* o) {
    list_.remove(o);
  }

  void Notify() const {
    for(ObserverConstIter it=list_.begin();it!=list_.end();++it)
      (*it)->ObserverUpdate();
  }
  void Notify(const Extent& e) const {
    for(ObserverConstIter it=list_.begin();it!=list_.end();++it)
      (*it)->ObserverUpdate(e);
  }
  void Notify(const Point& p) const {
    for(ObserverConstIter it=list_.begin();it!=list_.end();++it)
      (*it)->ObserverUpdate(p);
  }

  int GetListSize() const {
    return list_.size();
  }

  long MemSize() const {
    return sizeof(list_) + list_.size()*sizeof(T*);
  }

  private:
    ObserverList list_;
};


}} // namespace

#endif
