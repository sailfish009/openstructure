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
#include "sline_renderer.hh"

#include <ost/gfx/entity.hh>

namespace ost { namespace gfx { namespace impl {

SlineRenderer::SlineRenderer(BackboneTrace& trace): 
  TraceRendererBase(trace, 3), options_(new SlineRenderOptions()) 
{}

void SlineRenderer::PrepareRendering()
{
  TraceRendererBase::PrepareRendering();
  va_.Clear();
  this->PrepareRendering(trace_subset_, va_, false);
  sel_va_.Clear();
  if (this->HasSelection()) {
    this->PrepareRendering(sel_subset_, sel_va_, true);
    sel_va_.SetLineWidth(options_->GetLineWidth()+3.0);    
  }
}

void SlineRenderer::PrepareRendering(TraceSubset& trace_subset, 
                                     IndexedVertexArray& va, bool is_sel)
{
  const Color& sel_clr=this->GetSelectionColor();
  
  int spline_detail=std::max((unsigned int) 1, options_->GetSplineDetail());
  if(options_!=NULL){
    va.Clear();
    va.SetLighting(false);
    va.SetCullFace(false);
    va.SetColorMaterial(false);
    va.SetMode(0x2);
    va.SetTwoSided(true);
    va.SetLineWidth(options_->GetLineWidth());
    va.SetPointSize(options_->GetLineWidth());
    va.SetAALines(options_->GetAALines());
    for (int node_list=0; node_list<trace_subset.GetSize(); ++node_list) {
      // first build the spline
      Spline spl;
      const NodeListSubset& nl=trace_subset[node_list];
      assert(nl.GetSize() && "node list subset with zero eles encountered!");
      for (int i=0; i<nl.GetSize();++i) {
        const NodeEntry& entry=nl[i];
        SplineEntry& ee = spl.AddEntry(entry.atom.GetPos(), entry.direction,
                                       entry.normal, entry.rad, 
                                       is_sel ? sel_clr : entry.color1, 
                                       is_sel ? sel_clr : entry.color2, 0);
        ee.v1 = entry.v1;
      }
      SplineEntryList sel = spl.Generate(spline_detail);      
      SplineEntryList::const_iterator sit=sel.begin(), 
                                     send=sel.end()-spline_detail+1;
      if (nl.AtStart()>0) {
        sit+=nl.AtStart()*spline_detail-spline_detail/2;
      }
      if (nl.AtEnd()>0) {
        send-=nl.AtEnd()*spline_detail-spline_detail/2;
      }
      VertexID p0=va.Add(sit->position, geom::Vec3(),sit->color1);
      for (++sit; sit<send; ++sit) {
        VertexID p1 = va.Add(sit->position, geom::Vec3(),sit->color1);
        va.AddLine(p0,p1);
        p0=p1;
      }
    }
  }
  sel_state_=0;
  state_=0;
}

bool SlineRenderer::CanSetOptions(RenderOptionsPtr& render_options)
{
 return render_options->GetRenderMode()==RenderMode::SLINE;
}

void SlineRenderer::SetOptions(RenderOptionsPtr& render_options)
{
  options_ = boost::static_pointer_cast<SlineRenderOptions>(render_options);
}

RenderOptionsPtr SlineRenderer::GetOptions()
{
 return options_;
}

SlineRenderer::~SlineRenderer() 
{

}

}}}
