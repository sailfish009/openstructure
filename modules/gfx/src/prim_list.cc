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

#include "scene.hh"

#include "prim_list.hh"

namespace ost { namespace gfx {

PrimList::PrimList(const String& name):
  GfxObj(name),
  points_(),
  lines_(),
  radius_(0.5),
  diameter_(0.5),
  sphere_detail_(4),
  arc_detail_(4)
{}

void PrimList::Clear()
{
  points_.clear();
  lines_.clear();
  Scene::Instance().RequestRedraw();
}

void PrimList::ProcessLimits(geom::Vec3& minc, geom::Vec3& maxc, 
                             const mol::Transform& tf) const
{
  for(PointEntryList::const_iterator it=points_.begin();it!=points_.end();++it) {
    geom::Vec3 tpos = tf.Apply(it->pos);
    minc=geom::Min(minc,tpos);
    maxc=geom::Max(maxc,tpos);
  }
  for(LineEntryList::const_iterator it=lines_.begin();it!=lines_.end();++it) {
    geom::Vec3 tpos = tf.Apply(it->pos1);
    minc=geom::Min(minc,tpos);
    maxc=geom::Max(maxc,tpos);
    tpos = tf.Apply(it->pos2);
    minc=geom::Min(minc,tpos);
    maxc=geom::Max(maxc,tpos);
  }
  minc-=1.0;
  maxc+=1.0;
}

geom::Vec3 PrimList::GetCenter() const
{
  geom::Vec3 cen;
  for(PointEntryList::const_iterator it=points_.begin();it!=points_.end();++it) {
    cen+=it->pos;
  }
  for(LineEntryList::const_iterator it=lines_.begin();it!=lines_.end();++it) {
    cen+=0.5*(it->pos1+it->pos2);
  }
  if(!lines_.empty() || !points_.empty()) {
    cen/=static_cast<float>(points_.size()+lines_.size());
  }
  return cen;
}

void PrimList::OnRenderModeChange()
{
  if(GetRenderMode()==RenderMode::CUSTOM) {
    render_custom();
  } else {
    render_simple();
  }
  // this does not work
  //GfxObj::OnRenderModeChange();
}

void PrimList::CustomPreRenderGL(bool flag)
{
  if(flag) {
    if(GetRenderMode()==RenderMode::CUSTOM) {
      render_custom();
    } else {
      render_simple();
    }
  }
}

namespace {

struct AALineEntry {
  float p0[3],p1[3];
  float edge0[3],edge1[3],edge2[3],edge3[3];
  float color[4];
  float z;
};

geom::Vec3 make_edge(const geom::Vec2& c1, const geom::Vec2& c0, float s)
{
  geom::Vec3 nrvo(c1[1]-c0[1],c0[0]-c1[0],c1[0]*c0[1]-c0[0]*c1[1]);
  nrvo*=1.0/(s*Length(c1-c0));
  return nrvo;
}

struct AALineEntryLess
{
  bool operator()(const AALineEntry& e1, const AALineEntry& e2)
  {
    // provides back-to-front sorting
    return e1.z<e2.z;
  }
};

}

void PrimList::CustomRenderGL(RenderPass pass)
{
  if(pass!=STANDARD_RENDER_PASS) return;
  va_.RenderGL();
}

void PrimList::CustomRenderPov(PovState& pov)
{
}

void PrimList::AddPoint(geom::Vec3& p, const Color& col)
{
  points_.push_back(PointEntry(p,col));
  Scene::Instance().RequestRedraw();
  FlagRebuild();
}

void PrimList::AddLine(geom::Vec3& p1, geom::Vec3& p2, const Color& col)
{
  lines_.push_back(LineEntry(p1,p2,col));
  Scene::Instance().RequestRedraw();
  FlagRebuild();
}

void PrimList::SetDiameter(float d)
{
  diameter_=d;
  Scene::Instance().RequestRedraw();
  FlagRebuild();
}

void PrimList::SetRadius(float r)
{
  radius_=r;
  Scene::Instance().RequestRedraw();
  FlagRebuild();
}

void PrimList::SetSphereDetail(unsigned int d)
{
  sphere_detail_=d;
  Scene::Instance().RequestRedraw();
  FlagRebuild();
}

void PrimList::SetArcDetail(unsigned int d)
{
  arc_detail_=d;
  Scene::Instance().RequestRedraw();
  FlagRebuild();
}

void PrimList::SetColor(const Color& c)
{
  for(LineEntryList::iterator it=lines_.begin();it!=lines_.end();++it) {
    it->color=c;
  }
  Scene::Instance().RequestRedraw();
  FlagRebuild();
}


////////////////////////////////
// private methods

void PrimList::render_simple()
{
  va_.Clear();
  va_.SetLighting(false);
  va_.SetCullFace(false);
  va_.SetColorMaterial(false);
  va_.SetMode(0x3);
  va_.SetTwoSided(true);
  va_.SetAALines(GetAALines());

  for(PointEntryList::const_iterator it=points_.begin();it!=points_.end();++it) {
    va_.Add(it->pos,geom::Vec3(),it->color);
  }

  for(LineEntryList::const_iterator it=lines_.begin();it!=lines_.end();++it) {
    VertexID id0 = va_.Add(it->pos1,geom::Vec3(),it->color);
    VertexID id1 = va_.Add(it->pos2,geom::Vec3(),it->color);
    va_.AddLine(id0,id1);
  }
}

void PrimList::render_custom()
{
  va_.Clear();
  va_.SetLighting(true);
  va_.SetCullFace(true);
  va_.SetColorMaterial(true);
  va_.SetMode(0x4);

  for(PointEntryList::const_iterator it=points_.begin();it!=points_.end();++it) {
    va_.AddSphere(SpherePrim(it->pos, radius_, it->color),
                  GetSphereDetail());
  }

  for(LineEntryList::const_iterator it=lines_.begin();it!=lines_.end();++it) {
    va_.AddSphere(SpherePrim(it->pos1, diameter_/2.0, it->color),
                  GetSphereDetail());
    va_.AddSphere(SpherePrim(it->pos2, diameter_/2.0, it->color),
                  GetSphereDetail());
    va_.AddCylinder(CylinderPrim(it->pos1,it->pos2,diameter_/2.0,it->color),
                    GetArcDetail());
  }
}

}} // ns
