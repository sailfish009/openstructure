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
#include "chain_handle.hh"
#include <ost/mol/entity_handle.hh>
#include <ost/mol/residue_handle.hh>
#include <ost/mol/impl/chain_impl.hh>
#include <ost/mol/impl/residue_impl.hh>
#include <ost/mol/iterator.hh>
#include <ost/mol/impl/entity_impl.hh>
namespace ost { namespace mol {


EntityHandle ChainHandle::GetEntity() const
{
  this->CheckValidity();  
  return EntityHandle(Impl()->GetEntity());
}

ChainHandle::ChainHandle(const impl::ChainImplPtr& impl) 
  : ChainBase(impl) {
}

ChainHandle::ChainHandle() {
}

void ChainHandle::Apply(EntityVisitor& v)
{
  this->CheckValidity();    
  Impl()->Apply(v);
}

ResidueHandle ChainHandle::GetPrev(const ResidueHandle& rh)
{
  this->CheckValidity();    
  impl::ResidueImplPtr ri = Impl()->GetPrev(rh.Impl());
  if(ri) {
    return ResidueHandle(ri);
  }
  return ResidueHandle(); // invalid handle indicates failure
}

int ChainHandle::GetAtomCount() const {
  this->CheckValidity();    
  return Impl()->GetAtomCount();
}

int ChainHandle::GetBondCount() const {
  this->CheckValidity();    
  return Impl()->GetBondCount();
}

int ChainHandle::GetResidueCount() const {
  this->CheckValidity();  
  return Impl()->GetResidueCount();
}

ResidueHandle ChainHandle::GetNext(const ResidueHandle& rh)
{
  this->CheckValidity();  
  impl::ResidueImplPtr ri = Impl()->GetNext(rh.Impl());
  if(ri) {
    return ResidueHandle(ri);
  }
  return ResidueHandle(); // invalid handle indicates failure
}

ResidueHandle ChainHandle::FindResidue(const ResNum& num) const {
  this->CheckValidity();    
  return ResidueHandle(Impl()->FindResidue(num));
}

AtomHandle ChainHandle::FindAtom(const ResNum& num,
                                 const String& atom_name) const {
  this->CheckValidity();                                     
  return Impl()->FindAtom(num, atom_name);
}


AtomHandleList ChainHandle::GetAtomList() const
{
  this->CheckValidity();
  AtomHandleList atoms;
  std::copy(AtomsBegin(), AtomsEnd(), std::back_inserter(atoms));
  return atoms;
}

ResidueHandleList ChainHandle::GetResidueList() const
{
  ResidueHandleList reslist;
  this->CheckValidity();  
  const impl::ResidueImplList& rl = Impl()->GetResidueList();
  reslist.reserve(rl.size());
  for(impl::ResidueImplList::const_iterator it=rl.begin();it!=rl.end();++it) {
    reslist.push_back(ResidueHandle(*it));
  }
  return reslist;
}

bool ChainHandle::operator==(const ChainHandle& ref) const
{
  return Impl()==ref.Impl();
}

bool ChainHandle::operator!=(const ChainHandle& ref) const
{
  return Impl()!=ref.Impl();
}

ResidueHandleIter ChainHandle::ResiduesBegin() const {
  this->CheckValidity();
  impl::ChainImplPtr c=Impl();
  return ResidueHandleIter(c->GetEntity()->GetChainIter(this->GetName()), 
                           impl::begin(c->GetResidueList()), 
                           c->GetEntity(), true);
}

ResidueHandleIter ChainHandle::ResiduesEnd() const {
  this->CheckValidity();
  impl::ChainImplPtr c=Impl();
  impl::pointer_it<impl::ChainImplPtr> cc=c->GetEntity()->GetChainIter(this->GetName());
  /*impl::pointer_it<impl::ChainImplPtr> nc=cc; ++nc;
  
  if (nc!=impl::end(c->GetEntity()->GetChainList())) {
    return ResidueHandleIter(nc, (*nc)->GetResidueList().begin(), 
                             c->GetEntity(), true);    
  } else {
    return ResidueHandleIter(cc, c->GetResidueList().end(), 
                             c->GetEntity(), true);    
  }*/
  return ResidueHandleIter(cc, impl::end(c->GetResidueList()), 
                           c->GetEntity(), true);
}

AtomHandleIter ChainHandle::AtomsBegin() const 
{
  this->CheckValidity();
  impl::ChainImplPtr c=Impl();
  if (c->GetResidueList().empty()) {

    return AtomHandleIter();
  }  
  impl::pointer_it<impl::ChainImplPtr> cc=c->GetEntity()->GetChainIter(this->GetName()); 
  return AtomHandleIter(cc, impl::begin(c->GetResidueList()),
                        impl::begin(c->GetResidueList().front()->GetAtomList()),
                        c->GetEntity(), true);    

}

AtomHandleIter ChainHandle::AtomsEnd() const {
  this->CheckValidity();
  impl::ChainImplPtr c=Impl();
  if (c->GetResidueList().empty()) {
    return AtomHandleIter();
  }  
  impl::pointer_it<impl::ChainImplPtr> cc=c->GetEntity()->GetChainIter(this->GetName());
  impl::pointer_it<impl::ChainImplPtr> nc=cc; ++nc;
  impl::ResidueImplList& rc=(*nc)->GetResidueList();
  if (nc!=impl::end(c->GetEntity()->GetChainList())) {  
    return AtomHandleIter(nc, impl::begin(rc), 
                          impl::begin(rc.front()->GetAtomList()),
                          c->GetEntity(), false);
  } else {
    return AtomHandleIter(cc, impl::end(c->GetResidueList()),
                          impl::end(c->GetResidueList().back()->GetAtomList()),
                          c->GetEntity(), false);
  }  
}

ResidueHandle ChainHandle::GetResidueByIndex(int index) const {
  this->CheckValidity();
  if (index<0 || !(static_cast<int>(Impl()->GetResidueList().size())>index))
    return ResidueHandle();
  return ResidueHandle((Impl()->GetResidueList())[index]);
}

void ChainHandle::AssignSecondaryStructure(SecStructure ss, 
                                           const ResNum& start, 
                                           const ResNum& end)
{
  this->CheckValidity();
  Impl()->AssignSecondaryStructure(ss, start, end);
}

geom::AlignedCuboid ChainHandle::GetBounds() const
{
  this->CheckValidity();
  return Impl()->GetBounds();
}

geom::Vec3 ChainHandle::GetCenterOfMass() const
{
  this->CheckValidity();
  return Impl()->GetCenterOfMass();
}

geom::Vec3 ChainHandle::GetCenterOfAtoms() const
{
  this->CheckValidity();
  return Impl()->GetCenterOfAtoms();
}

Real ChainHandle::GetMass() const
{
  this->CheckValidity();
  return Impl()->GetMass();
}

bool ChainHandle::InSequence() const
{
  this->CheckValidity();
  return Impl()->InSequence();
}

ChainHandle ChainHandle::GetHandle() const
{
  return *this;
}

EntityView ChainHandle::Select(const Query& q, QueryFlags flags) const
{
  this->CheckValidity();
  if (q.GetQueryString() != "") {
    return this->GetEntity().Select(Query("cname='"+Impl()->GetName()+"' and ("+
                                        q.GetQueryString()+")"), flags);
  }
  else return this->GetEntity().Select(Query("cname='"+Impl()->GetName()+"'"), flags);
}

EntityView ChainHandle::Select(const String& q, QueryFlags flags) const {
  this->CheckValidity();
  if (q != "") {
    return this->GetEntity().Select(Query("cname='"+Impl()->GetName()+"' and ("+
                                        q+")"), flags);
  }
  else return this->GetEntity().Select(Query("cname='"+Impl()->GetName()+"'"), flags);
}

void ChainHandle::SetInSequence(const int index)
{
  this->CheckValidity();    
  Impl()->SetInSequence(index);
}

}}

