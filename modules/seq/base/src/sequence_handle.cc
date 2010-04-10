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

/*
  Author: Marco Biasini
 */

#include <ost/invalid_handle.hh>
#include <ost/seq/sequence_handle.hh>
#include <ost/seq/impl/sequence_impl.hh>


namespace ost { namespace seq {

ConstSequenceHandle::ConstSequenceHandle()
{ }

SequenceHandle::SequenceHandle(const impl::SequenceImplPtr& impl):
  ConstSequenceHandle(impl)
{ }

ConstSequenceHandle::ConstSequenceHandle(const impl::SequenceImplPtr& impl):
  impl_(impl)
{
  
}

bool ConstSequenceHandle::operator==(const ConstSequenceHandle& rhs) const
{
  return impl_==rhs.impl_;
}

bool ConstSequenceHandle::operator!=(const ConstSequenceHandle& rhs) const
{
  return impl_!=rhs.impl_;
}


void ConstSequenceHandle::CheckValidity() const
{
  if (!impl_) {
    throw InvalidHandle();
  }
}

bool ConstSequenceHandle::IsValid() const
{
  return impl_.get()!=0;
}

impl::SequenceImplPtr& ConstSequenceHandle::Impl() const 
{
  return impl_;
}


SequenceHandle CreateSequence(const String& name, const String& seq)
{
  return SequenceHandle(impl::SequenceImpl::FromString(name, seq));
}

SequenceHandle::SequenceHandle():
  ConstSequenceHandle()
{ }


String ConstSequenceHandle::GetGaplessString() const
{
  this->CheckValidity();
  return Impl()->GetGaplessString();
}

int ConstSequenceHandle::GetSequenceOffset() const
{
  this->CheckValidity();
  return Impl()->GetSequenceOffset();  
}


int ConstSequenceHandle::GetLength() const
{
  this->CheckValidity();
  return Impl()->GetLength();
}


char ConstSequenceHandle::GetOneLetterCode(int position) const
{
  this->CheckValidity();
  return Impl()->GetOneLetterCode(position);
}

mol::ResidueView ConstSequenceHandle::GetResidue(int position) const
{
  this->CheckValidity();
  return Impl()->GetResidue(position);  
}

mol::EntityView ConstSequenceHandle::GetAttachedView() const
{
  this->CheckValidity();
  return Impl()->GetAttachedView(); 
}

SequenceHandle ConstSequenceHandle::Copy() const
{
  this->CheckValidity();
  return Impl()->Copy();     
}

bool ConstSequenceHandle::HasAttachedView() const
{
  this->CheckValidity();
  return Impl()->HasAttachedView();   
}


int ConstSequenceHandle::GetResidueIndex(int pos) const
{
  this->CheckValidity();
  return Impl()->GetResidueIndex(pos);   
}

int ConstSequenceHandle::GetPos(int residue_index) const
{
  this->CheckValidity();
  return Impl()->GetPos(residue_index);   
}


int ConstSequenceHandle::GetFirstNonGap() const
{
  this->CheckValidity();
  return Impl()->GetFirstNonGap(); 
}


int ConstSequenceHandle::GetLastNonGap() const
{
  this->CheckValidity();
  return Impl()->GetLastNonGap(); 
}

const String& ConstSequenceHandle::GetName() const
{
  this->CheckValidity();
  return Impl()->GetName();
}

const String& ConstSequenceHandle::GetString() const
{
  this->CheckValidity();
  return Impl()->GetString(); 
}

void SequenceHandle::SetName(const String& name)
{
  this->CheckValidity();
  return Impl()->SetName(name);   
}



void SequenceHandle::SetString(const String& seq)
{
  this->CheckValidity();
  return Impl()->SetString(seq);   
}

void SequenceHandle::SetSequenceOffset(int offset)
{
  this->CheckValidity();
  return Impl()->SetSequenceOffset(offset);   
}


void SequenceHandle::AttachView(const mol::EntityView& view)
{
  this->CheckValidity();
  Impl()->AttachView(view);   
}


void SequenceHandle::AttachView(const mol::EntityView& view, 
                                const String& chain_name)
{
  this->CheckValidity();
  Impl()->AttachView(view, chain_name);   
}

/// \brief export sequence to info
void  SequenceToInfo(const ConstSequenceHandle& sequence,
                     info::InfoGroup& group)
{
  impl::SequenceImplToInfo(sequence.Impl(), group);
}

/// \brief create sequence from info
SequenceHandle SequenceFromInfo(info::InfoGroup& group)
{
  return SequenceHandle(impl::SequenceImplFromInfo(group));
}

std::ostream& operator<<(std::ostream& os, const ConstSequenceHandle& sequence)
{
  if (sequence.IsValid()) {
    os << sequence.GetName() << ": " << sequence.GetString();    
  } else {
    os << "invalid";
  }

  return os;
}

}}
