//------------------------------------------------------------------------------
// This file is part of the OpenStructure project <www.openstructure.org>
//
// Copyright (C) 2008-2011 by the OpenStructure authors
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
#include <ost/invalid_handle.hh>
#include <ost/integrity_error.hh>
#include <ost/log.hh>
#include <ost/mol/in_mem_coord_source.hh>
#include <ost/mol/view_op.hh>
#include <ost/mol/mol.hh>
#include "coord_group.hh"


namespace ost { namespace mol {

CoordGroupHandle CreateCoordGroup(const AtomHandleList& atomlist)
{
  return CoordGroupHandle(CoordSourcePtr(new InMemCoordSource(atomlist)));
}

CoordGroupHandle::CoordGroupHandle():
  source_()
{}

CoordGroupHandle::CoordGroupHandle(CoordSourcePtr source):
  source_(source)
{
  this->CheckValidity();
}

bool CoordGroupHandle::IsValid() const
{
  this->CheckValidity();  
  return source_.get()!=0;
}

EntityHandle CoordGroupHandle::GetEntity() const
{
  this->CheckValidity();  
  return source_->GetEntity();
}

uint CoordGroupHandle::GetAtomCount() const
{
  this->CheckValidity();  
  return source_->GetAtomCount();
}

uint CoordGroupHandle::GetFrameCount() const
{
  this->CheckValidity();  
  return source_->GetFrameCount();
}

void CoordGroupHandle::SetFramePositions(uint frame, 
                                         const std::vector<geom::Vec3>& clist)
{
  this->CheckValidity();  
  //source_->SetFramePositions(frame, clist);
}

void CoordGroupHandle::CopyFrame(uint frame)
{
  this->CheckValidity();
  source_->CopyFrame(frame);
}

CoordGroupHandle::operator bool() const
{
  return this->IsValid();
}

void CoordGroupHandle::AddFrame(const std::vector<geom::Vec3>& clist)
{
  this->CheckValidity();
  if (source_->IsMutable()) {
    source_->AddFrame(clist);    
  } else {
    throw IntegrityError("Can't add frame to immutable CoordGroup");
  }
}

void CoordGroupHandle::AddFrames(const CoordGroupHandle& cg)
{
  this->CheckValidity();
  if (source_->IsMutable()) {
    if (cg.GetAtomCount()!=this->GetAtomCount()) {
      throw IntegrityError("Atom number don't match");
    }
    for (size_t i=0; i<cg.GetFrameCount(); ++i) {
      source_->AddFrame(*cg.GetFrame(i));
    }
  } else {
    throw IntegrityError("Can't add frame to immutable CoordGroup");
  }
}

void CoordGroupHandle::CheckValidity() const
{
  if (!source_) {
    throw InvalidHandle();
  }
}

void CoordGroupHandle::SetAtomPos(uint frame, AtomHandle atom, const geom::Vec3& pos)
{
  this->CheckValidity();
  source_->SetAtomPos(frame,atom,pos);
}

geom::Vec3 CoordGroupHandle::GetAtomPos(uint frame, AtomHandle atom) const
{
  this->CheckValidity();
  return source_->GetAtomPos(frame,atom);
}

CoordFramePtr CoordGroupHandle::GetFrame(uint frame) const
{
  this->CheckValidity();
  return source_->GetFrame(frame);
}

AtomHandleList CoordGroupHandle::GetAtomList() const
{
  this->CheckValidity();
  return source_->GetAtomList();
}

void CoordGroupHandle::Capture()
{
  this->CheckValidity();
  if (source_->IsMutable()) {
    source_->Capture();    
  } else {
    throw IntegrityError("Can't capture. CoordGroup is immutable");
  }  
}

void CoordGroupHandle::CaptureInto(int pos)
{
  this->CheckValidity();
  if (source_->IsMutable()) {
    source_->CaptureInto(pos);    
  } else {
    throw IntegrityError("Can't capture. CoordGroup is immutable");
  }  
}

void CoordGroupHandle::Capture(uint frame)
{
  this->CheckValidity();
  if (source_->IsMutable()) {
    source_->Capture(frame);    
  } else {
    throw IntegrityError("Can't capture. CoordGroup is immutable");
  }  
}

CoordGroupHandle CoordGroupHandle::Filter(const EntityView& selected) const
{
  this->CheckValidity();
  std::vector<unsigned long> indices;
  EntityHandle new_ent;  
  if (!selected.IsValid()) {
    indices.reserve(this->GetAtomCount());
    for (size_t i=0;i<this->GetAtomCount(); ++i) {
      indices.push_back(i);
    }
    new_ent=this->GetEntity().Copy();
  } else {
    AtomViewList atoms=selected.GetAtomList();
    indices.reserve(atoms.size());
    for (AtomViewList::const_iterator i=atoms.begin(), 
         e=atoms.end(); i!=e; ++i) {
      indices.push_back(i->GetIndex());
    }
    new_ent=CreateEntityFromView(selected, false);
  }

  CoordGroupHandle filtered_cg=CreateCoordGroup(new_ent.GetAtomList());
  std::vector<geom::Vec3> vecs(indices.size());
  for (size_t i=0; i<this->GetFrameCount(); ++i) {
    LOG_INFO("Filtering frame " << i << "/" << this->GetFrameCount());
    CoordFramePtr frame=this->GetFrame(i);
    for (std::vector<unsigned long>::const_iterator 
         j=indices.begin(), e2=indices.end(); j!=e2; ++j) {
      vecs[j-indices.begin()]=(*frame)[*j];
    }
    filtered_cg.AddFrame(vecs);
  }
  return filtered_cg;
}
 
  
}} // ns
