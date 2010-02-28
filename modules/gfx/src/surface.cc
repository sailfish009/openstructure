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
  Author: Ansgar Philippsen
*/

#include <map>
#include <limits>

#include <ost/mol/atom_prop.hh>
#include <ost/mol/mol.hh>
#include <ost/mol/entity_property_mapper.hh>
#include "surface.hh"
#include "glext_include.hh"
#include "impl/mapped_property.hh"

namespace ost { 

using namespace geom;

namespace gfx {

Surface::Surface(const String& name, const mol::SurfaceHandle& sh):
  GfxObj(name),
  sh_(sh),
  vmap_(),
  recalc_bb_(true)
{
  // TODO replace with def mat for this gfx obj type
  SetMat(0.0,1.0,0.2,48.0);
  Rebuild();
}

void Surface::CustomRenderGL(RenderPass pass)
{
  if(pass>0) return;
  va_.RenderGL();
}

void Surface::CustomRenderPov(PovState& pov)
{
  va_.RenderPov(pov, GetName());
}

void Surface::OnRenderModeChange()
{
  Rebuild();
}

void Surface::Replace(const mol::SurfaceHandle& sh)
{
  sh_=sh;
  recalc_bb_=true;
  Rebuild();
}

void Surface::Rebuild()
{
  if(GetRenderMode()==RenderMode::DEBUG2) {
    vmap_.clear();
    va_.Clear();
    va_.SetMode(0x7);
    va_.SetPolyMode(1);
    va_.SetLighting(false);
    va_.SetCullFace(false);
    va_.SetPointSize(3.0);
    
    std::vector<mol::SurfaceVertexID> svid_list = sh_.GetVertexIDList();
    std::vector<mol::SurfaceTriID> stid_list = sh_.GetTriIDList();
    
    for(std::vector<mol::SurfaceVertexID>::const_iterator it=svid_list.begin();
        it!=svid_list.end();++it) {
      mol::SurfaceVertex vert=sh_.GetVertex(*it);
      Color col(1.0,1.0,1.0,1.0);
      if(vert.type==1) {
        col=Color(1.0,0.0,0.0);
      } else if(vert.type==2) {
        col=Color(0.0,1.0,0.0);
      } else if(vert.type==3) {
        col=Color(0.0,0.0,1.0);
      }
      vmap_[*it] = va_.Add(vert.position,vert.normal,col);
    }
    
    for(std::vector<mol::SurfaceTriID>::const_iterator it=stid_list.begin();
        it!=stid_list.end();++it) {
      mol::SurfaceTri tri=sh_.GetTri(*it);
      va_.AddTri(vmap_[tri.v0],vmap_[tri.v1],vmap_[tri.v2]);
    }
  } else {
    va_.Clear();
    va_.SetMode(0x4);
    va_.SetLighting(true);
    va_.SetTwoSided(true);
    va_.SetCullFace(false);

    if(GetRenderMode()==RenderMode::DEBUG) {
      va_.DrawNormals(true);
    } else {
      va_.DrawNormals(false);
    }
    
    std::vector<mol::SurfaceVertexID> svid_list = sh_.GetVertexIDList();
    std::vector<mol::SurfaceTriID> stid_list = sh_.GetTriIDList();
    
    for(std::vector<mol::SurfaceVertexID>::const_iterator it=svid_list.begin();
        it!=svid_list.end();++it) {
      mol::SurfaceVertex vert=sh_.GetVertex(*it);
      Color col(1.0,1.0,1.0,1.0);
      vmap_[*it] = va_.Add(vert.position,vert.normal,col);
    }
    
    for(std::vector<mol::SurfaceTriID>::const_iterator it=stid_list.begin();
        it!=stid_list.end();++it) {
      mol::SurfaceTri tri=sh_.GetTri(*it);
      va_.AddTri(vmap_[tri.v0],vmap_[tri.v1],vmap_[tri.v2]);
    }
  }
  this->ReapplyColorOps();
  recalc_bb_=true;
}

geom::AlignedCuboid Surface::GetBoundingBox() const
{
  static Vec3 minc,maxc;
  if(recalc_bb_) {
    minc=Vec3(std::numeric_limits<float>::max(),
                    std::numeric_limits<float>::max(),
                    std::numeric_limits<float>::max());
    maxc=Vec3(-std::numeric_limits<float>::max(),
                    -std::numeric_limits<float>::max(),
                    -std::numeric_limits<float>::max());
    
    std::vector<mol::SurfaceVertexID> svid_list = sh_.GetVertexIDList();
    for(std::vector<mol::SurfaceVertexID>::const_iterator it=svid_list.begin();
        it!=svid_list.end();++it) {
      Vec3 pos = sh_.GetVertex(*it).position;
      minc=Min(minc,pos);
      maxc=Max(maxc,pos);
    }
    minc-=Vec3(1.0,1.0,1.0);
    maxc+=Vec3(1.0,1.0,1.0);
    recalc_bb_=false;
  }
  return geom::AlignedCuboid(minc,maxc);
}

void Surface::CustomPreRenderGL(bool flag)
{
  if(flag) {
    //Rebuild();
  } else {
    RefreshVA(va_);
  }
}

void Surface::SetColor(const Color& col, const String& selection)
{
  UniformColorOp cop = UniformColorOp(selection, col);
  this->Apply(cop);
}

void Surface::ColorBy(const mol::EntityView& ev, 
                      const String& prop,
                      const Gradient& g, float minv, float maxv)
{
  EntityViewColorOp evop = EntityViewColorOp(prop, g,minv,maxv,ev);
  this->Apply(evop);
}
#if OST_IMG_ENABLED
void Surface::ColorBy(const img::MapHandle& mh,
                      const String& prop,
                      const Gradient& g,float minv, float maxv)
{
  MapHandleColorOp mhop = MapHandleColorOp("",prop,g,minv,maxv,mh);
  this->Apply(mhop);
}

#endif //OST_IMG_ENABLED


namespace {

float normalize(float v, float min_v, float max_v)
{
  return (v-min_v)/(max_v-min_v);
}
}

void Surface::ColorBy(const String& prop, 
                     const Gradient& gradient,
                     float minv,float maxv,
                     mol::Prop::Level level)
{
  GradientLevelColorOp glop = GradientLevelColorOp("",prop, gradient,minv,maxv,level);
  this->Apply(glop);
}

void Surface::ColorBy(const String& prop, 
                     const Color& c1, const Color& c2, 
                     float minv, float maxv,
                     mol::Prop::Level level)
{
  Gradient g;
  g.SetColorAt(0.0,c1);
  g.SetColorAt(1.0,c2);
  this->ColorBy(prop,g,minv,maxv,level);
}

void Surface::ColorBy(const String& prop, 
                     const Gradient& gradient,
                     mol::Prop::Level level)
{
  BasicGradientColorOp bgop = BasicGradientColorOp(prop, gradient,level);
  this->Apply(bgop);
}

void Surface::ColorBy(const String& prop, 
                     const Color& c1, const Color& c2,
                     mol::Prop::Level level)
{
  Gradient g;
  g.SetColorAt(0.0,c1);
  g.SetColorAt(1.0,c2);
  this->ColorBy(prop,g,level);
}

void Surface::Apply(const gfx::UniformColorOp& op, bool store){
  if(store){
	UniformColorOp* op_ptr = new UniformColorOp(op);
    this->AppendColorOp(op_ptr);
  }
  mol::Query q(op.GetSelection());
  gfx::Color col = op.GetColor();
  if(op.GetSelection()=="") {
	for(VMap::const_iterator it=vmap_.begin();it!=vmap_.end();++it) {
	  va_.SetColor(it->second,col);
	}
  } else {
	for(VMap::const_iterator it=vmap_.begin();it!=vmap_.end();++it) {
	  mol::AtomHandle ah = sh_.GetVertex(it->first).atom;
	  if(ah.IsValid()) {
		if(q.IsAtomSelected(ah)) {
		  va_.SetColor(it->second,col);
		}
	  }
	}
  }
  FlagRefresh();
}
void Surface::Apply(const gfx::BasicGradientColorOp& op, bool store){
  if(store){
    BasicGradientColorOp* op_ptr = new BasicGradientColorOp(op);
    this->AppendColorOp(op_ptr);
  }
  const String& prop = op.GetSelection();
  const Gradient& gradient = op.GetGradient();
  mol::Prop::Level level = op.GetLevel();
  // get the min/max values first, requires one lookup round
  float minv=std::numeric_limits<float>::max();
  float maxv=-std::numeric_limits<float>::max();
  mol::EntityPropertyMapper epm(prop, level);
  std::vector<std::pair<VertexID,float> > v2v;
  for(VMap::const_iterator it=vmap_.begin();it!=vmap_.end();++it) {
	mol::AtomHandle ah = sh_.GetVertex(it->first).atom;
	if(ah.IsValid()) {
	  float v = epm.Get(ah);
	  v2v.push_back(std::make_pair(it->second,v));
	  minv=std::min(minv,v);
	  maxv=std::max(maxv,v);
	}
  }

  // reuse values for speed optimization
  for(std::vector<std::pair<VertexID,float> >::const_iterator it=v2v.begin();it!=v2v.end();++it) {
	va_.SetColor(it->first,gradient.GetColorAt(normalize(it->second,minv,maxv)));
  }
  FlagRefresh();
}

void Surface::Apply(const gfx::GradientLevelColorOp& op, bool store){
  if(store){
    GradientLevelColorOp* op_ptr = new GradientLevelColorOp(op);
    this->AppendColorOp(op_ptr);
  }
  mol::Prop::Level level = op.GetLevel();
  const String& prop = op.GetProperty();
  const Gradient& gradient = op.GetGradient();
  float minv = op.GetMinV();
  float maxv = op.GetMaxV();
  // don't bother mapping any surface specific properties, just go
  // for the attached atoms
  mol::EntityPropertyMapper epm(prop, level);
  for(VMap::const_iterator it=vmap_.begin();it!=vmap_.end();++it) {
	mol::AtomHandle ah = sh_.GetVertex(it->first).atom;
	if(ah.IsValid()) {
	  va_.SetColor(it->second,gradient.GetColorAt(normalize(epm.Get(ah),minv,maxv)));
	}
  }
  FlagRefresh();
}

void Surface::Apply(const gfx::EntityViewColorOp& op, bool store){
  if(store){
    EntityViewColorOp* op_ptr = new EntityViewColorOp(op);
    this->AppendColorOp(op_ptr);
  }
  const mol::EntityView& ev = op.GetEntityView();
  const String& prop = op.GetProperty();
  const Gradient& g = op.GetGradient();
  float minv = op.GetMinV();
  float maxv = op.GetMaxV();
  for(VMap::const_iterator it=vmap_.begin();it!=vmap_.end();++it) {
  va_.SetColor(it->second,impl::MappedProperty(ev,prop,g,minv,maxv,
                                               sh_.GetVertex(it->first).position));
  }
  FlagRefresh();
}

#if OST_IMG_ENABLED
void Surface::Apply(const gfx::MapHandleColorOp& op, bool store){
  if(store){
    MapHandleColorOp* op_ptr = new MapHandleColorOp(op);
    this->AppendColorOp(op_ptr);
  }
  const img::MapHandle& mh = op.GetMapHandle();
  const String& prop = op.GetProperty();
  const Gradient& g = op.GetGradient();
  float minv = op.GetMinV();
  float maxv = op.GetMaxV();
  for(VMap::const_iterator it=vmap_.begin();it!=vmap_.end();++it) {
	va_.SetColor(it->second,impl::MappedProperty(mh,prop,g,minv,maxv,sh_.GetVertex(it->first).position));
  }
  FlagRefresh();
}
#endif //OST_IMG_ENABLED

void Surface::CleanColorOps(){
  GfxObj::CleanColorOps();
}

void Surface::ReapplyColorOps(){
  GfxObj::ReapplyColorOps();
}

// rsurf stuff
using namespace mol::rsurf;

RSurface::RSurface(const String& name, const RSurfP& rs):
  GfxObj(name),
  rs_(rs)
{
  ArcList alist = rs->GetArcList();
  TetList tlist = rs->GetTetList();
  SphereList slist = rs->GetSphereList();


#if 0
  for(TetList::const_iterator it=tlist.begin();it!=tlist.end();++it) {
    VertexID id1 = va_.Add((*it)->A->pos,Vec3(),Color(1,1,1));
    VertexID id2 = va_.Add((*it)->B->pos,Vec3(),Color(1,1,1));
    VertexID id3 = va_.Add((*it)->C->pos,Vec3(),Color(1,1,1));
    VertexID id4 = va_.Add((*it)->cA,Vec3(),Color(0,1,0));
    VertexID id5 = va_.Add((*it)->cB,Vec3(),Color(0,1,0));
    VertexID id6 = va_.Add((*it)->cC,Vec3(),Color(0,1,0));
    va_.AddLine(id1,id4);
    va_.AddLine(id2,id5);
    va_.AddLine(id3,id6);
    va_.AddLine(id4,id5);
    va_.AddLine(id5,id6);
    va_.AddLine(id6,id4);
  }
#endif

#if 0
  for(ArcList::const_iterator it=alist.begin();it!=alist.end();++it) {
    Arc& arc= *(*it);
    if(arc.S && arc.T) {
      VertexID id0 = va_.Add(arc.S->pos,Vec3(),Color(1,0,0));
      VertexID id1 = va_.Add(arc.mid,Vec3(),Color(1,1,1));
      VertexID id2 = va_.Add(arc.T->pos,Vec3(),Color(1,0,1));
      VertexID id3 = va_.Add(arc.H->pos,Vec3(),Color(1,1,1));
      VertexID id4 = va_.Add(arc.K->pos,Vec3(),Color(1,1,1));
      va_.AddLine(id0,id1);
      va_.AddLine(id1,id2);
      va_.AddLine(id1,id3);
      va_.AddLine(id1,id4);
    }
  }
#endif


  for(SphereList::const_iterator it=slist.begin();it!=slist.end();++it) {
    for(uint l=0;l<(*it)->arc_list_list.size();++l) {
      const Sphere& sp = *(*it);
      const ArcDirDeque& alist=sp.arc_list_list[l];
      if(alist.empty()) continue; 

      if(alist.size()<4) continue;
      Vec3 a0 = alist[0].inv_arc ? -alist[0].arc->axis : alist[0].arc->axis;
      Vec3 a1 = alist[1].inv_arc ? -alist[1].arc->axis : alist[1].arc->axis;
      Vec3 s0 = alist[0].inv_arc ? alist[0].arc->T->pos : alist[0].arc->S->pos;
      Vec3 s1 = alist[1].inv_arc ? alist[1].arc->T->pos : alist[1].arc->S->pos;
      Vec3 p0 = sp.pos+sp.rad*(s0-sp.pos)/((sp.rad+alist[0].arc->S->rad));
      Vec3 p1 = sp.pos+sp.rad*(s1-sp.pos)/((sp.rad+alist[1].arc->S->rad));

      VertexID id00 = va_.Add(p0,Vec3(),Color(1,1,1));
      VertexID id10 = va_.Add(p1,Vec3(),Color(1,1,1));
      VertexID id20 = id00;

      for(float c=0.0;c<=1.0;c+=0.1) {
        Mat3 r0 = AxisRotation(a0,c*alist[0].arc->phi);
        Mat3 r1 = AxisRotation(a1,c*alist[1].arc->phi);
        Vec3 v0 = r0*(p0-alist[0].arc->fixpoint)+alist[0].arc->fixpoint;
        Vec3 v1 = r1*(p1-alist[1].arc->fixpoint)+alist[1].arc->fixpoint;
        Vec3 v2 = r1*(v0-alist[1].arc->fixpoint)+alist[1].arc->fixpoint;
        VertexID id01 = va_.Add(v0,Vec3(),Color(1,1,1));
        VertexID id11 = va_.Add(v1,Vec3(),Color(1,1,1));
        VertexID id21 = va_.Add(v2,Vec3(),Color(1,c,0));
        va_.AddLine(id00,id01);
        va_.AddLine(id10,id11);
        va_.AddLine(id20,id21);
        id00=id01;
        id10=id11;
        id20=id21;
      }

#if 0
      Vec3 sum1;
      Vec3 sum2;
      Vec3 sum3;
      Vec3 prev;
      float plan=0.0;
      for(uint i=0;i<alist.size();++i) {
        uint j = (i-1+alist.size())%alist.size();
        uint k = (i+1)%alist.size();
        sum1+=(alist[i].arc->mid-sp.pos)*alist[i].arc->phi;
        Vec3 d0 = alist[i].arc->S->pos-sp.pos;
        Vec3 d1 = alist[i].arc->mid-sp.pos;
        Vec3 d2 = alist[i].arc->T->pos-sp.pos;

        Vec3 p0 = sp.pos+sp.rad*(d0)/(sp.rad+alist[i].arc->S->rad);
        Vec3 p1 = sp.pos+sp.rad*(d1)/(sp.rad+alist[i].arc->S->rad);
        Vec3 p2 = sp.pos+sp.rad*(d2)/(sp.rad+alist[i].arc->S->rad);
        Vec3 c0 = Normalize(Cross(Normalize(alist[i].w.pos-alist[i].v.pos),
                                  Normalize(alist[k].w.pos-alist[k].v.pos))+
                            Cross(Normalize(alist[j].w.pos-alist[j].v.pos),
                                  Normalize(alist[i].w.pos-alist[i].v.pos)));
        Vec3 c1 = Normalize(Cross(Normalize(p0-p1),Normalize(p2-p1)));

        if(i>0) {
          plan+=Dot(prev,Normalize(d1));
        }
        prev=Normalize(d1);
        sum2+=c0*alist[i].arc->phi;

        VertexID id0 = va_.Add(p0,Vec3(),Color(1,1,0));
        VertexID id1 = va_.Add(p1,Vec3(),Color(1,1,1));
        VertexID id2 = va_.Add(p2,Vec3(),Color(1,1,0));
        VertexID id3 = va_.Add(sp.pos,Vec3(),Color(1,1,1));
        va_.AddLine(id0,id1);
        va_.AddLine(id1,id2);
        //va_.AddLine(id1,id3);
      }
      float f = plan/static_cast<float>(alist.size());
      VertexID id8 = va_.Add(sp.pos,Vec3(),Color(1,1,1));
      VertexID id9 = va_.Add(sp.pos+1.1*sp.rad*Normalize(sum1),Vec3(),Color(0,0,1));
      VertexID idA = va_.Add(sp.pos+1.1*sp.rad*Normalize(sum2),Vec3(),Color(1,0,0));
      VertexID idB = va_.Add(sp.pos+1.1*sp.rad*Normalize(f*sum1+(1.0-f)*sum2),Vec3(),Color(0,1,0));
      va_.AddLine(id8,id9);
      va_.AddLine(id8,idA);
      va_.AddLine(id8,idB);
#endif
#if 0
      VertexID id0 = va_.Add(sp.pos,Vec3(),Color(1,1,1));
      VertexID id1 = va_.Add(sp.top_list[l],Vec3(),Color(1,1,0));
      va_.AddLine(id0,id1);
#endif
    }
  }

  va_.SetMode(0x2);
  va_.SetLighting(false);
  va_.SetCullFace(false);
  va_.SetColorMaterial(true);

}

Vec3 RSurface::GetCenter() const
{
  return Vec3(0,0,0);
}

void RSurface::CustomRenderGL(RenderPass pass)
{
  if (!(pass==STANDARD_RENDER_PASS || pass==OPAQUE_RENDER_PASS)) 
    return;
  va_.RenderGL();
}

geom::AlignedCuboid RSurface::GetBoundingBox() const
{
  return geom::AlignedCuboid(Vec3(-1,-1,-1), Vec3(1,1,1));
}

void RSurface::ProcessLimits(Vec3& minc, Vec3& maxc, const mol::Transform& tf) const
{
  // nop
}

}} // ns
