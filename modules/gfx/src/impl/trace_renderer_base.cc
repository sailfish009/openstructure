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

#include <ost/mol/entity_property_mapper.hh>
#include <ost/gfx/scene.hh>

#include "mapped_property.hh"

#include "trace_renderer_base.hh"


namespace ost { namespace gfx { namespace impl {

TraceRendererBase::TraceRendererBase(BackboneTrace& trace, int n):
  trace_(trace), trace_subset_(trace, n), sel_subset_(trace, n)
{
}

void TraceRendererBase::PrepareRendering()
{
}

void TraceRendererBase::UpdateViews()
{
  if (state_ & DIRTY_VIEW) {
    mol::EntityView view=this->GetEffectiveView();
    trace_subset_.Update(view);
    state_&=~DIRTY_VIEW;    
  }
  if (this->HasSelection() && (sel_state_ & DIRTY_VIEW)) {
    sel_subset_.Update(sel_);
    sel_state_&=~DIRTY_VIEW;    
  }
}

geom::AlignedCuboid TraceRendererBase::GetBoundingBox() const
{
  geom::Vec3 mmin, mmax;
  assert(!(state_ & DIRTY_VIEW));
  bool empty=true;
  for (int node_list=0; node_list<trace_subset_.GetSize(); ++node_list) {
    // first build the spline
    Spline spl;
    const NodeListSubset& nl=trace_subset_[node_list];
    for (int i=0; i<nl.GetSize();++i) {
      const NodeEntry& entry=nl[i];      
      empty=false;
      geom::Vec3 p=entry.atom.GetPos();
      if (node_list+i==0) {
        mmin=p;
        mmax=p;
      } else {
        mmin=geom::Min(mmin, p);
        mmax=geom::Max(mmax, p);
      }
    }
  }  
  if (empty) {
    throw Error("Can't calculate bounding box of empty renderer");
  }
  return geom::AlignedCuboid(mmin, mmax);
}


void TraceRendererBase::Apply(const gfx::ByElementColorOp& op)
{
  this->UpdateViews();
  mol::Query q(op.GetSelection());
  for (int node_list=0; node_list<trace_subset_.GetSize(); ++node_list) {
    NodeListSubset& nl=trace_subset_[node_list];
    for (int i=0; i<nl.GetSize();++i) {
      if (q.IsAtomSelected(nl[i].atom)) {
        Color clr=GfxObj::Ele2Color(nl[i].atom.GetProp().element);
        this->set_node_entry_color(nl[i], op.GetMask(), clr);
      }
    } 
  }  
  state_|=DIRTY_VA;
}

void TraceRendererBase::Apply(const gfx::ByChainColorOp& op)
{
  this->UpdateViews();
  mol::Query q(op.GetSelection());
  for (int node_list=0; node_list<trace_subset_.GetSize(); ++node_list) {
    NodeListSubset& nl=trace_subset_[node_list];
    for (int i=0; i<nl.GetSize();++i) {
      if (q.IsAtomSelected(nl[i].atom)) {
        Color clr =op.GetColor(nl[i].atom.GetResidue().GetChain().GetName());
        this->set_node_entry_color(nl[i],op.GetMask(),clr);
      }
    }
  }
  state_|=DIRTY_VA;
}


void TraceRendererBase::Apply(const gfx::UniformColorOp& op)
{
  this->UpdateViews();  
  mol::Query q(op.GetSelection());
  for (int node_list=0; node_list<trace_subset_.GetSize(); ++node_list) {
    NodeListSubset& nl=trace_subset_[node_list];
    for (int i=0; i<nl.GetSize();++i) {
      if (q.IsAtomSelected(nl[i].atom)) {
        this->set_node_entry_color(nl[i], op.GetMask(), op.GetColor());
      }
    }
  }
  state_|=DIRTY_VA;
}

void TraceRendererBase::Apply(const gfx::GradientLevelColorOp& op)
{
  this->UpdateViews();
  mol::Query q(op.GetSelection());
  mol::EntityPropertyMapper epm(op.GetProperty(), op.GetLevel());
  gfx::Gradient gradient = op.GetGradient();  
  for (int node_list=0; node_list<trace_subset_.GetSize(); ++node_list) {
    NodeListSubset& nl=trace_subset_[node_list];
    for (int i=0; i<nl.GetSize();++i) {
      try {
        if (q.IsAtomSelected(nl[i].atom)) {
          float n=Normalize(epm.Get(nl[i].atom), op.GetMinV(), op.GetMaxV());
          this->set_node_entry_color(nl[i], op.GetMask(), gradient.GetColorAt(n));
        }
      } catch (std::exception&) {
        LOGN_DEBUG("property " << op.GetProperty() << " not found");
      }
    }
  }
  state_|=DIRTY_VA;
}

void TraceRendererBase::Apply(const gfx::EntityViewColorOp& op)
{
  this->UpdateViews();
  mol::EntityView ev = op.GetEntityView();
  gfx::Gradient g = op.GetGradient();
  const String prop = op.GetProperty();
  float minv = op.GetMinV();
  float maxv = op.GetMaxV();  
  for (int node_list=0; node_list<trace_subset_.GetSize(); ++node_list) {
    NodeListSubset& nl=trace_subset_[node_list];
    for (int i=0; i<nl.GetSize();++i) {
        Color clr=MappedProperty(ev,prop,g,minv,maxv,
                                 nl[i].atom.GetPos());      
        this->set_node_entry_color(nl[i], op.GetMask(), clr);
    }
  }
  state_|=DIRTY_VA;
}

bool TraceRendererBase::HasDataToRender() const
{
  assert(!(state_ & DIRTY_VIEW));
  return this->trace_subset_.GetSize()>0;
}

void TraceRendererBase::set_node_colors(const Color& col, const mol::Query& q, 
                                        ColorMask mask)
{
  this->UpdateViews();
  for (int node_list=0; node_list<trace_subset_.GetSize(); ++node_list) {
    NodeListSubset& nl=trace_subset_[node_list];
    for (int i=0; i<nl.GetSize();++i) {
      if(q.IsAtomSelected(nl[i].atom)) {
        set_node_entry_color(nl[i],mask,col);
      }
    }
  }
  state_|=DIRTY_VA;  
}

void TraceRendererBase::set_node_entry_color(NodeEntry& e, ColorMask mask, 
                                             const Color& c)
{
  if (mask & MAIN_COLOR) e.color1=c;
  if (mask & DETAIL_COLOR) e.color2=c;
}

void TraceRendererBase::PickAtom(const geom::Line3& line, Real line_width,
                                 mol::AtomHandle& picked_atom)
{
  static const float max_dist = 2.0;
  float zmin=1.0;
  if (picked_atom.IsValid()) {
    geom::Vec3 pp=Scene::Instance().Project(picked_atom.GetPos());
    if (pp[2]>0.0 && pp[2]<zmin) {
       zmin=pp[2];
    }    
  }

  mol::AtomHandle atom;
  for (int node_list=0; node_list<trace_subset_.GetSize(); ++node_list) {
    NodeListSubset& nl=trace_subset_[node_list];
    for (int i=0; i<nl.GetSize();++i) {
      geom::Vec3 p=nl[i].atom.GetPos();
      float dist = geom::Distance(line, p);
      if(dist<=max_dist) {
        geom::Vec3 pp = Scene::Instance().Project(p);
        if(pp[2]>0.0 && pp[2]<zmin) {
          zmin=pp[2];
          atom=nl[i].atom.GetHandle();
        }
      }
    }
  }
  if (atom.IsValid()) {
    picked_atom=atom;
  }
}


void TraceRendererBase::PickBond(const geom::Line3& line, Real line_width,
                                 mol::BondHandle& picked_bond)
{
  // don't to anything
}

#if OST_IMG_ENABLED
void TraceRendererBase::Apply(const gfx::MapHandleColorOp& op)
{
  this->UpdateViews();  
  img::MapHandle mh = op.GetMapHandle();
  gfx::Gradient g = op.GetGradient();
  const String& prop = op.GetProperty();
  float minv = op.GetMinV();
  float maxv = op.GetMaxV();  
  for (int node_list=0; node_list<trace_subset_.GetSize(); ++node_list) {
    NodeListSubset& nl=trace_subset_[node_list];
    for (int i=0; i<nl.GetSize();++i) {
      set_node_entry_color(nl[i], 0xff, MappedProperty(mh,prop,g,minv,maxv,
                                                       nl[i].atom.GetPos()));
    }
  }
  state_|=DIRTY_VA;
}
#endif

}}}

