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

#include <ost/mol/mol.hh>

#include "backbone_trace.hh"

/*
  Authors: Ansgar Philippsen, Marco Biasini
 */
 
namespace ost { namespace gfx { namespace impl {

namespace {

bool in_sequence(const mol::ResidueHandle& r1, const mol::ResidueHandle& r2)
{
  if(!r1.IsValid() || !r2.IsValid()) return false;
  if(r1.GetChain()!=r2.GetChain()) return false;
  mol::ResNum n1 = r1.GetNumber();
  mol::ResNum n2 = r2.GetNumber();
  if(n2.GetInsCode()!='\0') {
    if(n1.NextInsertionCode()==n2) return true;
  }
  if(mol::InSequence(r1,r2)) return true;
  // perhaps this fallback is not so good...
  if(n1.GetNum()+1==n2.GetNum()) return true;
  return false;
}

} // anon ns

class TraceBuilder: public mol::EntityVisitor {
public:
  TraceBuilder(BackboneTrace* bb_trace):
    backbone_trace_(bb_trace),
    last_residue_(),
    list_(),
    id_counter_(0)
  {}

  virtual bool VisitChain(const mol::ChainHandle& chain)
  {
    if(last_chain_ && chain!=last_chain_) {
      if(!list_.empty()) {
        backbone_trace_->AddNodeEntryList(list_);
        list_.clear();
      }
      last_chain_=chain;
    }
    return true;
  }

  virtual bool VisitResidue(const mol::ResidueHandle& res)
  {
    // check in-sequence
    bool in_seq=in_sequence(last_residue_,res);
    if(!in_seq) {
      if(!list_.empty()) {
        backbone_trace_->AddNodeEntryList(list_);
        list_.clear();
      }
    }
    // determine atom to add to list
    mol::AtomHandle ca = res.GetCentralAtom();
    if (ca) {
      NodeEntry entry={ca, GfxObj::Ele2Color(ca.GetElement()),
                       GfxObj::Ele2Color(ca.GetElement()),
                       geom::Vec3(), // this will be set by the gfx trace obj
                       res.GetCentralNormal(),
                       1.0,
                       geom::Vec3(),geom::Vec3(),geom::Vec3(), // for later use in NA rendering
                       false,id_counter_++};
      list_.push_back(entry);
    }

    last_residue_=res;

    return false;
  }

  virtual void OnExit()
  {
    if (!list_.empty()) {
      backbone_trace_->AddNodeEntryList(list_);
      list_.clear();
    }
  }
private:
  BackboneTrace*     backbone_trace_;
  mol::ResidueHandle last_residue_;
  mol::ChainHandle   last_chain_;
  NodeEntryList      list_;
  int                id_counter_;
};

BackboneTrace::BackboneTrace()
{}

BackboneTrace::BackboneTrace(const mol::EntityView& ent)
{
  view_=ent;
  Rebuild();
}

void BackboneTrace::ResetView(const mol::EntityView& ent)
{
  view_=ent;
  Rebuild();
}

int BackboneTrace::GetListCount() const
{
  return node_list_list_.size();
}

const NodeEntryList& BackboneTrace::GetList(int index) const
{
  return node_list_list_[index];
}

NodeEntryList& BackboneTrace::GetList(int index)
{
  return node_list_list_[index];
}

void BackboneTrace::Rebuild()
{
  if (view_) {
    node_list_list_.clear();
    TraceBuilder trace(this);    
    view_.Apply(trace);
  }
}

void BackboneTrace::OnUpdatedPositions()
{
  for(NodeEntryListList::iterator nitnit=node_list_list_.begin();nitnit!=node_list_list_.end();++nitnit) {
    NodeEntryList& nlist=*nitnit;
    for(NodeEntryList::iterator nit=nlist.begin();nit!=nlist.end();++nit) {
      nit->normal=nit->atom.GetResidue().GetCentralNormal();
    }
    PrepList(nlist);
  }
}

void BackboneTrace::AddNodeEntryList(const NodeEntryList& l)
{
  if(l.size()>=3) {
    node_list_list_.push_back(l);
    PrepList(node_list_list_.back());
  }
}

void BackboneTrace::PrepList(NodeEntryList& nelist)
{
  // orthogonalize the residue normals with
  // twist detection; important for later
  // spline generation
  if(nelist.size()<3) return;
  NodeEntry* e0=&nelist[0];
  NodeEntry* e1=&nelist[1];
  NodeEntry* e2=&nelist[2];
  geom::Vec3 p0 = e0->atom.GetPos();
  geom::Vec3 p1 = e1->atom.GetPos();
  geom::Vec3 p2 = e2->atom.GetPos();

  geom::Vec3 dir=geom::Normalize(p1-p0);
  e0->direction=geom::Normalize(p1-p0);
  geom::Vec3 orth=geom::Normalize(geom::Cross(e0->direction,e0->normal));
  geom::Vec3 norm=geom::Normalize(geom::Cross(orth,dir));
  e0->normal=norm;
  // start loop with the second residue
  unsigned int i=1;
  for(;i<nelist.size()-1;++i) {
    geom::Vec3 p10=p0-p1;
    geom::Vec3 p12=p2-p1;
    dir=geom::Normalize(p2-p0);
    e1->direction = dir;
    orth=geom::Normalize(geom::Cross(dir,e1->normal));
    norm=geom::Normalize(geom::Cross(orth,dir));
    // twist check
    if(geom::Dot(geom::Cross(e0->normal,dir),geom::Cross(norm,dir))<0.0) {
      norm=-norm;
    }
    e1->normal=norm;
    // skip over shift for the last iteration
    if(i==nelist.size()-2) break;
    // shift to i+1 for next iteration
    e0=&nelist[i];
    e1=&nelist[i+1];
    e2=&nelist[i+2];
    p0 = e0->atom.GetPos();
    p1 = e1->atom.GetPos();
    p2 = e2->atom.GetPos();
  }
  dir=geom::Normalize(p2-p1);
  e2->direction=dir;
  orth=geom::Normalize(geom::Cross(dir,e2->normal));
  norm=geom::Normalize(geom::Cross(orth,dir));
  if(geom::Dot(geom::Cross(e1->normal,dir),geom::Cross(norm,dir))<0.0) {
    norm=-norm;
  }
  e2->normal=norm;
}

BackboneTrace BackboneTrace::CreateSubset(const mol::EntityView& subview)
{
  BackboneTrace nrvo;
  nrvo.view_=subview;
  nrvo.node_list_list_.clear();
  for(NodeEntryListList::const_iterator nitnit=node_list_list_.begin();nitnit!=node_list_list_.end();++nitnit) {
    NodeEntryList new_nlist;
    const NodeEntryList& nlist=*nitnit;
    for(NodeEntryList::const_iterator nit=nlist.begin();nit!=nlist.end();++nit) {
      if(subview.FindAtom(nit->atom).IsValid()) {
        new_nlist.push_back(*nit);
      }
    }
    if(!new_nlist.empty()) {
      nrvo.node_list_list_.push_back(new_nlist);
    }
  }
  return nrvo;
}

}}} // ns

