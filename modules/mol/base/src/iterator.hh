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


#include <ost/mol/chain_handle.hh>
#include <ost/mol/chain_view.hh>
#include <ost/mol/residue_handle.hh>
#include <ost/mol/residue_view.hh>
#include <ost/mol/atom_handle.hh>
#include <ost/mol/atom_view.hh>
#include <ost/mol/entity_handle.hh>
#include <ost/mol/entity_view.hh>
#include <ost/mol/impl/entity_impl_fw.hh>
#include <ost/mol/impl/chain_impl_fw.hh>
#include <ost/mol/impl/residue_impl_fw.hh>
#include <ost/mol/impl/pointer_iterator.hh>

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
  // forward declearation of chain impl list.
  typedef std::vector<ChainImplPtr> ChainImplList;
}

class DLLEXPORT_OST_MOL ChainHandleIter : public std::iterator<std::forward_iterator_tag, 
                                             ChainHandle> {
public: // internally used constructors
  ChainHandleIter(impl::ChainImplList::iterator chain_it)  
    : cur_(chain_it) {    
  }
public:
  ChainHandle operator*() {
    return ChainHandle(*cur_);
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
  impl::ChainImplList::iterator cur_;
};

class DLLEXPORT_OST_MOL ResidueHandleIter : public std::iterator<std::forward_iterator_tag,
                                               ResidueHandle> {
public:
  ResidueHandleIter(): cur_chain_(NULL), cur_res_(NULL) { }
  ResidueHandleIter(impl::pointer_it<impl::ChainImplPtr> chain_it, 
                    impl::pointer_it<impl::ResidueImplPtr> res_it,
                    impl::EntityImplPtr ent, bool skip_empty);
  
  bool operator==(const ResidueHandleIter& rhs) const 
  {
    return cur_res_==rhs.cur_res_;
  }
  
  void SkipEmpty();
  
  bool operator!=(const ResidueHandleIter& rhs) const 
  {
    return !this->operator==(rhs);
  }
  ResidueHandleIter& operator++();
  
  ResidueHandle operator*();
  
private:
  impl::pointer_it<impl::ChainImplPtr> cur_chain_;
  impl::pointer_it<impl::ResidueImplPtr> cur_res_;
  impl::EntityImplPtr  ent_;
};

class DLLEXPORT_OST_MOL ResidueViewIter : public std::iterator<std::forward_iterator_tag,
                                             ResidueView> {
public:
  ResidueViewIter(): cur_chain_(NULL), cur_res_(NULL) { }

  ResidueViewIter(impl::pointer_it<ChainView> chain_it,
                  impl::pointer_it<ResidueView> res_it,
                  EntityView ent, bool skip_empty);

  bool operator==(const ResidueViewIter& rhs) const {
    return cur_res_==rhs.cur_res_;
  }
  void SkipEmpty();
  
  bool operator!=(const ResidueViewIter& rhs) const {
    return !this->operator==(rhs);
  }
  ResidueViewIter& operator++();
  
  ResidueView operator*();
  
private:
  impl::pointer_it<ChainView>    cur_chain_;
  impl::pointer_it<ResidueView>  cur_res_;
  EntityView                     ent_;
};

class DLLEXPORT_OST_MOL AtomHandleIter : public std::iterator<std::forward_iterator_tag,
                                            AtomHandle> {
public:
  AtomHandleIter(): cur_chain_(NULL), cur_res_(NULL), cur_atom_(NULL) { }

  AtomHandleIter(impl::pointer_it<impl::ChainImplPtr> chain_it,
                 impl::pointer_it<impl::ResidueImplPtr> res_it,
                 impl::pointer_it<impl::AtomImplPtr> atom_it,
                 impl::EntityImplPtr ent, bool skip_empty);
                 
  bool operator==(const AtomHandleIter& rhs) const 
  {
   return cur_atom_==rhs.cur_atom_;
  }
  
  bool operator!=(const AtomHandleIter& rhs) const 
  {
    return !this->operator==(rhs);
  }
  
  AtomHandleIter& operator++();

  AtomHandle operator*();
private:
  void SkipEmpty();
  impl::pointer_it<impl::ChainImplPtr> cur_chain_;
  impl::pointer_it<impl::ResidueImplPtr> cur_res_;
  impl::pointer_it<impl::AtomImplPtr> cur_atom_;
  impl::EntityImplPtr  ent_;
};

class DLLEXPORT_OST_MOL AtomViewIter : public std::iterator<std::forward_iterator_tag,
                                          AtomView> {
public:
  AtomViewIter(): cur_chain_(NULL), cur_res_(NULL), cur_atom_(NULL) { }

  AtomViewIter(impl::pointer_it<ChainView> chain_it,
               impl::pointer_it<ResidueView> res_it,
               impl::pointer_it<AtomView> atom_it,
               EntityView ent, bool skip_empty);
                 
  bool operator==(const AtomViewIter& rhs) const 
  {
   return cur_atom_==rhs.cur_atom_;
  }
  
  bool operator!=(const AtomViewIter& rhs) const 
  {
    return !this->operator==(rhs);
  }
  
  AtomViewIter& operator++();

  AtomView operator*();
private:
  void SkipEmpty();
  impl::pointer_it<ChainView>    cur_chain_;
  impl::pointer_it<ResidueView>  cur_res_;
  impl::pointer_it<AtomView>     cur_atom_;
  EntityView                       ent_;
};

}} //ns


#endif
