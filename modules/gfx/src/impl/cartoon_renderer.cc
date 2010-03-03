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
#include "cartoon_renderer.hh"


#include <ost/gfx/entity.hh>
#include <ost/gfx/impl/tabulated_trig.hh>

namespace ost {

namespace gfx {

static const unsigned int MAX_ARC_DETAIL=12;

using namespace impl;

CartoonRenderer::CartoonRenderer(BackboneTrace& trace, bool force_tube): 
  TraceRendererBase(trace, 3),   force_tube_(force_tube),
  options_(new CartoonRenderOptions())
{
if(force_tube){
  this->SetName("Smooth Tube");
}
else{
  this->SetName("Helix & Strand Cartoon");
}
}

void CartoonRenderer::SetForceTube(bool force_tube)
{
  force_tube_ = force_tube;
}

void CartoonRenderer::PrepareRendering(TraceSubset& subset, 
                                       IndexedVertexArray& va,
                                       SplineEntryListList& spline_list_list,
                                       bool is_sel)
{
  int spline_detail=std::max((unsigned int) 1, options_->GetSplineDetail());
  const Color& sel_clr=this->GetSelectionColor();
  if(options_!=NULL){
    va.Clear();
    va.SetLighting(true);
    va.SetCullFace(true);
    va.SetColorMaterial(true);
    va.SetMode(0x4);
    va.SetPolyMode(options_->GetPolyMode());
    spline_list_list.clear();
    for (int node_list=0; node_list<subset.GetSize(); ++node_list) {
      // first build the spline
      Spline spl;
      const NodeListSubset& nl=subset[node_list];
      for (int i=0; i<nl.GetSize();++i) {      
        int type=0;
        const NodeEntry& entry=nl[i];
        if(!force_tube_) {
          mol::ResidueHandle resh = entry.atom.GetResidue();
          mol::SecStructure sst=resh.GetSecStructure();
          mol::ResidueHandle resh2 = resh.GetNext();
          mol::SecStructure sst2=mol::SecStructure(mol::SecStructure::COIL);
          if (resh2.IsValid()) {
            sst2=resh2.GetSecStructure();
          }
          if(sst.IsHelical()) {
            type=1;
          } else if(sst.IsExtended()) {
            if(!sst2.IsExtended()) {
              type=3; // end of strand
            } else {
              type=2;
            }
          }
        }
        SplineEntry& ee = spl.AddEntry(entry.atom.GetPos(),entry.direction,
                                       entry.normal, entry.rad, 
                                       is_sel ? sel_clr : entry.color1, 
                                       is_sel ? sel_clr : entry.color2, type);
        ee.v1 = entry.v1;
      }
      spline_list_list.push_back(spl.Generate(spline_detail));
      // now create the shape around the interpolated pathway
    }
    RebuildSplineObj(spline_list_list.back(), va, spline_list_list, subset, is_sel);    
    va.SmoothNormals(options_->GetNormalSmoothFactor());
  }  
}
void CartoonRenderer::PrepareRendering()
{
  TraceRendererBase::PrepareRendering();
  va_.Clear();
  this->PrepareRendering(trace_subset_, va_, spline_list_list_, false);
  if (this->HasSelection()) {
    this->PrepareRendering(sel_subset_, sel_va_, sel_spline_list_list_, true);
    sel_va_.SetColorMaterial(false);
    sel_va_.SetLighting(false);
  }
  sel_state_=0;
  state_=0;
}

void CartoonRenderer::Render(){}

bool CartoonRenderer::CanSetOptions(RenderOptionsPtr& render_options)
{
 return render_options.get()->GetRenderMode()==RenderMode::HSC;
}

void CartoonRenderer::SetOptions(RenderOptionsPtr& render_options)
{
  options_ = boost::static_pointer_cast<CartoonRenderOptions>(render_options);
}

RenderOptionsPtr CartoonRenderer::GetOptions()
{
 return options_;
}

void CartoonRenderer::RebuildSplineObj(const SplineEntryList& l, 
                                       IndexedVertexArray& va,
                                       SplineEntryListList& spline_list_list,
                                       const TraceSubset& subset, bool is_sel)
{
  unsigned int detail = std::min(MAX_ARC_DETAIL,
                                 std::max(options_->GetArcDetail(),
                                 (unsigned int)1));
  int spline_detail=std::max((unsigned int) 1, options_->GetSplineDetail());
  std::vector<TraceProfile> profiles;
  float factor=is_sel ? 0.2 : 0.0;
  profiles.push_back(GetCircProfile(detail,
      options_->GetTubeRadius()*options_->GetTubeRatio()+factor,
      options_->GetTubeRadius()+factor, 1.0)); // tube
  if (!force_tube_) {
    profiles.push_back(GetCircProfile(detail,
        options_->GetHelixWidth()+factor,
        options_->GetHelixThickness()+factor,
        options_->GetHelixEcc()+factor)); // helix
    profiles.push_back(GetCircProfile(detail,
        options_->GetStrandWidth()+factor,
        options_->GetStrandThickness()+factor,
        options_->GetStrandEcc()+factor)); // strand
    profiles.push_back(GetCircProfile(detail,
        0.1*options_->GetStrandWidth()+factor,
        options_->GetStrandThickness()+factor,
        options_->GetStrandEcc()+factor)); // arrow end
    profiles.push_back(GetCircProfile(detail,
        1.7*options_->GetStrandWidth()+factor,
        1.1*options_->GetStrandThickness()+factor,
        options_->GetStrandEcc()+factor)); // arrow start    
  }

  // iterate over all spline segments
  for(SplineEntryListList::iterator it=spline_list_list.begin();
      it<spline_list_list.end();++it) {
    /*
      for each spline segment, transform the profile according to the
      normal and direction and assemble it together with the last
      transformed profile into a graphical segment
    */
    SplineEntryList& slist=*it;
    const NodeListSubset& nl=subset[it-spline_list_list.begin()];
    int sit=0, send=slist.size()-spline_detail+1;
    if (nl.AtStart()>0) {
      sit+=nl.AtStart()*spline_detail-spline_detail/2;
    }
    if (nl.AtEnd()>0) {
      send-=nl.AtEnd()*spline_detail-spline_detail/2;
    }
    TraceProfile tprof1=TransformAndAddProfile(profiles,slist[sit],va);
    CapProfile(tprof1,slist[sit],true,va);
    TraceProfile tprof2;
    SplineEntry& last_se=slist[send-1];
    for (int sc=sit+1; sc<send; ++sc) {
      if(slist[sc-1].type==2 && slist[sc].type==3) {
        // arrow on last strand segment
        // type1 is already set to 4 (Spline::Generate),
        // hence just insert single bridging profile
        SplineEntry se(slist[sc]);
        se.type=last_se.type;
        se.type1=last_se.type1;
        se.type2=last_se.type2;
        tprof2=TransformAndAddProfile(profiles,se, va);
        AssembleProfile(tprof1,tprof2,va);
        tprof1=tprof2;
        tprof2=TransformAndAddProfile(profiles,slist[sc], va);
        last_se=slist[sc];
      } else {
        tprof2=TransformAndAddProfile(profiles,slist[sc], va);
      }
      AssembleProfile(tprof1,tprof2,va);
      tprof1=tprof2;
      last_se=slist[sc];
    }
    CapProfile(tprof1,slist[send-1],false,va);
  }
}



TraceProfile CartoonRenderer::TransformAndAddProfile(const std::vector<TraceProfile>& profiles, const SplineEntry& se, IndexedVertexArray& va)
{
  TraceProfile prof1 = profiles[se.type1];
  TraceProfile prof2 = profiles[se.type2];

  bool fuse_flag = se.type!=se.type2;

  // now the transformation
  const geom::Vec3& dir=se.direction;
  const geom::Vec3& norm=se.normal;
  geom::Vec3 orth=geom::Cross(dir,norm);
  // this matrix brings (0,0,1) ->dir (1,0,0) -> norm and (0,1,0) -> orth
  geom::Mat3 rmat(norm[0],orth[0],dir[0],
                  norm[1],orth[1],dir[1],
                  norm[2],orth[2],dir[2]);

  // assemble profile with custom coloring
  TraceProfile tf_prof(prof1.size());
  unsigned int half=prof1.size()/2;
  unsigned int seg=prof1.size()/16;
  for(unsigned int c=0;c<prof1.size();++c) {
    geom::Vec3 vec=rmat*se.rad*prof1[c].v;
    geom::Vec3 norm=rmat*prof1[c].n;
    if(fuse_flag) {
      geom::Vec3 vec2=rmat*se.rad*prof2[c].v;
      geom::Vec3 norm2=rmat*prof2[c].n;
      vec=se.position+(1.0f-se.frac)*vec+se.frac*vec2;
      norm=Normalize((1.0f-se.frac)*norm+se.frac*norm2);
    } else {
      vec+=se.position;
      norm=Normalize(norm);
    }
    tf_prof[c]=TraceProfileEntry(vec,norm);
    Color col = se.color1;
    if(se.type==1) {
      if(se.nflip) {
        col = c<half ? se.color1 : se.color2;
      } else {
        col = c<half ? se.color2 : se.color1;
      }
    } else if(se.type==2 || se.type==3) {
      if(c<=seg || (c>=half-seg && c<=half+seg) || c>=prof1.size()-seg) col=se.color2;
    }
    tf_prof[c].id=va.Add(vec,norm, col);
  }
  return tf_prof;
}

void CartoonRenderer::AssembleProfile(const TraceProfile& prof1,
                                      const TraceProfile& prof2, 
                                      IndexedVertexArray& va)
{
  // determine rotational offset with a heuristic routine
  int best_off=0;
  uint i1=0;
  uint i2=prof1.size()/4;
  uint i3=prof1.size()/2;
  uint i4=std::min(prof1.size()-1,size_t(i2+i3));
  float best_val = Dot(prof1[i1].n,prof2[i1].n)+
                    Dot(prof1[i2].n,prof2[i2].n)+
                    Dot(prof1[i3].n,prof2[i3].n)+
                    Dot(prof1[i4].n,prof2[i4].n);

  for(unsigned int oo=1;oo<prof1.size();++oo) {
    float val = Dot(prof1[i1].n,prof2[oo].n)+
                 Dot(prof1[i2].n,prof2[(oo+i2)%prof1.size()].n)+
                 Dot(prof1[i3].n,prof2[(oo+i3)%prof1.size()].n)+
                 Dot(prof1[i4].n,prof2[(oo+i4)%prof1.size()].n);
    if(val>best_val) {
      best_val=val;
      best_off=oo;
    }
  }

  // assume both profiles have the same size
  for(unsigned int i1=0;i1<prof1.size();++i1) {
    unsigned int i2=(i1+1)%prof1.size();
    unsigned int i3=(i1+best_off)%prof1.size();
    unsigned int i4=(i1+best_off+1)%prof1.size();

    va.AddTri(prof1[i1].id,prof1[i2].id,prof2[i3].id);
    va.AddTri(prof1[i2].id,prof2[i4].id,prof2[i3].id);
    // curvature induces bow-tie artefacts with quads, hence triangles
  }
}

void CartoonRenderer::CapProfile(const impl::TraceProfile& p,
                                 const impl::SplineEntry& se,
                                 bool flipn, IndexedVertexArray& va)
{
  VertexID pi0 = va.Add(se.position,flipn ? -se.direction : se.direction, se.color1);
  for(unsigned int i1=0;i1<p.size();++i1) {
    unsigned int i2=(i1+1)%p.size();
    if(flipn) {
      va.AddTri(pi0,p[i2].id,p[i1].id);
    } else {
      va.AddTri(pi0,p[i1].id,p[i2].id);
    }
  }
}

TraceProfile CartoonRenderer::GetCircProfile(unsigned int detail, float rx, 
                                             float ry, float ex)
{
  TraceProfile prof;

  if(prof.empty()) {
    prof=TraceProfile(detail*4);
    for(unsigned int i=0;i<detail;++i) {
      float ang=0.5*M_PI*static_cast<float>(i)/static_cast<float>(detail);
      float ca=TabCos(ang);
      float sa=TabSin(ang);
      float px=pow(ca,ex);
      float py=pow(sa,ex);
      prof[i]=TraceProfileEntry(geom::Vec3(px,py,0),geom::Vec3());
      prof[detail+i]=TraceProfileEntry(geom::Vec3(-py,px,0),geom::Vec3());
      prof[2*detail+i]=TraceProfileEntry(geom::Vec3(-px,-py,0),geom::Vec3());
      prof[3*detail+i]=TraceProfileEntry(geom::Vec3(py,-px,0),geom::Vec3());
    }
    unsigned int d4=detail*4;
    for(unsigned int i=0;i<d4;++i) {
      prof[i].v=geom::CompMultiply(prof[i].v,geom::Vec3(rx,ry,1.0));
    }
    for(unsigned int i=0;i<d4;++i) {
      unsigned int p1=(d4+i-1)%d4;
      unsigned int p2=(i+1)%d4;
      float dx=prof[p2].v[0]-prof[p1].v[0];
      float dy=prof[p2].v[1]-prof[p1].v[1];
      prof[i].n=geom::Normalize(geom::Vec3(dy,-dx,0.0));
    }
  }
  return prof;
}

CartoonRenderer::~CartoonRenderer() {}

}

}
