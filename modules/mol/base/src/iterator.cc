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
#include <ost/mol/impl/residue_impl.hh>
#include <ost/mol/impl/chain_impl.hh>
#include <ost/mol/impl/entity_impl.hh>
#include <ost/mol/impl/atom_impl.hh>

#include "iterator.hh"

namespace ost { namespace mol {


ResidueHandleIter& ResidueHandleIter::operator++() {
  ++cur_res_;
  if (cur_res_==(*cur_chain_)->GetResidueList().end()) {
    // we have to skip over empty chains otherwise we end up pointing to an 
    // invalid residue.
    do {
      ++cur_chain_;
      if (ent_->GetChainList().end()==cur_chain_) {
        break;        
      }
      cur_res_=(*cur_chain_)->GetResidueList().begin();
    } while ((*cur_chain_)->GetResidueList().empty());
  }        
  return *this;
}

ResidueHandleIter::ResidueHandleIter(impl::ChainImplList::iterator chain_it, 
                                     impl::ResidueImplList::iterator res_it,
                                     impl::EntityImplPtr ent) 
 : cur_chain_(chain_it), cur_res_(res_it),
   ent_(ent) {

}

ResidueHandle ResidueHandleIter::operator*() {
  return ResidueHandle(*cur_res_);
}



ResidueView ResidueViewIter::operator*() {
  return ResidueView(*cur_res_);
}

ResidueViewIter& ResidueViewIter::operator++() {
  ++cur_res_;
  if (cur_res_==cur_chain_->GetResidueList().end()) {
    // we have to skip over empty chains otherwise we end up pointing to an 
    // invalid residue.
    do {
      ++cur_chain_;
      if (ent_.GetChainList().end()==cur_chain_) {
        break;        
      }
      cur_res_=cur_chain_->GetResidueList().begin();
    } while (cur_chain_->GetResidueList().empty());
  }        
  return *this;
}

ResidueViewIter::ResidueViewIter() 
#ifdef _MSC_VER
    : cur_res_() 
#else
    : cur_res_(NULL) 
#endif
{}

ResidueViewIter::ResidueViewIter(ChainViewList::const_iterator chain_it,
                                 ResidueViewList::const_iterator res_it,
                                 EntityView ent) 
 : cur_chain_(chain_it), cur_res_(res_it),
   ent_(ent) {
}

AtomHandleIter::AtomHandleIter() 
#ifdef _MSC_VER
    : cur_atom_() 
#else
    : cur_atom_(NULL) 
#endif
{}  

AtomHandleIter::AtomHandleIter(impl::ChainImplList::iterator chain_it,
                               impl::ResidueImplList::iterator res_it,
                               impl::AtomImplList::iterator atom_it,
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
  if ((*cur_res_)->GetAtomList().end()==cur_atom_) {
    // we have to skip over empty chains and residues otherwise we end up 
    // pointing to an invalid atom.
    do {
      ++cur_res_;
      if ((*cur_chain_)->GetResidueList().end()==cur_res_) {
        do {
          ++cur_chain_;
          if (ent_->GetChainList().end()==cur_chain_) {
            return;
          }

          cur_res_=(*cur_chain_)->GetResidueList().begin();
          if (!(*cur_chain_)->GetResidueList().empty())          
            cur_atom_=(*cur_res_)->GetAtomList().begin();
          else
            cur_atom_=impl::AtomImplList::iterator();
        } while ((*cur_chain_)->GetResidueList().empty());
      } else {
        cur_atom_=(*cur_res_)->GetAtomList().begin();
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

AtomViewIter::AtomViewIter()
#ifdef _MSC_VER
    : cur_atom_() 
#else
    : cur_atom_(NULL) 
#endif
{}  

AtomViewIter::AtomViewIter(ChainViewList::const_iterator chain_it,
                           ResidueViewList::const_iterator res_it,
                           AtomViewList::const_iterator atom_it,
                           EntityView ent, bool skip_empty)
  : cur_chain_(chain_it), cur_res_(res_it), cur_atom_(atom_it),
    ent_(ent) {
  if (skip_empty) {
    this->SkipEmpty();    
  }
}

void AtomViewIter::SkipEmpty()
{
  if (cur_res_->GetAtomList().end()==cur_atom_) {
    do {
      ++cur_res_;      
      if (cur_chain_->GetResidueList().end()==cur_res_) {
        do {
          ++cur_chain_;
          if (ent_.GetChainList().end()==cur_chain_) {
            return;
          }          
          cur_res_=cur_chain_->GetResidueList().begin();
          if (!cur_chain_->GetResidueList().empty())          
            cur_atom_=cur_res_->GetAtomList().begin();
          else
            cur_atom_=AtomViewList::const_iterator();
        } while (cur_chain_->GetResidueList().empty());
      } else {
        cur_atom_=cur_res_->GetAtomList().begin();
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
