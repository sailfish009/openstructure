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
#include <ost/invalid_handle.hh>
#include <ost/integrity_error.hh>

#include <ost/mol/in_mem_coord_source.hh>
#include <ost/mol/entity_handle.hh>

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

}} // ns
