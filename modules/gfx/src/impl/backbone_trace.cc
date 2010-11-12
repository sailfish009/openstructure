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

void BackboneTrace::AddNodeEntryList(const NodeEntryList& l)
{
  if(l.size()>=3) {
    node_list_list_.push_back(l);
    PrepList(node_list_list_.back());
  }
}

void BackboneTrace::PrepList(NodeEntryList& nelist)
{
  // assign direction and normal vectors for each entry
  // they are composed of the i-1 -> i and i->i+1 directions
  //
  // this same algorithm is used in the spline generation, so
  // perhaps all of this here is not necessary ?!
  //
  NodeEntry* e0=&nelist[0];
  NodeEntry* e1=&nelist[1];
  NodeEntry* e2=&nelist[2];
  geom::Vec3 p0 = e0->atom.GetPos();
  geom::Vec3 p1 = e1->atom.GetPos();
  geom::Vec3 p2 = e2->atom.GetPos();
  
  e0->direction=geom::Normalize(p1-p0);
  // e0->normal is set afterwards to normal of second one
  // backup residue normal
  e0->v1 = e0->normal;
  
  //reference normal to avoid twisting
  geom::Vec3 nref=geom::Normalize(geom::Cross(p0-p1,p2-p1));
  
  // start loop with the second
  unsigned int i=1;
  for(;i<nelist.size()-1;++i) {
    geom::Vec3 p10 = p0-p1;
    geom::Vec3 p12 = p2-p1;
    if(p10==-p12 || p10==p12) p12+=geom::Vec3(0.001,0.001,0.001);
    e1->v1=e1->normal;
    e1->normal=geom::Normalize(geom::Cross(p10,p12));

    // twist avoidance
    if(geom::Dot(geom::Normalize(geom::Cross(nref,p10)),
    		 geom::Normalize(geom::Cross(p10,e1->normal)))>0.0) {
      e1->v1=-e1->v1;
      e1->normal=-e1->normal;
    }
    nref = e1->normal;

    float omega=0.5*acos(geom::Dot(geom::Normalize(p10),geom::Normalize(p12)));
    geom::Vec3 orth=geom::Normalize(geom::AxisRotation(e1->normal, -omega)*p12);
    e1->direction=geom::Normalize(geom::Cross(e1->normal,orth));
    
    // align normals to avoid twisting
    //if(geom::Dot(e1->normal,nref)<0.0) e1->normal=-e1->normal;
    //nref=e1->normal;
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
  // finish remaining values
  // i is at size-2 due to break statement above
  nelist[0].normal=nelist[1].normal;
  nelist[i+1].direction=geom::Normalize(p2-p1);
  nelist[i+1].v1=nelist[i+1].normal;
  if (geom::Dot(nelist[i].v1,
                nelist[i+1].v1)<0.0) {
    nelist[i+1].v1=-nelist[i+1].v1;
  }
  nelist[i+1].normal=nelist[i].normal;
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

