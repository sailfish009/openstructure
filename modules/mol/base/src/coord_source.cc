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
#include <ost/log.hh>
#include <ost/mol/atom_handle.hh>
#include <ost/mol/xcs_editor.hh>
#include <ost/mol/in_mem_coord_source.hh>

#include "coord_source.hh"


namespace ost { namespace mol {
  

CoordSource::CoordSource(const AtomHandleList& atoms):
  atoms_(atoms), 
  entity_(),
  mutable_(false),
  atom_dict_()
{
  if (!atoms_.empty()) {
    entity_=atoms_.front().GetEntity();
  }
  for(uint n=0;n<atoms_.size();++n) {
    atom_dict_[atoms_[n].GetHashCode()]=n;
  }
}

  
CoordSource::~CoordSource()
{
  
}

void CoordSource::CopyFrame(uint frame_id)
{
  if (atoms_.empty()) {
    LOGN_DEBUG("atom list empty, ignored");
    return;
  }  
  CoordFramePtr frame=this->GetFrame(frame_id);
  if (!frame) {
    LOGN_DEBUG("invalid frame given, ignored");
    return;
  }
  assert(frame->size()==atoms_.size());
  XCSEditor edi=atoms_.front().GetEntity().RequestXCSEditor(BUFFERED_EDIT);  
  CoordFrame::const_iterator c=frame->begin();
  for (AtomHandleList::iterator i=atoms_.begin(), 
       e=atoms_.end(); i!=e; ++i, ++c) {
     edi.SetAtomPos(*i, *c);
  }
}

int CoordSource::GetAtomCount() const
{
  return atoms_.size();
}

void CoordSource::Capture()
{
  std::vector<geom::Vec3> coords;
  coords.reserve(atoms_.size());
  for (AtomHandleList::const_iterator i=atoms_.begin(), 
       e=atoms_.end(); i!=e; ++i) {
    coords.push_back(i->GetPos());
  }
  this->AddFrame(coords);
}

void CoordSource::Capture(uint f)
{
  CoordFrame& fp=*(GetFrame(f));
  if(fp.size()!=atoms_.size()) {
    return;
  }
  unsigned int count=0;
  for (AtomHandleList::const_iterator i=atoms_.begin(); i!=atoms_.end(); ++i) {
    fp[count++]=i->GetPos();
  }
}

CoordSourcePtr CoordSource::Extract(int start, int stop, int step)
{
  InMemCoordSourcePtr in_mem_source(new InMemCoordSource(atoms_));
  int frame_count=this->GetFrameCount();
  if (stop==-1) {
    stop=frame_count-1;
  }
  int start2=std::min(static_cast<int>(frame_count)-1,std::max(0,start));
  int end2=std::min(static_cast<int>(frame_count)-1,std::max(0,stop));
  for (int i=start2;i<=end2;i+=step) {
    in_mem_source->AddFrame(this->GetFrame(i));
  }
  return in_mem_source;
}

EntityHandle CoordSource::GetEntity() const
{
  return entity_;
}

bool CoordSource::IsMutable() const
{
 return mutable_; 
}

void CoordSource::AddFrame(const std::vector<geom::Vec3>& coords)
{
  assert(0 && "implement me");
}

void CoordSource::SetMutable(bool flag)
{
  mutable_=flag;
}

const AtomHandleList& CoordSource::GetAtomList() const
{
  return atoms_;
}

void CoordSource::SetAtomPos(uint frame, AtomHandle atom, const geom::Vec3& pos)
{
  if(!atom.IsValid()) return;
  std::map<long,uint>::iterator it = atom_dict_.find(atom.GetHashCode());
  if(it!=atom_dict_.end()) {
    CoordFrame& fp=*(GetFrame(frame));
    fp[it->second]=pos;
  }
}

geom::Vec3 CoordSource::GetAtomPos(uint frame, AtomHandle atom) const
{
  if(!atom.IsValid()) return geom::Vec3();
  std::map<long,uint>::const_iterator it = atom_dict_.find(atom.GetHashCode());
  if(it!=atom_dict_.end()) {
    const CoordFrame& fp=*(GetFrame(frame));
    return fp[it->second];
  }
  return geom::Vec3();
}

}} // ns
