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
  Author: Ansgar Philippsen, Marco Biasini
*/

#include <ost/log.hh>
#include <ost/profile.hh>
#include <ost/profile.hh>
#include <ost/base.hh>
#include <ost/img/alg/stat.hh>
#include <ost/img/alg/discrete_shrink.hh>

#include "gl_helper.hh"
#include "glext_include.hh"

#include "map_iso.hh"
#include "impl/map_iso_gen.hh"
#include "impl/map_iso_gen_s.hh"
#include "impl/map_iso_gen_o.hh"
#include "impl/octree_isocont.hh"
#include "scene.hh"

#if OST_SHADER_SUPPORT_ENABLED
#include "shader.hh"
#endif

namespace {

int compute_downsampling_fact(const ost::img::ImageHandle& mh)
{
  ost::img::Extent ext = mh.GetExtent();
  int max = ext.GetWidth();
  if (ext.GetHeight() > ext.GetWidth() && ext.GetHeight() > ext.GetDepth()) max=ext.GetHeight();
  if (ext.GetDepth() > ext.GetWidth() && ext.GetDepth() > ext.GetHeight()) max=ext.GetDepth();
  int fact = std::ceil(static_cast<float>(max)/64.0);
  return fact;
}

}

namespace ost { 

namespace gfx {

MapIso::MapIso(const String& name, const img::MapHandle& mh, float level):
  GfxObj(name),
  original_mh_(mh),
  downsampled_mh_(MapIso::DownsampleMap(mh)),
  mh_(downsampled_mh_),
  octree_(mh_),
  level_(level),
  normals_calculated_(false),
  alg_(0),
  smoothf_(0.2),
  debug_octree_(false),
  color_(Color::GREY)
{
  SetMatAmb(Color(0,0,0));
  SetMatDiff(Color(1,1,1));
  SetMatSpec(Color(0.1,0.1,0.1));
  SetMatShin(32);
  mol::Transform tf=this->GetTF();
  tf.SetCenter(this->GetCenter());
  tf.SetTrans(this->GetCenter());
  this->SetTF(tf);
  Rebuild();
}

MapIso::MapIso(const String& name, const img::MapHandle& mh, 
               float level, uint a):
  GfxObj(name),
  original_mh_(mh),
  downsampled_mh_(MapIso::DownsampleMap(mh)),
  mh_(downsampled_mh_),
  octree_(mh_),
  level_(level),
  normals_calculated_(false),
  alg_(a),
  debug_octree_(false),
  color_(Color::GREY)
{
  // TODO replace with def mat for this gfx obj type
  SetMatAmb(Color(0,0,0));
  SetMatDiff(Color(1,1,1));
  SetMatSpec(Color(0.1,0.1,0.1));
  SetMatShin(32);
  mol::Transform tf=this->GetTF();
  tf.SetCenter(this->GetCenter());
  tf.SetTrans(this->GetCenter());  
  this->SetTF(tf);
  Rebuild();
}

geom::AlignedCuboid MapIso::GetBoundingBox() const
{
  geom::Vec3 minc = mh_.IndexToCoord(mh_.GetExtent().GetStart());
  geom::Vec3 maxc = mh_.IndexToCoord(mh_.GetExtent().GetEnd());
  return geom::AlignedCuboid(minc,maxc);
}

geom::Vec3 MapIso::GetCenter() const
{
  geom::Vec3 nrvo = mh_.IndexToCoord(mh_.GetExtent().GetCenter());
  return nrvo;
}
  
void MapIso::OnRenderModeChange()
{
  if(GetRenderMode()==RenderMode::FILL ||
     GetRenderMode()==RenderMode::OUTLINE) {
    if(!normals_calculated_) {
      normals_calculated_=true;
      va_.CalcNormals(smoothf_);
    }
    va_.SetLighting(true);
    va_.SetCullFace(false);
    if(GetRenderMode()==RenderMode::FILL) {
      va_.SetMode(0x4); // filled tris
    } else {
      va_.SetMode(0x6);
    }
    va_.SetTwoSided(true);
  } else {
    render_mode_=RenderMode::SIMPLE;
    va_.SetLighting(false);
    va_.SetCullFace(false);
    va_.SetLineWidth(this->GetLineWidth());
    va_.SetMode(0x2); // only lines
    va_.SetTwoSided(true);
  }
  GfxObj::OnRenderModeChange();
}

void MapIso::CustomPreRenderGL(bool flag)
{
  if(flag) {
    Rebuild();
  } else {
    //RefreshVA(va_);
  }
}

namespace {

struct OctreeDebugger {
  OctreeDebugger(float level, const geom::Vec3& scale): 
    level_(level), scale_(scale) 
  { }
  bool VisitNode(const impl::OctreeNode& node, uint8_t level, 
                 const img::Extent& ext)
  {
    if (node.GetMin()>level_ || node.GetMax()<level_) {
      return false;
    }
    geom::Vec3 s=CompMultiply(scale_, ext.GetStart().ToVec3());
    geom::Vec3 e=CompMultiply(scale_, ext.GetEnd().ToVec3())+scale_;
    glColor3f(1.0, 1.0, 1.0);
    this->DrawBox(s, e);
    return true;
  }
  
