//------------------------------------------------------------------------------
// This file is part of the OpenStructure project <www.openstructure.org>
//
// Copyright (C) 2008-2011 by the OpenStructure authors
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

#include <ost/mol/atom_handle.hh>

#include "scene.hh"

#include "prim_list.hh"
#include "povray.hh"

namespace ost { namespace gfx {

PrimList::PrimList(const String& name):
  GfxObj(name),
  points_(),
  lines_(),
  spheres_(),
  cyls_(),
  sphere_detail_(4),
  arc_detail_(4),
  simple_va_()
{}

void PrimList::Clear()
{
  points_.clear();
  lines_.clear();
  spheres_.clear();
  cyls_.clear();
  Scene::Instance().RequestRedraw();
  this->FlagRebuild();
}


geom::AlignedCuboid PrimList::GetBoundingBox() const
{
  if(points_.empty() && lines_.empty() && spheres_.empty() && cyls_.empty()) {
    return geom::AlignedCuboid(geom::Vec3(-1,-1,-1),geom::Vec3(1,1,1));
  }
  geom::Vec3 minc(std::numeric_limits<float>::max(),
                  std::numeric_limits<float>::max(),
                  std::numeric_limits<float>::max());
  geom::Vec3 maxc(-std::numeric_limits<float>::max(),
                  -std::numeric_limits<float>::max(),
                  -std::numeric_limits<float>::max());
  ProcessLimits(minc,maxc,mol::Transform());
  return geom::AlignedCuboid(minc,maxc);
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
  for(PointEntryList::const_iterator it=spheres_.begin();it!=spheres_.end();++it) {
    geom::Vec3 tpos = tf.Apply(it->pos);
    minc=geom::Min(minc,tpos-it->rad);
    maxc=geom::Max(maxc,tpos+it->rad);
  }
  for(LineEntryList::const_iterator it=cyls_.begin();it!=cyls_.end();++it) {
    geom::Vec3 tpos = tf.Apply(it->pos1);
    minc=geom::Min(minc,tpos-it->rad1);
    maxc=geom::Max(maxc,tpos+it->rad1);
    tpos = tf.Apply(it->pos2);
    minc=geom::Min(minc,tpos-it->rad2);
    maxc=geom::Max(maxc,tpos+it->rad2);
  }
  minc-=1.0;
  maxc+=1.0;
}

geom::Vec3 PrimList::GetCenter() const
{
  geom::Vec3 cen;
  size_t sum=0;
  for(PointEntryList::const_iterator it=points_.begin();it!=points_.end();++it) {
    cen+=it->pos;
  }
  sum+=points_.size();
  for(LineEntryList::const_iterator it=lines_.begin();it!=lines_.end();++it) {
    cen+=0.5*(it->pos1+it->pos2);
  }
  sum+=lines_.size();
  for(PointEntryList::const_iterator it=spheres_.begin();it!=spheres_.end();++it) {
    cen+=it->pos;
  }
  sum+=spheres_.size();
  for(LineEntryList::const_iterator it=cyls_.begin();it!=cyls_.end();++it) {
    cen+=0.5*(it->pos1+it->pos2);
  }
  sum+=cyls_.size();
  if(sum>0) {
    cen/=static_cast<float>(sum);
  }
  return cen;
}

void PrimList::OnRenderModeChange()
{
  // noop
}

void PrimList::CustomPreRenderGL(bool flag)
{
  prep_va();
  prep_simple_va();
}

void PrimList::CustomRenderGL(RenderPass pass)
{
  if(pass==STANDARD_RENDER_PASS || pass==TRANSPARENT_RENDER_PASS) {
    va_.RenderGL();
    simple_va_.RenderGL();
  }
}

void PrimList::CustomRenderPov(PovState& pov)
{
  if(points_.empty() && lines_.empty()) return;
  pov.write_merge_or_union(GetName());

  for(PointEntryList::const_iterator it=points_.begin();it!=points_.end();++it) {
    pov.write_sphere(it->pos,0.1,it->col,GetName());
  }
  for(LineEntryList::const_iterator it=lines_.begin();it!=lines_.end();++it) {
    pov.write_sphere(it->pos1,0.1,it->col1,GetName());
    pov.write_sphere(it->pos2,0.1,it->col2,GetName());
    pov.write_cyl(it->pos1,it->pos2,0.1,it->col1,GetName(),true);
  }
  for(PointEntryList::const_iterator it=spheres_.begin();it!=spheres_.end();++it) {
    pov.write_sphere(it->pos,it->rad,it->col,GetName());
  }
  for(LineEntryList::const_iterator it=cyls_.begin();it!=cyls_.end();++it) {
    pov.write_sphere(it->pos1,it->rad1,it->col1,GetName());
    pov.write_sphere(it->pos2,it->rad2,it->col2,GetName());
    pov.write_cyl(it->pos1,it->pos2,it->rad1,it->col1,GetName(),true);
  }
  pov.inc() << " }\n";
}

void PrimList::AddPoint(const geom::Vec3& p, const Color& col)
{
  points_.push_back(PointEntry(p, 0.0, col));
  Scene::Instance().RequestRedraw();
  FlagRebuild();
}

void PrimList::AddLine(const geom::Vec3& p1, const geom::Vec3& p2, const Color& col1, const Color& col2)
{
  lines_.push_back(LineEntry(p1,p2,0.0,0.0,col1,col2));
  Scene::Instance().RequestRedraw();
  FlagRebuild();
}

void PrimList::AddSphere(const geom::Vec3& c, float r, const Color& col)
{
  spheres_.push_back(PointEntry(c, r, col));
  Scene::Instance().RequestRedraw();
  FlagRebuild();
}

void PrimList::AddCyl(const geom::Vec3& p1, const geom::Vec3& p2, float r1, float r2, const Color& col1, const Color& col2)
{
  cyls_.push_back(LineEntry(p1, p2, r1, r2, col1, col2));
  Scene::Instance().RequestRedraw();
  FlagRebuild();
}

void PrimList::SetDiameter(float d)
{
  LOG_WARNING("PrimList::SetDiameter is defunct");
}

void PrimList::SetRadius(float r)
{
  LOG_WARNING("PrimList::SetDiameter is defunct");
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
  for(PointEntryList::iterator it=points_.begin();it!=points_.end();++it) {
    it->col=c;
  }
  for(LineEntryList::iterator it=lines_.begin();it!=lines_.end();++it) {
    it->col1=c;
    it->col2=c;
  }
  for(PointEntryList::iterator it=spheres_.begin();it!=spheres_.end();++it) {
    it->col=c;
  }
  for(LineEntryList::iterator it=cyls_.begin();it!=cyls_.end();++it) {
    it->col1=c;
    it->col2=c;
  }
  Scene::Instance().RequestRedraw();
  FlagRebuild();
}


////////////////////////////////
// private methods

void PrimList::prep_simple_va()
{
  simple_va_.Clear();
  simple_va_.SetLighting(false);
  simple_va_.SetCullFace(false);
  simple_va_.SetColorMaterial(false);
  simple_va_.SetMode(0x3);
  simple_va_.SetTwoSided(true);
  simple_va_.SetAALines(GetAALines());
  simple_va_.SetOpacity(GetOpacity());

  for(PointEntryList::const_iterator it=points_.begin();it!=points_.end();++it) {
    simple_va_.Add(it->pos,geom::Vec3(),it->col);
  }

  for(LineEntryList::const_iterator it=lines_.begin();it!=lines_.end();++it) {
    VertexID id0 = simple_va_.Add(it->pos1,geom::Vec3(),it->col1);
    VertexID id1 = simple_va_.Add(it->pos2,geom::Vec3(),it->col2);
    simple_va_.AddLine(id0,id1);
  }
}

void PrimList::prep_va()
{
  va_.Clear();
  va_.SetLighting(true);
  va_.SetCullFace(true);
  va_.SetColorMaterial(true);
  va_.SetMode(0x4);

  for(PointEntryList::const_iterator it=spheres_.begin();it!=spheres_.end();++it) {
    va_.AddSphere(SpherePrim(it->pos, it->rad, it->col), 
                  GetSphereDetail());
  }

  for(LineEntryList::const_iterator it=cyls_.begin();it!=cyls_.end();++it) {
    va_.AddCylinder(CylinderPrim(it->pos1, it->pos2, it->rad1, it->rad2, it->col1, it->col2),
                    GetArcDetail(),
                    true);
  }
}

}} // ns
