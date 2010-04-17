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

#include <Eigen/Core>
#include <Eigen/Array>
#include <Eigen/SVD>
#include <Eigen/LU>

#include <ost/gfx/entity.hh>
#include <ost/gfx/impl/tabulated_trig.hh>

namespace ost {

namespace gfx {

static const unsigned int MAX_ARC_DETAIL=12;

using namespace impl;

CartoonRenderer::CartoonRenderer(BackboneTrace& trace, bool force_tube): 
  TraceRendererBase(trace, 3),   force_tube_(force_tube),
  options_(new CartoonRenderOptions(force_tube))
{
  this->SetName(force_tube ? "Smooth Tube" : "Helix & Strand Cartoon");
}

void CartoonRenderer::SetForceTube(bool force_tube)
{
  force_tube_ = force_tube;
}

void CartoonRenderer::PrepareRendering(const TraceSubset& subset, 
                                       IndexedVertexArray& va,
                                       SplineEntryListList& spline_list_list,
                                       bool is_sel)
{
  int spline_detail=std::max((unsigned int) 1, options_->GetSplineDetail());
  const Color& sel_clr=this->GetSelectionColor();
  SplineEntryListList tmp_sll;
  if(options_!=NULL){
    va.Clear();
    va.SetLighting(true);
    va.SetCullFace(true);
    va.SetColorMaterial(true);
    va.SetMode(0x4);
    va.SetPolyMode(options_->GetPolyMode());
    for (int node_list=0; node_list<subset.GetSize(); ++node_list) {
      // first build the spline
      SplineEntryList spl;
      const NodeListSubset& nl=subset[node_list];
      for (int i=0; i<nl.GetSize();++i) {      
        int type=0;
        const NodeEntry& entry=nl[i];
        if(!force_tube_) {
          mol::ResidueHandle resh = entry.atom.GetResidue();
          mol::SecStructure sst=resh.GetSecStructure();
          if(sst.IsHelical()) {
            type=1;
          } else if(sst.IsExtended()) {
            type=2;
          }
        }
        SplineEntry ee(entry.atom.GetPos(),entry.direction,
                       entry.normal, entry.rad, 
                       is_sel ? sel_clr : entry.color1, 
                       is_sel ? sel_clr : entry.color2, type);
        ee.v1 = entry.v1;
        spl.push_back(ee);
      }
      if(!spl.empty()) {
        tmp_sll.push_back(spl);
      }
    }
    FudgeSplineObj(va,tmp_sll);
    spline_list_list.clear();
    for(SplineEntryListList::const_iterator sit=tmp_sll.begin();sit!=tmp_sll.end();++sit) {
      spline_list_list.push_back(Spline::Generate(*sit,spline_detail));
    }
    RebuildSplineObj(va, spline_list_list, is_sel);    
    //va.SmoothNormals(options_->GetNormalSmoothFactor());
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

namespace {
  typedef Eigen::Matrix<Real,3,1> EVec3;
  typedef Eigen::Matrix<Real,3,3> EMat3;
  typedef Eigen::Matrix<Real,1,3> ERVec3;
  typedef Eigen::Matrix<Real,Eigen::Dynamic,Eigen::Dynamic> EMatX;
  typedef Eigen::Matrix<Real,1,Eigen::Dynamic> ERVecX;

  ERVec3 to_eigen(const geom::Vec3& v)
  {
    EVec3 nrvo=EVec3::Zero();
    nrvo[0]=v[0]; nrvo[1]=v[1]; nrvo[2]=v[2];
    return nrvo;
  }
  
  std::pair<geom::Vec3,geom::Vec3> fit_helix(const std::vector<geom::Vec3>& points)
  {
    if(points.size()<4) {
      return std::make_pair(points.front(),points.back());
    }
    geom::Vec3 cen(0.0,0.0,0.0);
    for(unsigned int i=0;i<points.size();++i) cen+=points[i];
    cen/=static_cast<float>(points.size());

    EMatX A=EMatX::Zero(points.size(),3);
    for(unsigned int i=0;i<points.size();++i) {
      A.row(i)=to_eigen(points[i]-cen);
    }

    Eigen::SVD<EMatX> svd(A);
    EMatX V=svd.matrixV();
    geom::Vec3 ax(V(0,0),V(1,0),V(2,0));

    geom::Vec3 p1=cen+ax*(-geom::Dot(cen,ax)+geom::Dot(points.front(),ax))/geom::Length2(ax);
    geom::Vec3 p2=cen+ax*(-geom::Dot(cen,ax)+geom::Dot(points.back(),ax))/geom::Length2(ax);

    return std::make_pair(p1,p2);
  }
  
} // ns

void CartoonRenderer::FudgeSplineObj(IndexedVertexArray& va, SplineEntryListList& olistlist)
{
  SplineEntryListList nlistlist;
  SplineEntryList nlist;
  
  for(unsigned int llc=0;llc<olistlist.size();++llc) {
    SplineEntryList olist = olistlist[llc];
    for(unsigned int lc=0;lc<olist.size();++lc) {
      if(olist.at(lc).type==1 && options_->GetHelixMode()==1) {
        if(options_->GetHelixMode()==1) {
          // cylindrical helix
          // collect all CA positions 
          std::vector<geom::Vec3> points;
          SplineEntry tmp_end(olist.at(lc));
          unsigned int lstart=lc;
          for(;lc<olist.size() && olist.at(lc).type==1;++lc) {
            points.push_back(olist.at(lc).position);
          }
          unsigned int lend=lc-1;
          if(lend-lstart<3) {
            // this is a short helix, may as well not bother
            for(unsigned int i=lstart;i<=lend;++i) {
              nlist.push_back(olist[i]);
              nlist.back().type=0;
            }
          } else {
            // fit points to a cylinder
            std::pair<geom::Vec3,geom::Vec3> cyl=fit_helix(points);

            // extend the current spline to start of cyl
            geom::Vec3 cyl_dir = geom::Normalize(cyl.second-cyl.first);
            
            if(lstart>0) {
              SplineEntry tmp_end(olist[lstart]);
              tmp_end.type=0;
              tmp_end.position=cyl.first;
              tmp_end.direction=cyl_dir;
              tmp_end.color1=olist[lstart-1].color1;
              tmp_end.color2=olist[lstart-1].color2;
              tmp_end.normal=geom::Normalize(geom::Cross(cyl_dir,geom::Cross(tmp_end.normal,cyl_dir)));
              if(geom::Dot(tmp_end.normal,olist[lstart].normal)<0.0) {
                tmp_end.normal=-tmp_end.normal;
              }
              nlist.push_back(tmp_end);

              // break nodelist
              nlistlist.push_back(nlist);
              nlist.clear();
            }

            if(lend+1<olist.size()) {
              // and get going with an entry at the end of the cylinder
              SplineEntry tmp_start(olist[lend]);
              tmp_start.type=0;
              tmp_start.position=cyl.second;
              tmp_start.direction=cyl_dir;
              tmp_start.color1=olist[lend+1].color1;
              tmp_start.color2=olist[lend+1].color2;
              tmp_start.normal=geom::Normalize(geom::Cross(cyl_dir,geom::Cross(tmp_start.normal,cyl_dir)));
              if(geom::Dot(tmp_start.normal,olist[lstart].normal)<0.0) {
                tmp_start.normal=-tmp_start.normal;
              }
              nlist.push_back(tmp_start);
            }

            // and finally make the cylinder
            va.AddCylinder(CylinderPrim(cyl.first,cyl.second,
                                        options_->GetHelixWidth(),
                                        olist[lstart].color1,
                                        olist[lend].color1),
                           options_->GetArcDetail(),true);
          }
        } else { // helix mode 0
          // just copy them over
          for(;lc<olist.size() && olist.at(lc).type==1;++lc) {
            nlist.push_back(olist.at(lc));
          }
        }
      } else if(olist.at(lc).type==2) {
        // strand
        unsigned int kstart=nlist.size();
        unsigned int kend=kstart;
        for(;lc<olist.size() && olist.at(lc).type==2;++lc,++kend) {
          nlist.push_back(olist.at(lc));
        }
        kend-=1;
        nlist[kend].type=3; // mark end of strand

        if(options_->GetStrandMode()==1 && kend>kstart) {
          nlist[kend-1].type=3;
          nlist[kend].type=5;
          // smooth the strands
          nlist[kstart].direction = geom::Normalize(nlist[kend].position-nlist[kstart].position);
          nlist[kend].direction=nlist[kstart].direction;
          float invf=1.0/static_cast<float>(kend-kstart);
          for(unsigned int k=kstart;k<=kend;++k) {
            float f = static_cast<float>(k-kstart)*invf;
            nlist[k].position=nlist[kstart].position+f*(nlist[kend].position-nlist[kstart].position);
            nlist[k].direction=nlist[kstart].direction;
            geom::Vec3 tmpn=geom::Normalize(nlist[kstart].normal+f*(nlist[kend].normal-nlist[kstart].normal));
            geom::Vec3 tmpx=geom::Normalize(geom::Cross(nlist[kstart].direction,tmpn));
            nlist[k].normal=geom::Normalize(geom::Cross(tmpx,nlist[kstart].direction));
          }
          // break nodelist, re-start at arrow tip
          if(lc+1<olist.size()) {
            nlistlist.push_back(nlist);
            nlist.clear();
            nlist.push_back(nlistlist.back().back());
            nlist.back().type=0;
            nlist.back().color1=olist[lc+1].color1;
            nlist.back().color2=olist[lc+1].color2;
          }
        }
      }
      if(lc<olist.size()) {
        nlist.push_back(olist.at(lc));
      }
    }
    if(!nlist.empty()) {
      nlistlist.push_back(nlist);
      nlist.clear();
    }
  }
  olistlist.swap(nlistlist);
}

void CartoonRenderer::RebuildSplineObj(IndexedVertexArray& va,
                                       const SplineEntryListList& spline_list_list,
                                       bool is_sel)
{
  unsigned int detail = std::min(MAX_ARC_DETAIL,
                                 std::max(options_->GetArcDetail(),
                                 (unsigned int)1));
  int spline_detail=std::max((unsigned int) 1, options_->GetSplineDetail());
  std::vector<TraceProfile> profiles;
  float factor=is_sel ? 0.2 : 0.0;
  profiles.push_back(GetCircProfile(detail,
				    options_->GetTubeRadius()*options_->GetTubeRatio()+factor,
				    options_->GetTubeRadius()+factor, 
				    options_->GetTubeProfileType(),
				    1.0)); // profile 0 = tube
  if (!force_tube_) {
    profiles.push_back(GetCircProfile(detail,
				      options_->GetHelixWidth()+factor,
				      options_->GetHelixThickness()+factor,
				      options_->GetHelixProfileType(),
				      options_->GetHelixEcc())); // profile 1 = helix
    profiles.push_back(GetCircProfile(detail,
				      options_->GetStrandWidth()+factor,
				      options_->GetStrandThickness()+factor,
				      options_->GetStrandProfileType(),
				      options_->GetStrandEcc())); // profile 2 = strand
    profiles.push_back(profiles.back()); // profile 3==2, strand

    profiles.push_back(GetCircProfile(detail,
				      1.7*options_->GetStrandWidth()+factor,
				      1.1*options_->GetStrandThickness()+factor,
				      options_->GetStrandProfileType(),
				      options_->GetStrandEcc())); // profile 4 = arrow start
    profiles.push_back(GetCircProfile(detail,
				      0.01*options_->GetStrandWidth()+factor,
				      1.1*options_->GetStrandThickness()+factor,
				      options_->GetStrandProfileType(),
				      options_->GetStrandEcc())); // profile 5 = arrow end

  }

  // iterate over all spline segments
  for(SplineEntryListList::const_iterator it=spline_list_list.begin();
      it<spline_list_list.end();++it) {
    /*
      for each spline segment, transform the profile according to the
      normal and direction and assemble it together with the last
      transformed profile into a graphical segment
    */
    SplineEntryList slist=*it;
    if(slist.empty()) continue;
    int sit=0, send=slist.size()-spline_detail+1;
    //int sit=0,send=slist.size();
    TraceProfile tprof1=TransformAndAddProfile(profiles,slist[sit],va);
    CapProfile(tprof1,slist[sit],true,va);
    TraceProfile tprof2;
    SplineEntry last_se=slist[std::max(0,send-1)];
    for (int sc=sit+1; sc<send; ++sc) {
      if(slist[sc].type==3) {
        if(slist[sc-1].type==2) {
          // boundary to arrow
          SplineEntry se(slist[sc]);
          tprof2=TransformAndAddProfile(profiles,se, va);
          AssembleProfile(tprof1,tprof2,va);
          tprof1=tprof2;
          se.type=2;
          se.type1=4;
          se.type2=4;
          tprof2=TransformAndAddProfile(profiles,se, va);
        } else {
          SplineEntry se(slist[sc]);
          se.type1=4;
          if(options_->GetStrandMode()==1) se.type2=5;
          tprof2=TransformAndAddProfile(profiles,se, va);
        }
      } else {
        tprof2=TransformAndAddProfile(profiles,slist.at(sc), va);
      }
      AssembleProfile(tprof1,tprof2,va);
      tprof1=tprof2;
      last_se=slist[sc];
    }
    CapProfile(tprof1,slist.at(send-1),false,va);
  }
}



TraceProfile CartoonRenderer::TransformAndAddProfile(const std::vector<TraceProfile>& profiles, const SplineEntry& se, IndexedVertexArray& va)
{
  assert(se.type1>=0 && se.type1<=5);
  assert(se.type2>=0 && se.type2<=5);
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
#if 0
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
#endif

  // assume both profiles have the same size
  for(unsigned int i1=0;i1<prof1.size();++i1) {
    unsigned int i2=(i1+1)%prof1.size();
    unsigned int i3=(i1+best_off)%prof1.size();
    unsigned int i4=(i1+best_off+1)%prof1.size();

#if 1
    va.AddTri(prof1[i1].id,prof1[i2].id,prof2[i3].id);
    va.AddTri(prof1[i2].id,prof2[i4].id,prof2[i3].id);
#else
    va.AddQuad(prof1[i1].id,prof1[i2].id,prof2[i4].id,prof2[i3].id);
#endif
  }
}

void CartoonRenderer::CapProfile(const impl::TraceProfile& p,
                                 const impl::SplineEntry& se,
                                 bool flipn, IndexedVertexArray& va)
{
  geom::Vec3 norm=flipn ? -se.direction : se.direction;
  VertexID pi0 = va.Add(se.position,norm, se.color1);
  std::vector<VertexID> vertices(p.size());
  for(unsigned int i=0;i<p.size();++i) {
    vertices[i]=va.Add(p[i].v,norm,se.color1);
  }
  for(unsigned int i1=0;i1<p.size();++i1) {
    unsigned int i2=(i1+1)%p.size();
    if(flipn) {
      va.AddTri(pi0,vertices[i2],vertices[i1]);
    } else {
      va.AddTri(pi0,vertices[i1],vertices[i2]);
    }
  }
}

namespace {

  template<unsigned int TYPE>
  void circle_profile(TraceProfile& prof, float ecc);

  // algorithm to get a square x-section
  template<>
  void circle_profile<1>(TraceProfile& prof, float ecc)
  {
    unsigned int detail=prof.size()/4;
    for(unsigned int i=0;i<detail;++i) {
      float ang=0.5*M_PI*static_cast<float>(i)/static_cast<float>(detail);
      float ca=cos(ang);
      float sa=sin(ang);
      float px=std::pow(ca,ecc);
      float py=std::pow(sa,ecc);
      prof[i]=TraceProfileEntry(geom::Vec3(px,py,0),geom::Vec3());
      prof[detail+i]=TraceProfileEntry(geom::Vec3(-py,px,0),geom::Vec3());
      prof[2*detail+i]=TraceProfileEntry(geom::Vec3(-px,-py,0),geom::Vec3());
      prof[3*detail+i]=TraceProfileEntry(geom::Vec3(py,-px,0),geom::Vec3());
    }
  }

  // different algorithm for square x-section
  template<>
  void circle_profile<2>(TraceProfile& prof, float ecc)
  {
    unsigned int detail=prof.size()/4;
    for(unsigned int i=0;i<detail;++i) {
      float ang=0.5*M_PI*static_cast<float>(i)/static_cast<float>(detail);
      float ca=cos(ang);
      float sa=sin(ang);
      float rad=ecc+(1.0f-ecc)/std::max(ca,sa);
      float px=rad*ca;
      float py=rad*sa;
      prof[i]=TraceProfileEntry(geom::Vec3(px,py,0),geom::Vec3());
      prof[detail+i]=TraceProfileEntry(geom::Vec3(-py,px,0),geom::Vec3());
      prof[2*detail+i]=TraceProfileEntry(geom::Vec3(-px,-py,0),geom::Vec3());
      prof[3*detail+i]=TraceProfileEntry(geom::Vec3(py,-px,0),geom::Vec3());
    }
  }

  template<>
  void circle_profile<3>(TraceProfile& prof, float ecc)
  {
    unsigned int detail=prof.size()/4;
    for(unsigned int i=0;i<detail;++i) {
      float ang=0.5*M_PI*static_cast<float>(i)/static_cast<float>(detail);
      float ca=cos(ang);
      float sa=sin(ang);
      float sa2=sin(2.0*ang);
      float rad=1.0+std::abs(1.0-ecc)*0.5*sa2*sa2*sa2;
      float px=rad*ca;
      float py=rad*sa;
      prof[i]=TraceProfileEntry(geom::Vec3(px,py,0),geom::Vec3());
      prof[detail+i]=TraceProfileEntry(geom::Vec3(-py,px,0),geom::Vec3());
      prof[2*detail+i]=TraceProfileEntry(geom::Vec3(-px,-py,0),geom::Vec3());
      prof[3*detail+i]=TraceProfileEntry(geom::Vec3(py,-px,0),geom::Vec3());
    }
  }

  // default - plain circular
  template<unsigned int TYPE>
  void circle_profile(TraceProfile& prof, float ecc)
  {
    unsigned int detail=prof.size()/4;
    for(unsigned int i=0;i<detail;++i) {
      float ang=0.5*M_PI*static_cast<float>(i)/static_cast<float>(detail);
      float px=cos(ang);
      float py=sin(ang);
      prof[i]=TraceProfileEntry(geom::Vec3(px,py,0),geom::Vec3());
      prof[detail+i]=TraceProfileEntry(geom::Vec3(-py,px,0),geom::Vec3());
      prof[2*detail+i]=TraceProfileEntry(geom::Vec3(-px,-py,0),geom::Vec3());
      prof[3*detail+i]=TraceProfileEntry(geom::Vec3(py,-px,0),geom::Vec3());
    }
  }
}

  TraceProfile CartoonRenderer::GetCircProfile(unsigned int detail, float rx, float ry, unsigned int type, float ecc)
{
  unsigned int d4=detail*4;
  TraceProfile prof(d4);

  if(type==1) {
    circle_profile<1>(prof,ecc);
  } else if(type==2) {
    circle_profile<2>(prof,ecc);
  } else if(type==3) {
    circle_profile<3>(prof,ecc);
  } else if(type==4) {
    circle_profile<4>(prof,ecc);
  } else {
    circle_profile<0>(prof,ecc);
  }

  // adjust axis rations
  for(unsigned int i=0;i<d4;++i) {
    prof[i].v[0]*=rx;
    prof[i].v[1]*=ry;
  }
  
  // calculate normals
  for(unsigned int i=0;i<d4;++i) {
    unsigned int p1=(d4+i-1)%d4;
    unsigned int p2=(i+1)%d4;
    float dx=prof[p2].v[0]-prof[p1].v[0];
    float dy=prof[p2].v[1]-prof[p1].v[1];
    prof[i].n=geom::Normalize(geom::Vec3(dy,-dx,0.0));
  }
  return prof;
}

CartoonRenderer::~CartoonRenderer() {}

}

}
