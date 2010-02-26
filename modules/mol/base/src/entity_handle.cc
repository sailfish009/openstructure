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

#include "impl/entity_impl.hh"
#include "entity_visitor.hh"
#include "atom_handle.hh"
#include "chain_handle.hh"
#include "entity_handle.hh"
#include "iterator.hh"
#include "impl/chain_impl.hh"
#include "impl/atom_impl.hh"
#include "impl/residue_impl.hh"
#include "xcs_editor.hh"
#include "ics_editor.hh"

namespace ost { namespace mol {


EntityHandle::EntityHandle() 
{
}

EntityHandle CreateEntity() {
  return EntityHandle(impl::EntityImplPtr(new impl::EntityImpl()));
}

geom::Vec3 EntityHandle::GetCenterOfMass() const {
  this->CheckValidity();
  return Impl()->GetCenterOfMass();
}

Real EntityHandle::GetMass() const {
  this->CheckValidity();
  return Impl()->GetMass();
}

geom::Vec3 EntityHandle::GetCenterOfAtoms() const {
  this->CheckValidity();
  return Impl()->GetCenterOfAtoms();
}

geom::Vec3 EntityHandle::GetGeometricCenter() const {
  this->CheckValidity();
  return Impl()->GetGeometricCenter();
}

geom::Vec3 EntityHandle::GetGeometricStart() const {
  this->CheckValidity();
  return Impl()->GetGeometricStart();
}

geom::Vec3 EntityHandle::GetGeometricEnd() const {
  this->CheckValidity();
  return Impl()->GetGeometricEnd();
}

geom::Vec3 EntityHandle::GetBoundarySize() const {
  this->CheckValidity();
  return Impl()->GetBoundarySize();
}

EntityHandle::EntityHandle(const impl::EntityImplPtr& e)
  : EntityBase(e)
{}

EntityHandle EntityHandle::Copy() const {
  this->CheckValidity();
  return EntityHandle(Impl()->Copy());
}

void EntityHandle::Swap(EntityHandle& eh) {
  this->CheckValidity();
  impl::EntityImplPtr& other=eh.Impl();
  Impl().swap(other);
}

void EntityHandle::DeepSwap(EntityHandle& eh) {
  this->CheckValidity();
  Impl()->Swap(*eh.Impl());
  Impl()->NotifyObserver();
  eh.Impl()->NotifyObserver();
}

void EntityHandle::AttachObserver(const EntityObserverPtr& o) {
  this->CheckValidity();
  Impl()->AttachObserver(o);
}

void EntityHandle::DetachObserver(const EntityObserverPtr& o) {
  this->CheckValidity();
  Impl()->DetachObserver(o);
}


void EntityHandle::Apply(EntityVisitor& v)
{
  this->CheckValidity();
  Impl()->Apply(v);
}

AtomHandleList EntityHandle::FindWithin(const geom::Vec3& pos,
                                        Real radius) const {
  this->CheckValidity();
  impl::AtomImplList atoms=Impl()->FindWithin(pos,radius);
  AtomHandleList handles;
  handles.reserve(atoms.size());
  for (size_t i=0; i<atoms.size(); ++i) {
    handles.push_back(atoms[i]);
  }
  return handles;
}

EntityView EntityHandle::Select(const Query& q, QueryFlags flags) const
{
  this->CheckValidity();
  return Impl()->Select(*this, q, flags);
}

EntityView EntityHandle::Select(const String& q, QueryFlags flags) const {
  this->CheckValidity();
  return Impl()->Select(*this, Query(q), flags);
}

EntityView EntityHandle::CreateFullView() const
{
  this->CheckValidity();
  return Impl()->CreateFullView(*this);
}

bool EntityHandle::operator==(const EntityHandle& ref) const
{
  return Impl()==ref.Impl();
}

bool EntityHandle::operator!=(const EntityHandle& ref) const
{
  return Impl()!=ref.Impl();
}

ChainHandleList EntityHandle::GetChainList() const
{
  this->CheckValidity();
  ChainHandleList chainlist;
  const impl::ChainImplList& cl = Impl()->GetChainList();
  impl::ChainImplList::const_iterator i;
  chainlist.reserve(cl.size());
  for (i=cl.begin(); i!=cl.end(); ++i) {
    chainlist.push_back(ChainHandle(*i));
  }
  return chainlist;
}

TorsionHandle EntityHandle::FindTorsion(const AtomHandle& a1,
                                        const AtomHandle& a2,
                                        const AtomHandle& a3,
                                        const AtomHandle& a4) const {
  this->CheckValidity();
  return Impl()->FindTorsion(a1.Impl(), a2.Impl(), a3.Impl(), a4.Impl());
}

ChainHandle EntityHandle::FindChain(const String& name) const {
  this->CheckValidity();
  return Impl()->FindChain(name);
}

EntityView EntityHandle::CreateEmptyView() const {
  this->CheckValidity();
  return EntityView(*this);
}

int EntityHandle::GetAtomCount() const {
  this->CheckValidity();
  return Impl()->GetAtomCount();
}

mol::BondHandleList EntityHandle::GetBondList() const {
  this->CheckValidity();
  return Impl()->GetBondList();
}

int EntityHandle::GetBondCount() const {
  this->CheckValidity();
  return Impl()->GetBondCount();
}

int EntityHandle::GetResidueCount() const {
  this->CheckValidity();
  return Impl()->GetResidueCount();
}

int EntityHandle::GetChainCount() const {
  this->CheckValidity();
  return Impl()->GetChainCount();
}

Real EntityHandle::GetAngle(const AtomHandle& a1, const AtomHandle& a2,
                              const AtomHandle& a3) const {
  this->CheckValidity();
  return Impl()->GetAngle(a1.Impl(), a2.Impl(), a3.Impl());
}

const geom::Mat4& EntityHandle::GetTransformationMatrix() const

{
  this->CheckValidity();
  return Impl()->GetTransfMatrix();
}


const geom::Mat4& EntityHandle::GetInvTransformationMatrix() const
{
  this->CheckValidity();
  return Impl()->GetInvTransfMatrix();
}

bool EntityHandle::IsTransformationIdentity() const
{
  this->CheckValidity();
  return Impl()->IsTransfIdentity();
}

ResidueHandle EntityHandle::FindResidue(const String& chain_name,
                                        const ResNum& residue) const {
  this->CheckValidity();
  return Impl()->FindResidue(chain_name, residue);
}

AtomHandle EntityHandle::FindAtom(const String& chain_name,
                                  const ResNum& residue,
                                  const String& atom_name) const {
  this->CheckValidity();
  return Impl()->FindAtom(chain_name, residue, atom_name);
}


ResidueHandleIter EntityHandle::ResiduesBegin() const {
  this->CheckValidity();
  if (Impl()->GetChainMap().empty()) {
    return ResidueHandleIter();
  }
  impl::EntityImplPtr i=Impl();
  impl::ChainImplPtr chain=i->GetChainMap().begin()->second;
  return ResidueHandleIter(i->GetChainMap().begin(),
                           chain->GetResidueList().begin(), i);
}

ResidueHandleIter EntityHandle::ResiduesEnd() const {
  this->CheckValidity();
  if (Impl()->GetChainMap().empty()) {
    return ResidueHandleIter();
  }
  impl::EntityImplPtr i=Impl();
  impl::ChainImplPtr chain=i->GetChainMap().rbegin()->second;
  return ResidueHandleIter(i->GetChainMap().end(),
                           chain->GetResidueList().end(), i);
}

ChainHandleIter EntityHandle::ChainsBegin() const {
  this->CheckValidity();
  return ChainHandleIter(Impl()->GetChainMap().begin());
}


ChainHandleIter EntityHandle::ChainsEnd() const {
  return ChainHandleIter(Impl()->GetChainMap().end());
}


AtomHandleIter EntityHandle::AtomsBegin() const {
  this->CheckValidity();
  impl::EntityImplPtr ent=Impl();
  if (ent->GetChainMap().empty()) {
    return AtomHandleIter();
  }
  impl::ResidueImplList& r=ent->GetChainMap().begin()->second->GetResidueList();
  if (r.empty()) {
    return AtomHandleIter();
  }
  return AtomHandleIter(ent->GetChainMap().begin(), r.begin(),
                        r.front()->GetAtomList().begin(), ent, true);
}

AtomHandleIter EntityHandle::AtomsEnd() const 
{
  this->CheckValidity();
  impl::EntityImplPtr ent=Impl();
  if (ent->GetChainMap().empty()) {
    return AtomHandleIter();
  }
  impl::ResidueImplList& r=ent->GetChainMap().rbegin()->second->GetResidueList();
  if (r.empty()) {
    return AtomHandleIter();
  }
  return AtomHandleIter(ent->GetChainMap().end(), r.end(),
                        r.back()->GetAtomList().end(), ent, false);
}

XCSEditor EntityHandle::RequestXCSEditor(EditMode mode) const
{
  this->CheckValidity();
  return XCSEditor(*this, mode);
}

ICSEditor EntityHandle::RequestICSEditor(EditMode mode) const
{
  this->CheckValidity();
  Impl()->EnableICS();
  return ICSEditor(*this, mode);
}


ResidueHandleList EntityHandle::GetResidueList() const
{
  this->CheckValidity();
  ResidueHandleList residues;
  std::copy(ResiduesBegin(), ResiduesEnd(), std::back_inserter(residues));
  return residues;
}


AtomHandleList EntityHandle::GetAtomList() const
{
  this->CheckValidity();
  AtomHandleList atoms;
  std::copy(AtomsBegin(), AtomsEnd(), std::back_inserter(atoms));
  return atoms;
}

EntityHandle EntityHandle::GetHandle() const
{
  return *this;
}
}} // ns