  void DrawBox(const geom::Vec3& s, const geom::Vec3& e)
  {
    glBegin(GL_LINES);
      glVertex3(geom::Vec3(s[0], s[1], s[2]));
      glVertex3(geom::Vec3(e[0], s[1], s[2]));
      glVertex3(geom::Vec3(e[0], s[1], s[2]));      
      glVertex3(geom::Vec3(e[0], e[1], s[2]));
      glVertex3(geom::Vec3(e[0], e[1], s[2]));
      glVertex3(geom::Vec3(s[0], e[1], s[2]));
      glVertex3(geom::Vec3(s[0], e[1], s[2]));
      glVertex3(geom::Vec3(s[0], s[1], s[2]));
      
      glVertex3(geom::Vec3(s[0], s[1], e[2]));
      glVertex3(geom::Vec3(e[0], s[1], e[2]));
      glVertex3(geom::Vec3(e[0], s[1], e[2]));      
      glVertex3(geom::Vec3(e[0], e[1], e[2]));
      glVertex3(geom::Vec3(e[0], e[1], e[2]));
      glVertex3(geom::Vec3(s[0], e[1], e[2]));
      glVertex3(geom::Vec3(s[0], e[1], e[2]));
      glVertex3(geom::Vec3(s[0], s[1], e[2]));      
      
      glVertex3(geom::Vec3(s[0], s[1], s[2]));
      glVertex3(geom::Vec3(s[0], s[1], e[2]));
      
      glVertex3(geom::Vec3(s[0], e[1], s[2]));
      glVertex3(geom::Vec3(s[0], e[1], e[2]));
      
      glVertex3(geom::Vec3(e[0], e[1], s[2]));
      glVertex3(geom::Vec3(e[0], e[1], e[2]));
      
      glVertex3(geom::Vec3(e[0], s[1], s[2]));
      glVertex3(geom::Vec3(e[0], s[1], e[2]));
    glEnd();
  }
  
  void VisitLeaf(img::RealSpatialImageState* map, 
                 const img::Point& point) 
  {
    glColor3f(1.0, 1.0, 0.0);
    geom::Vec3 s=CompMultiply(point.ToVec3(), scale_);
    geom::Vec3 e=CompMultiply(point.ToVec3(), scale_)+scale_;    
    this->DrawBox(s, e);
  }
private:
  float level_;
  geom::Vec3 scale_;
};

}

void MapIso::CustomRenderGL(RenderPass pass)
{
  if(pass==STANDARD_RENDER_PASS) {
    va_.RenderGL();
    if (debug_octree_) {
      OctreeDebugger d(level_, mh_.IndexToCoord(img::Point(1,1,1))-
		       mh_.IndexToCoord(img::Point(0,0,0)));
      glPushAttrib(GL_ENABLE_BIT);
      glDisable(GL_LIGHTING);
      octree_.VisitDF(d);
      glPopAttrib();
    }
  }
}

void MapIso::CustomRenderPov(PovState& pov)
{
  // TODO: isocontour pov export
}

void MapIso::OnInput(const InputEvent& e)
{
  GfxObj::OnInput(e);
}

void MapIso::Rebuild()
{
  va_.Clear();
  va_.SetMode(0x2);
  normals_calculated_=false;
  bool triangles=this->GetRenderMode()!=gfx::RenderMode::SIMPLE;
  impl::OctreeIsocont cont(va_, level_, triangles, color_);
  octree_.VisitDF(cont);
  // for normal debugging
#if 0  
  normals_calculated_=true;
  va_.CalcNormals(1.0);
  va_.DrawNormals(true);
#endif  
  OnRenderModeChange();
}

void MapIso::SetLevel(float l)
{
  level_=l;
  Rebuild();
  Scene::Instance().RequestRedraw();
}

float MapIso::GetLevel() const
{
  return level_;
}

float MapIso::GetStdDev() const
{
  img::alg::Stat stat;
  mh_.Apply(stat);
  return stat.GetStandardDeviation();
}

img::ImageHandle& MapIso::GetMap()
{
  return mh_;
}

img::ImageHandle& MapIso::GetOriginalMap()
{
  return original_mh_;
}

float MapIso::GetMean() const
{
  img::alg::Stat stat;
  mh_.Apply(stat);
  return static_cast<float>(stat.GetMean());
}

void MapIso::SetNSF(float nsf)
{
  smoothf_=nsf;
  Rebuild();
  Scene::Instance().RequestRedraw();
}

img::ImageHandle MapIso::DownsampleMap(const img::ImageHandle& mh)
{
  uint downsampling_fact = compute_downsampling_fact(mh);
  img:: ImageHandle ret_mh = mh;
  if (downsampling_fact != 1) {
    img::alg::DiscreteShrink shrink_alg(img::Size(downsampling_fact,downsampling_fact,downsampling_fact));
    ret_mh = mh.Apply(shrink_alg);
  }
  return ret_mh;
}

}} // ns
