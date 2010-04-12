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

#include <ost/gfx/entity.hh>
#include <ost/gfx/povray.hh>

#include "trace_renderer.hh"


namespace ost { namespace gfx { namespace impl {

TraceRenderer::TraceRenderer(BackboneTrace& trace): 
  TraceRendererBase(trace, 1), options_(new TraceRenderOptions())
{
  this->SetName("Trace");
}

void TraceRenderer::PrepareRendering() 
{
  TraceRendererBase::PrepareRendering();
  va_.Clear();
  this->PrepareRendering(trace_subset_, va_, false);
  sel_va_.Clear();
  if (this->HasSelection()) {
    this->PrepareRendering(sel_subset_, sel_va_, true);
    sel_va_.SetLighting(false);    
  }
}

#if 1
void TraceRenderer::PrepareRendering(TraceSubset& trace_subset,
                                     IndexedVertexArray& va, bool is_sel)
{
  float plus=is_sel ? 0.05: 0.0;
  const Color& sel_clr=this->GetSelectionColor();
  if(options_!=NULL){
    va.Clear();
    va.SetLighting(true);
    va.SetCullFace(true);
    va.SetColorMaterial(true);
    va.SetTwoSided(false);
    for (int node_list=0; node_list<trace_subset.GetSize(); ++node_list) {
      const NodeListSubset& nl=trace_subset[node_list];
      mol::AtomHandle a1=nl[0].atom;
      va.AddSphere(SpherePrim(a1.GetPos(),
                              options_->GetTubeRadius()+plus,
                              is_sel ? sel_clr : nl[0].color1),
                   options_->GetArcDetail());
      for(int i=1;i<nl.GetSize();++i) {
        mol::AtomHandle a2=nl[i].atom;
        va.AddSphere(SpherePrim(a2.GetPos(),
                                options_->GetTubeRadius()+plus,
                                is_sel ? sel_clr : nl[i].color1),
                     options_->GetArcDetail());
        const geom::Vec3& p0=a1.GetPos();
        const geom::Vec3& p2=a2.GetPos();
        geom::Vec3 p1=(p0+p2)*0.5;
        va.AddCylinder(CylinderPrim(p0,p1,options_->GetTubeRadius()+plus,nl[i-1].color1),
                       options_->GetArcDetail());
        va.AddCylinder(CylinderPrim(p1,p2,options_->GetTubeRadius()+plus,nl[i].color1),
                       options_->GetArcDetail());
        a1=a2;
      }
    }
  }
  sel_state_=0;
  state_=0;
}

#else

void TraceRenderer::PrepareRendering(TraceSubset& trace_subset,
                                     IndexedVertexArray& va, bool is_sel)
{
  const Color& sel_clr=this->GetSelectionColor();
  float radius=options_->GetTubeRadius();
  if (is_sel) {
    radius+=0.1;
  }
  int n=8*options_->GetArcDetail();
  if(options_!=NULL) {
    va.SetLighting(true);
    va.SetCullFace(false);
    for (int node_list=0; node_list<trace_subset.GetSize(); ++node_list) {
      const NodeListSubset& nl=trace_subset[node_list];
      geom::Vec3 cursor_pos;
      geom::Mat3 cursor_ori;
      if (nl.GetSize()==2) {
        VertexID p0, p1;
        if (nl.AtStart()==0) {
          p0=va.Add(nl[0].atom.GetPos(), geom::Vec3(),
                    is_sel ? sel_clr : nl[0].color1);
          p1=va.Add((nl[0].atom.GetPos()+nl[1].atom.GetPos())/2, 
                    geom::Vec3(), is_sel ? sel_clr : nl[1].color1);
        } else {
          p0=va.Add((nl[0].atom.GetPos()+nl[1].atom.GetPos())/2, 
                    geom::Vec3(), is_sel ? sel_clr : nl[0].color1);
          p1=va.Add(nl[1].atom.GetPos(), 
                    geom::Vec3(), is_sel ? sel_clr : nl[1].color1);
        }
        va.AddLine(p0, p1);        
        continue;
      }
      if (nl.GetSize()<3) {
        continue;
      }
      VertexID p0;
      geom::Vec3 z=geom::Normalize(nl[1].atom.GetPos()-nl[0].atom.GetPos());
      geom::Vec3 y=geom::OrthogonalVector(z);
      geom::Vec3 x=geom::Cross(y, z);
      cursor_ori=geom::Mat3(x[0], y[0], z[0], x[1], y[1], z[1], 
                            x[2], y[2], z[2]);    
      if (nl.AtStart()==0) {
        cursor_pos=nl[0].atom.GetPos();
      } else {
        cursor_pos=(nl[0].atom.GetPos()+nl[1].atom.GetPos())/2;
      }
      p0=this->AddCappedProfile(va, is_sel? sel_clr : nl[0].color1, 
                                cursor_pos, cursor_ori, radius, true, n);
      for (int i=1; i<nl.GetSize()-1;++i) {
        const NodeEntry& entry=nl[i];
        geom::Vec3 old_dir=geom::Normalize(entry.atom.GetPos()-cursor_pos);
        cursor_pos=entry.atom.GetPos();

        geom::Vec3 z=geom::Normalize(nl[i+1].atom.GetPos()-cursor_pos);
        geom::Vec3 y=geom::Normalize(geom::Cross(z, cursor_ori.GetCol(0)));
        geom::Vec3 x=geom::Normalize(geom::Cross(y, z));
        geom::Vec3 iz=geom::Normalize(cursor_ori.GetCol(2)+z);

        geom::Vec3 iy=geom::Normalize(geom::Cross(cursor_ori.GetCol(2), z));
        geom::Vec3 ix=geom::Normalize(geom::Cross(iy, iz));

        geom::Mat3 i_ori=geom::Mat3(ix[0], iy[0], iz[0], ix[1], iy[1], iz[1], 
                                    ix[2], iy[2], iz[2]);                           
        cursor_ori=geom::Mat3(x[0], y[0], z[0], x[1], y[1], z[1], 
                              x[2], y[2], z[2]);
        // TODO. The intersection of two cylinders is an ellipse. Use an 
        //        elliptic profile instead of a circular profile.
        VertexID p1=this->AddCircularProfile(va, is_sel ? sel_clr : entry.color1,
                                             cursor_pos, i_ori, radius, n);
        this->ConnectProfiles(p0, p1, n, old_dir, va);
        p0=p1;
      }
      const NodeEntry& entry=nl[nl.GetSize()-1];      
      if (nl.AtEnd()==0) {
        cursor_pos=entry.atom.GetPos();
      } else {
        cursor_pos=(entry.atom.GetPos()+nl[nl.GetSize()-2].atom.GetPos())*0.5;
      }
      VertexID p1 =this->AddCappedProfile(va, is_sel ? sel_clr : entry.color1, 
                                          cursor_pos, cursor_ori, 
                                          radius, false, n);
      this->ConnectProfiles(p0, p1, n, cursor_ori.GetCol(2), va);      
    }
  }
  sel_state_=0;
  state_=0;
}
#endif

VertexID TraceRenderer::AddCappedProfile(IndexedVertexArray& va, 
                                         const Color& color,
                                         const geom::Vec3& center, 
                                         const geom::Mat3& ori, float radius, 
                                         bool flip_normal,
                                         int n)
{
   VertexID center_id=va.Add(center, geom::Vec3(), color);
   VertexID first=this->AddCircularProfile(va, color, center, ori, radius, n);
   for (int i=0; i<n; ++i) {
     va.AddTriN(center_id, first+i, first+((i+1) % n));
   }
   return first;
}

VertexID TraceRenderer::AddCircularProfile(IndexedVertexArray& va, 
                                           const Color& color,
                                           const geom::Vec3& center, 
                                           const geom::Mat3& ori, 
                                           float radius, 
                                           int n)
{
  float delta_angle=2*M_PI/n;
  VertexID f=0;
  geom::Vec3 normal=ori.GetRow(2);
  for (int i=0; i<n; ++i) {
    geom::Vec3 normal=ori*geom::Vec3(cos(i*delta_angle), 
                                     sin(i*delta_angle), 0.0);
    VertexID x=va.Add(center+normal*radius, normal, color);
    if (i==0) {
      f=x;
    }
  }
  return f;
}

void TraceRenderer::ConnectProfiles(VertexID prof0, VertexID prof1, int n, 
                                    const geom::Vec3& dir, 
                                    IndexedVertexArray& va)
{
  // avoid twisting
  int off=0;
  float best=0.0;
  geom::Vec3 pp=va.GetVert(prof0);
  for (int i=0; i<n; ++i) {
    geom::Vec3 dir2=geom::Normalize(va.GetVert(prof1+i)-pp);
    float dot=fabs(geom::Dot(dir, dir2));
    if (best<dot) {
      best=dot;
      off=i;
    }
  }
  for (int i=0; i<n; ++i) {
    VertexID i1=prof1+((i+off) % n), i2=prof1+((i+off+1) % n);
    VertexID  i3=prof0+i, i4=prof0+((i+1)%n);
    va.AddTriN(i1, i2, i3);
    va.AddTriN(i2, i3, i4);
  }
}

void TraceRenderer::RenderPov(PovState& pov, const std::string& name)
{
  pov.write_merge_or_union(name);

  for (int node_list=0; node_list<trace_subset_.GetSize(); ++node_list) {
    const NodeListSubset& nl=trace_subset_[node_list];

    geom::Vec3 p0=nl[0].atom.GetPos();
    float rad0=0.2;
    Color col0=nl[0].color1;
    pov.write_sphere(p0,rad0,col0,name);
    for (int i=1; i<nl.GetSize();++i) {
      const NodeEntry& entry=nl[i];
      geom::Vec3 p1=nl[i].atom.GetPos();
      float rad1=0.2;
      Color col1=nl[i].color1;
      pov.write_sphere(p1,rad1,col1,name);
      geom::Vec3 pm=(p0+p1)*0.5;
      pov.write_cyl(p0,pm,rad0,col0,name,true);
      pov.write_cyl(pm,p1,rad1,col1,name,true);
      p0=p1;
      rad0=rad1;
      col0=col1;
    }
  }

  pov.inc() << " }\n";
}

void TraceRenderer::Render()
{

}

bool TraceRenderer::CanSetOptions(RenderOptionsPtr& render_options)
{
  return render_options.get()->GetRenderMode()==RenderMode::TRACE;
}

void TraceRenderer::SetOptions(RenderOptionsPtr& render_options)
{
  options_=boost::static_pointer_cast<TraceRenderOptions>(render_options);
}

RenderOptionsPtr TraceRenderer::GetOptions()
{
 return options_;
}

TraceRenderer::~TraceRenderer() 
{

}

}}}
