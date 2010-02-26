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
#include "chain_base.hh"
#include <ost/mol/impl/chain_impl.hh>
#include <ost/invalid_handle.hh>

namespace ost { namespace mol {

ChainBase::ChainBase()
{ }

ChainBase::ChainBase(const impl::ChainImplPtr& impl): 
  impl_(impl) 
{}
GenericPropertyContainerImpl* ChainBase::GpImpl()
{
  return impl_.get();
}


const GenericPropertyContainerImpl* ChainBase::GpImpl() const
{
  return impl_.get();
}

String ChainBase::GetName() const {
  this->CheckValidity();
  return impl_->GetName();
}

ChainBase::operator bool() const {
  return impl_.get()!=0;
}

bool ChainBase::IsValid() const {
  return impl_;
}

void ChainBase::CheckValidity() const {
  if (!impl_)
    throw InvalidHandle();
}

DLLEXPORT_OST_MOL std::ostream& operator<<(std::ostream& os, 
                                            const ChainBase& chain) {
  os << chain.GetName();                                              
  return os;
}

}} // ns

