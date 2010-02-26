//------------------------------------------------------------------------------
// This file is part of the OpenStructure project <www.openstructure.org>
//
// Copyright (C) 2008-2010 by the OpenStructure authors
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
  high level info interface
  
  Author: Ansgar Philippsen
*/

#include <iostream>
#include <sstream>

#include "info.hh"
#include "info_impl.hh"
#include "info_error.hh"

namespace ost { namespace info {

InfoHandle CreateInfo()
{
  return InfoHandle(RootPtr(new detail::InfoImpl()));
}
InfoHandle CreateInfo(const String& dtdfile)
{
  return InfoHandle(RootPtr(new detail::InfoImpl(true,dtdfile)));
}

InfoHandle LoadInfo(const String& file)
{
  return InfoHandle(RootPtr(new detail::InfoImpl(file)));
}


// handle
InfoHandle::InfoHandle():
  impl_()
{}

InfoHandle::InfoHandle(RootPtr impl):
  impl_(impl)
{}

InfoHandle InfoHandle::Copy() const
{
  return InfoHandle(impl_->Copy());
}

void InfoHandle::Import(const String& file)
{
  impl_->Import(file);
}

void InfoHandle::Export(const String& file) const
{
  impl_->Export(file);
}

bool InfoHandle::IsValid() const
{
  return impl_;
}


InfoGroup InfoHandle::Root() const
{
  if(!impl_) throw InfoError("Empty InfoHandle");
  return InfoGroup(*this,ElePtr(new detail::EleImpl((*impl_).Root())));
}

void InfoHandle::AddDefault(const InfoHandle& h)
{
  impl_->DefList().push_back(h.impl_);
}

bool InfoHandle::HasDefaultGroup(const InfoPath& p) const
{
  for(RootPtrList::const_iterator it=impl_->DefList().begin();it!=impl_->DefList().end();++it) {
    InfoGroup lroot(*it,ElePtr(new detail::EleImpl((*it)->Root())));
    if(lroot.HasGroup(p)) {
      return true;
    }
  }
  return false;
}

InfoGroup InfoHandle::GetDefaultGroup(const InfoPath& p) const
{
  for (RootPtrList::const_iterator it=impl_->DefList().begin();it!=impl_->DefList().end();++it) {
    InfoGroup lroot(*it,ElePtr(new detail::EleImpl((*it)->Root())));
    if(lroot.HasGroup(p)) {
      return lroot.GetGroup(p);
    }
  }
  throw InfoError("default group not found");
}

bool InfoHandle::HasDefaultItem(const InfoPath& p) const
{
  if(impl_->DefList().empty()) return false;
  for(RootPtrList::const_iterator it=impl_->DefList().begin();it!=impl_->DefList().end();++it) {
    InfoGroup lroot(*it,ElePtr(new detail::EleImpl((*it)->Root())));
    if(lroot.HasItem(p)) {
      return true;
    }
  }
  return false;
}

InfoItem InfoHandle::GetDefaultItem(const InfoPath& p) const
{
  for(RootPtrList::const_iterator it=impl_->DefList().begin();it!=impl_->DefList().end();++it) {
    InfoGroup lroot(*it,ElePtr(new detail::EleImpl((*it)->Root())));
    if(lroot.HasItem(p)) {
      return lroot.GetItem(p);
    }
  }
  throw InfoError("default item not found");
}

}} // ns
