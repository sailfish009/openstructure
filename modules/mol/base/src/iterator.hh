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
#ifndef OST_ITERATOR_HH
#define OST_ITERATOR_HH

#include <map>

#include <ost/mol/impl/entity_impl_fw.hh>
#include <ost/mol/impl/chain_impl_fw.hh>
#include <ost/mol/impl/residue_impl_fw.hh>
#include <ost/mol/chain_handle.hh>
#include <ost/mol/chain_view.hh>
#include <ost/mol/residue_handle.hh>
#include <ost/mol/residue_view.hh>
#include <ost/mol/atom_handle.hh>
#include <ost/mol/atom_view.hh>
#include <ost/mol/entity_handle.hh>
#include <ost/mol/entity_view.hh>

namespace ost { namespace mol {

/// \brief adaptor to turn iterator into an iterator range
/// 
/// To turn two iterators marking the half-closed range [beg, end) into a
/// iterator range use:
/// \code
/// // ent is an instance of entity
/// IterRange<AtomHandleIter> atom_range(ent.AtomsBegin(), ent.AtomsEnd());
/// for (; !atom_range.AtEnd(); ++atom_range) {
/// }
/// \endcode
template <typename I>
class IterRange {
public:
  typedef  I                     iterator_type;
  typedef typename I::value_type value_type;
  IterRange() {}
  IterRange(I beg, I end)
    : end_(end), cur_(beg) {    
  }
  /// \brief evaluates to true when end of range is reached
  bool AtEnd() const {
    return cur_==end_;
  }
  /// \brief step to next element in range
  IterRange<I>& Next() {
    ++cur_;
    return *this;
  }
  typename I::value_type& Get() {
    return *cur_;
  }  
  IterRange<I>& operator++() {
    ++cur_;
    return *this;
  }
  typename I::value_type& operator*() {
    return this->Get();
  }
private:
  I end_;
  I cur_;
};

namespace impl {
  // forward declearation of chain impl map.
  typedef std::map<String, ChainImplPtr> ChainImplMap;
}
class DLLEXPORT_OST_MOL ChainHandleIter : public std::iterator<std::forward_iterator_tag, 
                                             ChainHandle> {
public: // internally used constructors
  ChainHandleIter(impl::ChainImplMap::iterator chain_it)  
    : cur_(chain_it) {    
  }
public:
  ChainHandle operator*() {
    return ChainHandle(cur_->second);
  }
  
  ChainHandleIter& operator++() {
    ++cur_;
    return *this;
  }
  
  bool operator==(const ChainHandleIter& rhs) const {
    return cur_==rhs.cur_;
  }
  
  bool operator!=(const ChainHandleIter& rhs) const {
    return !this->operator==(rhs);
  }
private:
  impl::ChainImplMap::iterator cur_;
};

class DLLEXPORT_OST_MOL ResidueHandleIter : public std::iterator<std::forward_iterator_tag,
                                               ResidueHandle> {
public:
  ResidueHandleIter() {    
  }
  ResidueHandleIter(impl::ChainImplMap::iterator chain_it, 
                    impl::ResidueImplList::iterator res_it,
                    impl::EntityImplPtr ent);
  bool operator==(const ResidueHandleIter& rhs) const {
#if defined(_MSC_VER)    
    return cur_chain_==rhs.cur_chain_ && cur_res_==rhs.cur_res_;
#else
    return cur_res_==rhs.cur_res_;
#endif
  }
  
  bool operator!=(const ResidueHandleIter& rhs) const {
    return !this->operator==(rhs);
  }
  ResidueHandleIter& operator++();
  
  ResidueHandle operator*();
  
private:
  impl::ChainImplMap::iterator cur_chain_;
  impl::ResidueImplList::iterator cur_res_;
  impl::EntityImplPtr  ent_;
};

class DLLEXPORT_OST_MOL ResidueViewIter : public std::iterator<std::forward_iterator_tag,
                                             ResidueView> {
public:
  ResidueViewIter();

  ResidueViewIter(ChainViewList::const_iterator chain_it,
                  ResidueViewList::const_iterator res_it,
                  EntityView ent);

  bool operator==(const ResidueViewIter& rhs) const {
#if defined(_MSC_VER)    
    return cur_chain_==rhs.cur_chain_ && cur_res_==rhs.cur_res_;
#else
    return cur_res_==rhs.cur_res_;
#endif    
    return cur_res_==rhs.cur_res_;
  }
  
  bool operator!=(const ResidueViewIter& rhs) const {
    return !this->operator==(rhs);
  }
  ResidueViewIter& operator++();
  
  ResidueView operator*();
  
private:
  ChainViewList::const_iterator    cur_chain_;
  ResidueViewList::const_iterator  cur_res_;
  EntityView                       ent_;
};

class DLLEXPORT_OST_MOL AtomHandleIter : public std::iterator<std::forward_iterator_tag,
                                            AtomHandle> {
public:
  AtomHandleIter(); 

  AtomHandleIter(impl::ChainImplMap::iterator chain_it,
                 impl::ResidueImplList::iterator res_it,
                 impl::AtomImplList::iterator atom_it,
                 impl::EntityImplPtr ent, bool skip_empty);
                 
  bool operator==(const AtomHandleIter& rhs) const {
#if defined(_MSC_VER)    
    return cur_chain_==rhs.cur_chain_ && cur_res_==rhs.cur_res_ &&
           cur_atom_==rhs.cur_atom_;
#else
   return cur_atom_==rhs.cur_atom_;
#endif
  }
  bool operator!=(const AtomHandleIter& rhs) const {
  return !this->operator==(rhs);
  }
  AtomHandleIter& operator++();

  AtomHandle operator*();
private:
  void SkipEmpty();
  impl::ChainImplMap::iterator cur_chain_;
  impl::ResidueImplList::iterator cur_res_;
  impl::AtomImplList::iterator cur_atom_;
  impl::EntityImplPtr  ent_;
};

class DLLEXPORT_OST_MOL AtomViewIter : public std::iterator<std::forward_iterator_tag,
                                          AtomView> {
public:
  AtomViewIter();

  AtomViewIter(ChainViewList::const_iterator chain_it,
               ResidueViewList::const_iterator res_it,
               AtomViewList::const_iterator atom_it,
               EntityView ent, bool skip_empty);
                 
  bool operator==(const AtomViewIter& rhs) const {
#if defined(_MSC_VER)    
    return cur_chain_==rhs.cur_chain_ && cur_res_==rhs.cur_res_ &&
           cur_atom_==rhs.cur_atom_;
#else
   return cur_atom_==rhs.cur_atom_;
#endif
  }
  bool operator!=(const AtomViewIter& rhs) const {
    return !this->operator==(rhs);
  }
  AtomViewIter& operator++();

  AtomView operator*();
private:
  void SkipEmpty();
  ChainViewList::const_iterator    cur_chain_;
  ResidueViewList::const_iterator  cur_res_;
  AtomViewList::const_iterator     cur_atom_;
  EntityView                       ent_;
};

}} //ns


#endif
