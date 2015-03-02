//------------------------------------------------------------------------------
// This file is part of the OpenStructure project <www.openstructure.org>
//
// Copyright (C) 2008-2015 by the OpenStructure authors
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
#include <ost/mol/impl/residue_impl.hh>
#include <ost/mol/impl/chain_impl.hh>
#include <ost/mol/impl/entity_impl.hh>
#include <ost/mol/impl/atom_impl.hh>

#include "iterator.hh"

namespace ost { namespace mol {

void ResidueHandleIter::SkipEmpty()
{
  if (cur_res_==impl::end((*cur_chain_)->GetResidueList())) {
    // we have to skip over empty chains otherwise we end up pointing to an 
    // invalid residue.
    do {
      ++cur_chain_;
      if (impl::end(ent_->GetChainList())==cur_chain_) {
        break;        
      }
      cur_res_=impl::begin((*cur_chain_)->GetResidueList());
    } while ((*cur_chain_)->GetResidueList().empty());
  }
}

ResidueHandleIter& ResidueHandleIter::operator++() 
{
  ++cur_res_;
  this->SkipEmpty();
  return *this;
}

ResidueHandleIter::ResidueHandleIter(impl::pointer_it<impl::ChainImplPtr> chain_it, 
                                     impl::pointer_it<impl::ResidueImplPtr> res_it,
                                     impl::EntityImplPtr ent, bool skip_empty) 
 : cur_chain_(chain_it), cur_res_(res_it),
   ent_(ent) {
  if (skip_empty) {
    this->SkipEmpty();
  }
}

ResidueHandle ResidueHandleIter::operator*() {
  return ResidueHandle(*cur_res_);
}



ResidueView ResidueViewIter::operator*() {
  return ResidueView(*cur_res_);
}

void ResidueViewIter::SkipEmpty()
{
  if (cur_res_==impl::end(cur_chain_->GetResidueList())) {
    // we have to skip over empty chains otherwise we end up pointing to an 
    // invalid residue.
    do {
      ++cur_chain_;
      if (impl::end(ent_.GetChainList())==cur_chain_) {
        break;        
      }
      cur_res_=impl::begin(cur_chain_->GetResidueList());
    } while (cur_chain_->GetResidueList().empty());
  }  
}
ResidueViewIter& ResidueViewIter::operator++() {
  ++cur_res_;
  this->SkipEmpty();
  return *this;
}

ResidueViewIter::ResidueViewIter(impl::pointer_it<ChainView>   chain_it,
                                 impl::pointer_it<ResidueView> res_it,
                                 EntityView ent, bool skip_empty) 
 : cur_chain_(chain_it), cur_res_(res_it),
   ent_(ent) {
  if (skip_empty) {
    this->SkipEmpty();
  }
}

AtomHandleIter::AtomHandleIter(impl::pointer_it<impl::ChainImplPtr> chain_it,
                               impl::pointer_it<impl::ResidueImplPtr> res_it,
                               impl::pointer_it<impl::AtomImplPtr> atom_it,
                               impl::EntityImplPtr ent, bool skip_empty)
 : cur_chain_(chain_it), cur_res_(res_it), cur_atom_(atom_it),
   ent_(ent) 
{
  if (skip_empty) {
    this->SkipEmpty();
  }
}

AtomHandle AtomHandleIter::operator*() {
  return AtomHandle(*cur_atom_);
}

void AtomHandleIter::SkipEmpty()
{
  if (impl::end((*cur_res_)->GetAtomList())==cur_atom_) {
    // we have to skip over empty chains and residues otherwise we end up 
    // pointing to an invalid atom.
    do {
      ++cur_res_;
      if (impl::end((*cur_chain_)->GetResidueList())==cur_res_) {
        do {
          ++cur_chain_;
          if (impl::end(ent_->GetChainList())==cur_chain_) {
            return;
          }

          cur_res_=impl::begin((*cur_chain_)->GetResidueList());
          if (!(*cur_chain_)->GetResidueList().empty())          
            cur_atom_=impl::begin((*cur_res_)->GetAtomList());
          else
            cur_atom_=impl::pointer_it<impl::AtomImplPtr>(NULL);
        } while ((*cur_chain_)->GetResidueList().empty());
      } else {
        cur_atom_=impl::begin((*cur_res_)->GetAtomList());
      }
    } while ((*cur_res_)->GetAtomList().empty());
  }
}

AtomHandleIter& AtomHandleIter::operator++() 
{
  ++cur_atom_;
  this->SkipEmpty();
  return *this;
}

AtomViewIter::AtomViewIter(impl::pointer_it<ChainView> chain_it,
                           impl::pointer_it<ResidueView> res_it,
                           impl::pointer_it<AtomView> atom_it,
                           EntityView ent, bool skip_empty)
  : cur_chain_(chain_it), cur_res_(res_it), cur_atom_(atom_it),
    ent_(ent) {
  if (skip_empty) {
    this->SkipEmpty();    
  }
}

void AtomViewIter::SkipEmpty()
{
  if (impl::end(cur_res_->GetAtomList())==cur_atom_) {
    do {
      ++cur_res_;      
      if (impl::end(cur_chain_->GetResidueList())==cur_res_) {
        do {
          ++cur_chain_;
          if (impl::end(ent_.GetChainList())==cur_chain_) {
            return;
          }          
          cur_res_=impl::begin(cur_chain_->GetResidueList());
          if (!cur_chain_->GetResidueList().empty()) {
            cur_atom_=impl::begin(cur_res_->GetAtomList());
          } else {
            cur_atom_=impl::pointer_it<AtomView>(NULL);
          }
        } while (cur_chain_->GetResidueList().empty());
      } else {
        cur_atom_=impl::begin(cur_res_->GetAtomList());
      }
    } while (cur_res_->GetAtomList().empty());
  }
}

AtomView AtomViewIter::operator*() 
{
  return AtomView(*cur_atom_);
}

AtomViewIter& AtomViewIter::operator++() 
{
  ++cur_atom_;
  this->SkipEmpty();
  return *this;
}

}} // ns
