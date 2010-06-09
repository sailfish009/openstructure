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
#include "atom_base.hh"
#include <ost/mol/impl/residue_impl.hh>
#include <ost/mol/impl/atom_impl.hh>
#include <ost/invalid_handle.hh>

namespace ost { namespace mol {

AtomBase::AtomBase()
{ }

AtomBase::AtomBase(const impl::AtomImplPtr& impl):
  impl_(impl) 
{}
GenericPropContainerImpl* AtomBase::GpImpl()
{
  return Impl().get();
}

const GenericPropContainerImpl* AtomBase::GpImpl() const
{
  return Impl().get();
}

const String& AtomBase::GetName() const 
{
  this->CheckValidity();  
  return impl_->GetName();
}

void AtomBase::SetName(const String& atom_name) 
{
  this->CheckValidity();
  return impl_->SetName(atom_name);
}

const geom::Vec3& AtomBase::GetPos() const 
{
  this->CheckValidity();  
  return impl_->GetPos();
}

const geom::Vec3& AtomBase::GetOriginalPos() const
{
  this->CheckValidity();
  return impl_->GetOriginalPos();
}

geom::Vec3 AtomBase::GetAltPos(const String& alt_group) const
{
  this->CheckValidity();
  return impl_->GetResidue()->GetAltAtomPos(Impl(), alt_group);
}

std::vector<String> AtomBase::GetAltGroupNames() const
{
  this->CheckValidity();
  return impl_->GetResidue()->GetAltAtomGroupNames(Impl());
}


const AtomProp& AtomBase::GetAtomProps() const 
{
  this->CheckValidity();  
  return impl_->GetAtomProps();
}

void AtomBase::SetAtomProps(const AtomProp& prop) 
{
  this->CheckValidity();  
  impl_->GetAtomProps()=prop;
}

impl::AtomImplPtr& AtomBase::Impl() 
{
  return impl_;
}

const impl::AtomImplPtr& AtomBase::Impl() const 
{
  return impl_;
}

bool AtomBase::IsValid() const 
{
  return (impl_.get()!=0);
}

AtomBase::operator bool() const 
{
  return (impl_.get()!=0);
}

String AtomBase::GetQualifiedName() const 
{
  this->CheckValidity();
  return Impl()->GetQualifiedName();
}

void AtomBase::CheckValidity() const 
{
  if (!impl_)
    throw InvalidHandle();
}

std::ostream& operator<<(std::ostream& os, const AtomBase& atom) 
{
  if (atom.IsValid()) {
    os << atom.GetQualifiedName();
  } else {
    os << "invalid atom";
  }
  return os;
}


Real AtomBase::GetRadius() const
{
  this->CheckValidity();
  return Impl()->GetAtomProps().radius;
}


const String& AtomBase::GetElement() const
{
  this->CheckValidity();
  return Impl()->GetAtomProps().element;
}


bool AtomBase::IsHetAtom() const
{
  this->CheckValidity();
  return Impl()->GetAtomProps().is_hetatm;
}

Real AtomBase::GetBFactor() const
{
  this->CheckValidity();
  return Impl()->GetAtomProps().b_factor;
}

Real AtomBase::GetMass() const
{
  this->CheckValidity();
  return Impl()->GetAtomProps().mass;
}

Real AtomBase::GetCharge() const
{
  this->CheckValidity();
  return Impl()->GetAtomProps().charge;
}

Real AtomBase::GetOccupancy() const
{
  this->CheckValidity();
  return Impl()->GetAtomProps().occupancy;
}


String AtomBase::GetStringProperty(Prop::ID prop_id) const
{
  this->CheckValidity();
  return Impl()->GetStringProperty(prop_id);
}
                      

Real AtomBase::GetFloatProperty(Prop::ID prop_id) const
{
  this->CheckValidity();
  return Impl()->GetFloatProperty(prop_id);
}

int AtomBase::GetIntProperty(Prop::ID prop_id) const
{
  this->CheckValidity();
  return Impl()->GetIntProperty(prop_id);
}

unsigned long AtomBase::GetIndex() const
{
  this->CheckValidity();
  return Impl()->GetIndex();
}

}} // ns

