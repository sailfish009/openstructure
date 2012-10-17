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
  Author: Ansgar Philippsen, Marco Biasini
*/

#include <ost/log.hh>
#include <ost/profile.hh>
#include <ost/profile.hh>
#include <ost/base.hh>
#include <ost/img/alg/discrete_shrink.hh>
#include <ost/img/alg/histogram.hh>

#include "gl_helper.hh"
#include "glext_include.hh"

#include "map_iso.hh"
#include "impl/map_iso_gen.hh"
#include "impl/map_iso_gen_s.hh"
#include "impl/map_iso_gen_o.hh"
#include "impl/octree_isocont.hh"
#include "impl/map_octree.hh"
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

namespace ost { namespace gfx {


MapIso::MapIso(const String& name, const img::XtalMapPtr& map, float level, uint a):
  GfxObj(name),
  original_mh_(),
  downsampled_mh_(),
  octree_(new impl::MapOctree(false)),
  xtal_map_(map),
  level_(level),
  alg_(a)
{
  this->SetVisibleExtent(img::Extent(img::Size(32,32,32)));
  this->Init();
}


MapIso::MapIso(const String& name, const img::MapHandle& mh, 
               float level, uint a):
  GfxObj(name),
  original_mh_(mh),
  downsampled_mh_(),
  mh_(MapIso::DownsampleMap(mh)),
  octree_(new impl::MapOctree(mh_, false)),
  stat_calculated_(false),
  histogram_calculated_(false),
  histogram_bin_count_(100),
  level_(level),
  normals_calculated_(false),
  alg_(a),
  debug_octree_(false),
  color_(1.0,1.0,1.0),
  bb_(),
  recalc_bb_(true)
{
  this->Init();
}

void MapIso::Init()
{
  stat_calculated_=false;
  histogram_calculated_=false;
  histogram_bin_count_=100;
  normals_calculated_=false;
  debug_octree_=false;
  color_=Color(1.0,1.0,1.0);
  near_slab_=30;
  far_slab_=60;
  slabbing_=false;
  // TODO replace with def mat for this gfx obj type
  if (!xtal_map_ && mh_ != original_mh_) {
    downsampled_mh_ = mh_;
  }
  SetMatAmb(Color(0,0,0));
  SetMatDiff(Color(1,1,1));
  SetMatSpec(Color(0.1,0.1,0.1));
  SetMatShin(32);
  geom::Transform tf=this->GetTF();
  tf.SetCenter(this->GetCenter());
  tf.SetTrans(this->GetCenter());  
  this->SetLineWidth(1.0);
  this->SetTF(tf);
  if (octree_) {
    octree_->Initialize();
    visible_extent_=mh_.GetExtent();
    octree_->SetNewMap(mh_);
  }
  Rebuild();  
}

geom::AlignedCuboid MapIso::GetBoundingBox(bool use_tf) const
{
  if(recalc_bb_) {
    bb_=va_.GetBoundingBox();
    recalc_bb_=false;
  }
  return use_tf ? transform_.Apply(bb_) : bb_;
}

geom::Vec3 MapIso::GetCenter() const
{
  if(recalc_bb_) GetBoundingBox();
  return bb_.GetCenter();
}


MapIso::~MapIso()
{
  if (octree_) {
    delete octree_;
    octree_=NULL;
  }
}
void MapIso::UpdateRenderParams()
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
}

void MapIso::OnRenderModeChange()
{
  this->UpdateRenderParams();
  this->FlagRebuild();
  GfxObj::OnRenderModeChange();
}

void MapIso::CustomPreRenderGL(bool flag)
{
  if(flag) {
    Rebuild();
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
  
  void VisitLeaf(const impl::MapOctree& octree, img::RealSpatialImageState* map, 
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
void MapIso::SetupSlabPlanes()
{
  if (!slabbing_) {
    return;
  }
  glMatrixMode(GL_MODELVIEW);
  glPushMatrix();
  glLoadIdentity();
  double far_plane[4]={ 0, 0, 1, far_slab_ };
  double near_plane[4]={ 0, 0,-1, -near_slab_ };
  glClipPlane(GL_CLIP_PLANE0, near_plane);
  glClipPlane(GL_CLIP_PLANE1, far_plane);
  glPopMatrix();
}
void MapIso::CustomRenderGL(RenderPass pass)
{
  if(pass==STANDARD_RENDER_PASS) {
    
    this->SetupSlabPlanes();
    if (slabbing_) {
      glEnable(GL_CLIP_PLANE0);
      glEnable(GL_CLIP_PLANE1);
    }
    va_.RenderGL();
    if (debug_octree_) {
      OctreeDebugger d(level_, mh_.IndexToCoord(img::Point(1,1,1))-
                       mh_.IndexToCoord(img::Point(0,0,0)));
      glPushAttrib(GL_ENABLE_BIT);
      glDisable(GL_LIGHTING);
      octree_->VisitDF(d);
      glPopAttrib();
    }
    if (slabbing_) {
      glDisable(GL_CLIP_PLANE0);
      glDisable(GL_CLIP_PLANE1);
    }
  }
}

void MapIso::CustomRenderPov(PovState& pov)
{
}

void MapIso::OnInput(const InputEvent& e)
{
  GfxObj::OnInput(e);
}

void MapIso::RebuildOctree()
{
  if (mh_.IsFrequency() == true){
    throw Error("Map not in real space.");
  }
  if (octree_->IsMapManageable(mh_) == false) {
    throw Error("Map is too big for visualization");
  }
  if (IfOctreeDirty()==true) {
    octree_->Initialize();
    dirty_octree_=false;
  }
  va_.Clear();
  va_.SetMode(0x2);
  normals_calculated_=false;
  bool triangles=this->GetRenderMode()!=gfx::RenderMode::SIMPLE;
  impl::OctreeIsocont cont(va_, level_, triangles, color_);
  octree_->VisitDF(cont, true);
  // for normal debugging  
}

void MapIso::Rebuild()
{
  this->RebuildOctree();
#if 0  
  normals_calculated_=true;
  va_.CalcNormals(1.0);
  va_.DrawNormals(true);
#endif  
  this->UpdateRenderParams();  
  recalc_bb_=true;
}

void MapIso::SetLevel(float l)
{
  level_=l;
  Rebuild();
  Scene::Instance().RequestRedraw();
}

void MapIso::CalculateStat() const
{
  mh_.ApplyIP(stat_);
  stat_calculated_=true;
}

void MapIso::CalculateHistogram() const
{
  histogram_ = img::alg::HistogramBase(histogram_bin_count_, 
                                       this->GetMinLevel(), this->GetMaxLevel());
  mh_.ApplyIP(histogram_);
  histogram_calculated_=true;
}

float MapIso::GetMinLevel() const
{
  if(!stat_calculated_)CalculateStat();
  return stat_.GetMinimum();
}

float MapIso::GetMaxLevel() const
{
  if(!stat_calculated_)CalculateStat();
  return stat_.GetMaximum();
}

float MapIso::GetLevel() const
{
  return level_;
}

float MapIso::GetStdDev() const
{
  if(!stat_calculated_)CalculateStat();
  return stat_.GetStandardDeviation();
}

void MapIso::SetHistogramBinCount(int count)
{
  if (count > 0){
    histogram_bin_count_ = count;
    histogram_calculated_ = false;
  }
}

int MapIso::GetHistogramBinCount() const
{
  return histogram_bin_count_;
}

std::vector<int> MapIso::GetHistogram() const
{  
  std::vector<int> a;
  return a;
  if (!histogram_calculated_)
    CalculateHistogram();
  return histogram_.GetBins();
}

img::ImageHandle& MapIso::GetMap()
{
  return mh_;
}

img::ImageHandle& MapIso::GetOriginalMap()
{
  return original_mh_;
}

img::ImageHandle& MapIso::GetDownsampledMap()
{
  return downsampled_mh_;
}

float MapIso::GetMean() const
{
  if(!stat_calculated_)CalculateStat();
  return static_cast<float>(stat_.GetMean());
}

void MapIso::SetNSF(float nsf)
{
  smoothf_=nsf;
  Rebuild();
  Scene::Instance().RequestRedraw();
}

/// \brief sets the donsampled map to active
void MapIso::ShowDownsampledMap()
{
  if (this->HasXtalMap()) {
    return;
  }
  if (downsampled_mh_==mh_) return;
  if (downsampled_mh_.IsValid()) mh_ = downsampled_mh_;
  
  octree_->SetNewMap(mh_);  
  visible_extent_=octree_->GetVisibleExtent();
  MakeOctreeDirty();
  stat_calculated_ = false;
  histogram_calculated_ = false;
  Rebuild();
  Scene::Instance().RequestRedraw();
}

/// \brief sets the original map to active
void MapIso::ShowOriginalMap()
{
  if (this->HasXtalMap()) {
    return;
  }
  if (original_mh_==mh_) 
    return;

  if (original_mh_.IsValid()) {
    mh_ = original_mh_;
  }
  
  octree_->SetNewMap(mh_);  
  MakeOctreeDirty();
  stat_calculated_ = false;
  histogram_calculated_ = false;
  Rebuild();
  Scene::Instance().RequestRedraw();
}


void MapIso::MakeOctreeDirty()
{
  dirty_octree_=true;
}

MapIsoType MapIso::GetShownMapType() const
{
   MapIsoType ret = ORIGINAL_MAP;
   if (mh_ == downsampled_mh_) {
     ret = DOWNSAMPLED_MAP;
   }
   return ret;
}

bool MapIso::IfOctreeDirty() const
{
  return dirty_octree_;
}

/// \brief checks if the downsampled map is available
bool MapIso::IsDownsampledMapAvailable() const
{
  return !(downsampled_mh_==img::ImageHandle());
}

img::ImageHandle MapIso::DownsampleMap(const img::ImageHandle& mh)
{
  uint downsampling_fact = compute_downsampling_fact(mh);
  img:: ImageHandle ret_mh = mh;
  if (downsampling_fact != 1) {
    LOG_INFO("Downsampling map for more comfortable visualization")
    img::alg::DiscreteShrink shrink_alg(img::Size(downsampling_fact,
                                                  downsampling_fact,
                                                  downsampling_fact));
    ret_mh = mh.Apply(shrink_alg);
  }
  return ret_mh;
}

void MapIso::CenterOn(const geom::Vec3& center, const img::Size& size)
{
  if (!this->HasXtalMap()) {
    return;
  }
  img::Point center_in_px(xtal_map_->CoordToIndex(center));
  this->SetVisibleExtent(img::Extent(size, center_in_px));
}


void MapIso::CenterOn(const geom::Vec3& center, const geom::Vec3& size)
{
  if (!this->HasXtalMap()) {
    return;
  }
  img::Point center_in_px(xtal_map_->CoordToIndex(center));
  img::Size size_in_px(geom::CompDivide(size, xtal_map_->GetSamplingVec()));
  this->SetVisibleExtent(img::Extent(size_in_px, center_in_px));
  
}

void MapIso::CenterOn(const geom::Vec3& center, Real size)
{
  if (!this->HasXtalMap()) {
    return;
  }
  img::Point center_in_px(xtal_map_->CoordToIndex(center));
  const geom::Vec3& sampling=xtal_map_->GetSamplingVec();
  img::Size size_in_px(size/sampling.x, size/sampling.y, size/sampling.z);
  this->SetVisibleExtent(img::Extent(size_in_px, center_in_px));
}

void MapIso::SetVisibleExtent(const img::Extent& vis_extent)
{

  if (vis_extent==visible_extent_) {
    return;
  }
  visible_extent_=vis_extent;
  if (this->HasXtalMap()) {
    mh_=xtal_map_->Extract(visible_extent_);
    octree_->SetNewMap(mh_);
    this->MakeOctreeDirty();
    octree_->SetVisibleExtent(visible_extent_);    
  }
  this->FlagRebuild();
}

}} // ns
