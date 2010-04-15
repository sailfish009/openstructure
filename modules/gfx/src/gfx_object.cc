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

#include <ost/config.hh>
#include <ost/log.hh>
#include <ost/dyn_cast.hh>

#include <ost/mol/entity_property_mapper.hh>

#include "gl_helper.hh"
#include "glext_include.hh"
#include "scene.hh"
#include "gfx_object.hh"

#include "povray.hh"
#include "impl/mapped_property.hh"

#if OST_IMG_ENABLED
#  include <ost/img/alg/stat.hh>
#endif // OST_IMG_ENABLED

namespace ost { namespace gfx {

GfxObj::GfxObj(const String& name):
  GfxNode(name),
  va_(),
  render_mode_(RenderMode::SIMPLE),
  debug_flags_(0),
  transform_(),
  rebuild_(true),
  refresh_(false),
  line_width_(2.0),
  sphere_detail_(4),
  arc_detail_(4),
  spline_detail_(8),
  poly_mode_(2),
  aalines_flag_(false),
  line_halo_(0.0),
  mat_(0.0,1.0,0.5,64.0,0.0),
  mat_dlist_(0),
  mat_update_(true),
  opacity_(1.0),
  smoothf_(0.0),
  omode_(0),
  c_ops_(),
  labels_(),
  use_occlusion_(false)
{
}

GfxObj::~GfxObj()
{}

GfxObj::GfxObj(const GfxObj& o):
  GfxNode("") // to make the compiler happy
{}

GfxObj& GfxObj::operator=(const GfxObj&) {return *this;}

void GfxObj::Apply(GfxNodeVisitor& v, GfxNodeVisitor::Stack st)
{
  v.VisitObject(this,st);
}

int GfxObj::GetType() const
{
  return 1;
}

geom::AlignedCuboid GfxObj::GetBoundingBox() const
{
  return geom::AlignedCuboid(geom::Vec3(),geom::Vec3());
}

void GfxObj::ProcessLimits(geom::Vec3& minc, geom::Vec3& maxc, 
                           const mol::Transform& tf) const
{
  try {
    geom::AlignedCuboid coord_limits=this->GetBoundingBox();
    // update min/max by transforming all 8 corners of the bounding box and 
    // comparing it against the current min/max
    geom::Vec3 mmin=coord_limits.GetMin();
    geom::Vec3 mmax=coord_limits.GetMax();
    geom::Vec3 t1=tf.Apply(geom::Vec3(mmin[0], mmin[1], mmin[2]));
    geom::Vec3 t2=tf.Apply(geom::Vec3(mmin[0], mmax[1], mmin[2]));
    geom::Vec3 t3=tf.Apply(geom::Vec3(mmax[0], mmax[1], mmin[2]));
    geom::Vec3 t4=tf.Apply(geom::Vec3(mmax[0], mmin[1], mmin[2]));
    geom::Vec3 t5=tf.Apply(geom::Vec3(mmin[0], mmin[1], mmax[2]));
    geom::Vec3 t6=tf.Apply(geom::Vec3(mmin[0], mmax[1], mmax[2]));
    geom::Vec3 t7=tf.Apply(geom::Vec3(mmax[0], mmax[1], mmax[2]));
    geom::Vec3 t8=tf.Apply(geom::Vec3(mmax[0], mmin[1], mmax[2]));
    minc = geom::Min(minc, geom::Min(t1, geom::Min(t2, geom::Min(t3, 
                     geom::Min(t4, geom::Min(t5, geom::Min(t6, 
                     geom::Min(t7, t8))))))));
    maxc = geom::Max(maxc, geom::Max(t1, geom::Max(t2, geom::Max(t3, 
                     geom::Max(t4, geom::Max(t5, geom::Max(t6,
                     geom::Max(t7, t8))))))));
  } catch(Error& e) {
    // in case the object is empty...
  }
}

GfxNodeP GfxObj::Copy() const
{
  return GfxNodeP(new GfxObj(*this));
}

void GfxObj::DeepSwap(GfxObj& go)
{
  GfxNode::DeepSwap(*this);
  std::swap(transform_,go.transform_);
  std::swap(rebuild_,go.rebuild_);
  std::swap(refresh_,go.refresh_);
  std::swap(sphere_detail_,go.sphere_detail_);
  std::swap(arc_detail_,go.arc_detail_);
  std::swap(spline_detail_,go.spline_detail_);
  std::swap(poly_mode_,go.poly_mode_);
  std::swap(aalines_flag_,go.aalines_flag_);
  std::swap(line_halo_,go.line_halo_);
  std::swap(mat_,go.mat_);
  std::swap(mat_dlist_,go.mat_dlist_);
  std::swap(mat_update_,go.mat_update_);
  std::swap(opacity_,go.opacity_);
  std::swap(smoothf_,go.smoothf_);
}

void GfxObj::CustomRenderGL(RenderPass pass) {}
void GfxObj::CustomPreRenderGL(bool flag) {}

void GfxObj::CustomRenderPov(PovState& pov) {}

/* 
  this should not be necessary anymore

void GfxObj::RefreshVA(IndexedVertexArray& va)
{

  va.SetLineWidth(GetLineWidth());
  va.SetPolyMode(GetPolyMode());
  va.SetAALines(GetAALines());
  va.SetLineHalo(GetLineHalo());
  va.DrawNormals(debug_flags_&0x1);
  va.SetPolyMode(debug_flags_&0x2 ? 1 : 2);
  va.UseAmbient(use_occlusion_);
  va.FlagRefresh();
}
*/

void GfxObj::OnInput(const InputEvent& e) 
{
  geom::Mat3 rot=gfx::Scene::Instance().GetTransform().GetRot();
  geom::Vec3 cam_trans=gfx::Scene::Instance().GetTransform().GetTrans();  
  float x=e.GetDelta()*M_PI/180.0;
  bool transformed=false;
  if (e.GetCommand()==INPUT_COMMAND_ROTX) {
    transformed=true;
    transform_.SetRot(AxisRotation(geom::Vec3(1,0,0)*rot,x)*transform_.GetRot());
  } else if(e.GetCommand()==INPUT_COMMAND_ROTY) {
    transformed=true;
    transform_.SetRot(AxisRotation(geom::Vec3(0,1,0)*rot,x)*transform_.GetRot());
  } else if(e.GetCommand()==INPUT_COMMAND_ROTZ) {
    transformed=true;
    transform_.SetRot(AxisRotation(geom::Vec3(0,0,1)*rot,x)*transform_.GetRot());
  } else if(e.GetCommand()==INPUT_COMMAND_TRANSX ||
            e.GetCommand()==INPUT_COMMAND_TRANSY) {
 
    double mm[]={1,0,0,0,
                 0,1,0,0,
                 0,0,1,0,
                 transform_.GetTrans()[0]-cam_trans[0],
                 transform_.GetTrans()[1]-cam_trans[1],
                 transform_.GetTrans()[2]-cam_trans[2], 1};
    double pm[16];
    glGetDoublev(GL_PROJECTION_MATRIX,pm);
    GLint vp[4];
    glGetIntegerv(GL_VIEWPORT,vp);
    double wx,wy,wz;
    gluProject(0.0,0.0,0.0,mm,pm,vp,&wx,&wy,&wz);
    double ox,oy,oz;
    gluUnProject(wx+1.0,wy+1.0,wz,mm,pm,vp,&ox,&oy,&oz);

    float fx=ox;
    float fy=oy;   
    geom::Vec3 trans=transform_.GetTrans();
    transformed=true;    
    if (e.GetCommand()==INPUT_COMMAND_TRANSX) {
      trans+=geom::Vec3(-fx*e.GetDelta(), 0.0, 0.0)*rot;
    } else {
      trans+=geom::Vec3(0.0, -fy*e.GetDelta(), 0.0)*rot;
    }
    transform_.SetTrans(trans);
  }
  if (transformed) {
    GfxObjP obj=dyn_cast<GfxObj>(this->shared_from_this());
    Scene::Instance().NodeTransformed(obj);
  }
}

const mol::Transform& GfxObj::GetTF() const
{
  return transform_;
}


void GfxObj::SetTF(const mol::Transform& tf)
{
  transform_=tf;
}

geom::Vec3 GfxObj::GetCenter() const 
{
  return this->GetBoundingBox().GetCenter();
}

bool GfxObj::OnSelect(const geom::Line3& l, geom::Vec3& result, 
                      float zlim, bool pick_flag)
{
  return false;
}

void GfxObj::SetRenderMode(RenderMode::Type m)
{
  if (render_mode_==m) return;
  render_mode_=m;
  OnRenderModeChange();
  FlagRebuild();
}

RenderMode::Type GfxObj::GetRenderMode() const
{
  return render_mode_;
}

void GfxObj::FlagRebuild()
{
  rebuild_=true;
  Scene::Instance().RequestRedraw();
}

void GfxObj::FlagRefresh()
{
  refresh_=true;
  Scene::Instance().RequestRedraw();
}

void GfxObj::SetNormalSmoothFactor(float smoothf)
{
  smoothf_=smoothf;
  FlagRebuild();
}

float GfxObj::GetNormalSmoothFactor() const
{
  return smoothf_;
}

void GfxObj::OnRenderModeChange()
{
  Scene::Instance().ObjectChanged(GetName());
  Scene::Instance().RenderModeChanged(GetName());
}

void GfxObj::SetLineWidth(float w)
{
  va_.SetLineWidth(w);
  line_width_=std::max(float(0.01),w);
  FlagRefresh();
  Scene::Instance().RenderModeChanged(GetName());
}

float GfxObj::GetLineWidth() const
{
  return line_width_;
}

void GfxObj::SetSphereDetail(unsigned int d)
{
  if(d==sphere_detail_) return;
  sphere_detail_=d;
  FlagRebuild();
}

unsigned int GfxObj::GetSphereDetail() const
{
  return sphere_detail_;
}

void GfxObj::SetArcDetail(unsigned int d)
{
  if(d==arc_detail_) return;
  arc_detail_=d;
  FlagRebuild();
}

unsigned int GfxObj::GetArcDetail() const
{
  return arc_detail_;
}

void GfxObj::SetSplineDetail(unsigned int d)
{
  if(d==spline_detail_) return;
  spline_detail_=d;
  FlagRebuild();
}

unsigned int GfxObj::GetSplineDetail() const
{
  return spline_detail_;
}

void GfxObj::SetPolyMode(unsigned int m)
{
  if(m==poly_mode_) return;
  poly_mode_=std::min((unsigned int)2,m);
  va_.SetPolyMode(poly_mode_);
  FlagRefresh();
}

unsigned int GfxObj::GetPolyMode() const
{
  return poly_mode_;
}

void GfxObj::SetAALines(bool f)
{
  if(f==aalines_flag_) return;
  va_.SetAALines(f);
  aalines_flag_=f;
  FlagRefresh();
}

bool GfxObj::GetAALines() const
{
  return aalines_flag_;
}

void GfxObj::SetLineHalo(float f)
{
  va_.SetLineHalo(f);
  line_halo_=f;
  FlagRefresh();
}

float GfxObj::GetLineHalo() const
{
  return line_halo_;
}

void GfxObj::SetOutlineMode(int m)
{
  omode_=m;
  FlagRefresh();
}

void GfxObj::SetOutlineExpandFactor(float f)
{
  va_.SetOutlineExpandFactor(f);
}

void GfxObj::SetOutlineExpandColor(const Color& c)
{
  va_.SetOutlineExpandColor(c);
}

void GfxObj::RenderGL(RenderPass pass)
{
  LOGN_TRACE("object " << GetName() << ": RenderGL()");

  if(pass==0) {
    if(mat_update_) {
      LOGN_TRACE("updating material display list");
      if(mat_dlist_==0) {
        mat_dlist_=glGenLists(1);
      }
      glNewList(mat_dlist_,GL_COMPILE);
      mat_.RenderGL();
      glEndList();
      mat_update_=false;
    }
    
    if(rebuild_ || refresh_) {
      PreRenderGL(rebuild_);
      rebuild_=false;
      refresh_=false;
    }
  }
  if(IsVisible()) {
    LOGN_TRACE("applying local transformation");
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glMultMatrix(transform_.GetTransposedMatrix().Data());
    if(Scene::Instance().InOffscreenMode()) {
      LOGN_TRACE("applying material");
      mat_.RenderGL();
    } else {
      LOGN_TRACE("applying material display list");
      glCallList(mat_dlist_);
    }
    LOGN_TRACE("calling custom render gl pass " << pass);

    CustomRenderGL(pass);

    if(pass==0 && omode_>0) {
      va_.SetOutlineMode(omode_);
      CustomRenderGL(pass);
      va_.SetOutlineMode(0);
    }

    if(pass==1) {
      LOGN_TRACE("drawing labels");
      render_labels();
    }

    glPopMatrix();    
  }
}


void GfxObj::RenderPov(PovState& pov)
{
  if(IsVisible()) {
    pov.start_obj(GetName(),1.0,1.0,1.0);
    // apply local transformation
    // using transform_
    CustomRenderPov(pov);
    pov.end_obj();
  }
}

void GfxObj::PreRenderGL(bool f)
{
  LOGN_DUMP("object " << GetName() << ": PreRenderGL()");
  CustomPreRenderGL(f);
}

void GfxObj::Clear()
{
}

Color GfxObj::Ele2Color(const String& ele)
{
  // TODO 1: allow this table to be set by the user
  // TODO 2: move this table to entity gfx
  if(ele=="C") {return Color(0.83,0.97,0.97);}
  else if(ele=="N") {return Color(0.0,0.0,1.0);}
  else if(ele=="O") {return Color(1.0,0.0,0.0);}
  else if(ele=="S") {return Color(1.0,1.0,0.0);}
  else if(ele=="P") {return Color(1.0,0.0,1.0);}
  else if(ele=="H") {return Color(0.95,0.95,1.0);}
  
  return Color(0.8,0.8,0.8);
}

void GfxObj::AddLabel(const String& s, const geom::Vec3& pos, const Color& col, float psize)
{
  labels_.push_back(TextPrim(s,pos,col,psize));
}

void GfxObj::AddLabel(const String& s, const geom::Vec3& pos, const Color& col)
{
  AddLabel(s,pos,col,1.0);
}

void GfxObj::AddLabel(const String& s, const geom::Vec3& pos, float psize)
{
  AddLabel(s,pos,Color(1,1,1),psize);
}

void GfxObj::AddLabel(const String& s, const geom::Vec3& pos)
{
  AddLabel(s,pos,Color(1,1,1),1.0);
}

void GfxObj::ClearLabels()
{
  labels_.clear();
}

void GfxObj::SetMatAmb(const Color& c)
{
  mat_.SetAmb(c);
  mat_update_=true;
  Scene::Instance().RequestRedraw();
}

void GfxObj::SetMatAmb(float c)
{
  mat_.SetAmb(c);
  mat_update_=true;
  Scene::Instance().RequestRedraw();
}

void GfxObj::SetMatDiff(const Color& c)
{
  mat_.SetDiff(c);
  mat_update_=true;
  Scene::Instance().RequestRedraw();
}

void GfxObj::SetMatDiff(float c)
{
  mat_.SetDiff(c);
  mat_update_=true;
  Scene::Instance().RequestRedraw();
}

void GfxObj::SetMatSpec(const Color& c)
{
  mat_.SetSpec(c);
  mat_update_=true;
  Scene::Instance().RequestRedraw();
}

void GfxObj::SetMatSpec(float c)
{
  mat_.SetSpec(c);
  mat_update_=true;
  Scene::Instance().RequestRedraw();
}

void GfxObj::SetMatShin(float s)
{
  mat_.SetShin(s);
  mat_update_=true;
  Scene::Instance().RequestRedraw();
}

void GfxObj::SetMatEmm(const Color& c)
{
  mat_.SetEmm(c);
  mat_update_=true;
  Scene::Instance().RequestRedraw();
}

void GfxObj::SetMatEmm(float c)
{
  mat_.SetEmm(c);
  mat_update_=true;
  Scene::Instance().RequestRedraw();
}

void GfxObj::SetMat(const Color& amb, const Color& diff, const Color& spec, float shin)
{
  SetMatAmb(amb);
  SetMatDiff(diff);
  SetMatSpec(spec);
  SetMatShin(shin);
  SetMatEmm(0.0);
  mat_update_=true;
  Scene::Instance().RequestRedraw();
}

void GfxObj::SetMat(float amb, float diff, float spec, float shin)
{
  SetMatAmb(amb);
  SetMatDiff(diff);
  SetMatSpec(spec);
  SetMatShin(shin);
  SetMatEmm(0.0);
  mat_update_=true;
  Scene::Instance().RequestRedraw();
}

Material GfxObj::GetMaterial() const
{
  return mat_;
}

void GfxObj::SetMaterial(const Material& m)
{
  mat_=m;
}

void GfxObj::GLCleanup()
{
  if(mat_dlist_!=0) {
    glDeleteLists(mat_dlist_,1);
    mat_dlist_=0;
  }
  OnGLCleanup();
}

void GfxObj::OnGLCleanup()
{}

void GfxObj::SetOpacity(float o)
{
  opacity_=o;
  FlagRebuild();
}

float GfxObj::GetOpacity() const
{
  return opacity_;
}

void GfxObj::SmoothVertices(float smoothf)
{
  va_.SmoothVertices(smoothf);
  FlagRefresh();
}

void GfxObj::AmbientOcclusion(bool f)
{
  va_.UseAmbient(f);
  Scene::Instance().RequestRedraw();
}

void GfxObj::AmbientLocalWeight(float w)
{
  va_.AmbientLocalWeight(w);
  Scene::Instance().RequestRedraw();
}

void GfxObj::AmbientOcclusionWeight(float w)
{
  va_.AmbientOcclusionWeight(w);
  Scene::Instance().RequestRedraw();
}

void GfxObj::ColorBy(const mol::EntityView& ev, 
                      const String& prop,
                      const Gradient& g, float minv, float maxv)
{
  LOGN_VERBOSE("ColorBy not implemented for this gfx object");
}


void GfxObj::ColorBy(const mol::EntityView& ev, 
                      const String& prop,
                      const Color& c1, const Color& c2, float minv, float maxv)
{
  Gradient g;
  g.SetColorAt(0.0,c1);
  g.SetColorAt(1.0,c2);
  this->ColorBy(ev,prop,g,minv,maxv);
}

#if OST_IMG_ENABLED
void GfxObj::ColorBy(const img::MapHandle& mh,
                      const String& prop,
                      const Gradient& g, float minv, float maxv)
{
  LOGN_VERBOSE("ColorBy not implemented for this gfx object");
}

void GfxObj::ColorBy(const img::MapHandle& mh,
                      const String& prop,
                      const Color& c1, const Color& c2, float minv, float maxv)
{
  Gradient g;
  g.SetColorAt(0.0,c1);
  g.SetColorAt(1.0,c2);
  this->ColorBy(mh,prop,g,minv,maxv);
}

void GfxObj::ColorBy(const img::MapHandle& mh,
                      const String& prop,
                      const Gradient& g)
{
  ost::img::alg::Stat stat;
  mh.Apply(stat);
  float min = static_cast<float>(stat.GetMinimum());
  float max = static_cast<float>(stat.GetMaximum());
  std::pair<float,float> minmax = std::make_pair(min,max);
  this->ColorBy(mh,prop,g,minmax.first, minmax.second);
}

void GfxObj::ColorBy(const img::MapHandle& mh,
                      const String& prop,
                      const Color& c1, const Color& c2)
{
  ost::img::alg::Stat stat;
  mh.Apply(stat);
  float min = static_cast<float>(stat.GetMinimum());
  float max = static_cast<float>(stat.GetMaximum());
  std::pair<float,float> minmax = std::make_pair(min,max);
  this->ColorBy(mh,prop,c1,c2,minmax.first, minmax.second);
}

#endif //OST_IMG_ENABLED

void GfxObj::ColorBy(const mol::EntityHandle& eh, 
                      const String& prop,
                      const Gradient& g, float minv, float maxv)
{
  this->ColorBy(eh.CreateFullView(),prop,g,minv,maxv);
}
    
void GfxObj::ColorBy(const mol::EntityHandle& eh, 
                      const String& prop,
                      const Color& c1, const Color& c2, float minv, float maxv)
{
  Gradient g;
  g.SetColorAt(0.0,c1);
  g.SetColorAt(1.0,c2);
  this->ColorBy(eh,prop,g,minv,maxv);
}

void GfxObj::ColorBy(const mol::EntityView& ev, 
                      const String& prop,
                      const Gradient& g)
{
  std::pair<float,float> minmax = ev.GetMinMax(prop);
  this->ColorBy(ev,prop,g,minmax.first, minmax.second);
}
    
void GfxObj::ColorBy(const mol::EntityView& ev, 
                      const String& prop,
                      const Color& c1, const Color& c2)
{
  std::pair<float,float> minmax = ev.GetMinMax(prop);
  this->ColorBy(ev,prop,c1,c2,minmax.first, minmax.second);
}

void GfxObj::ColorBy(const mol::EntityHandle& eh, 
                      const String& prop,
                      const Gradient& g)
{
  this->ColorBy(eh.CreateFullView(),prop,g);
}

void GfxObj::ColorBy(const mol::EntityHandle& eh, 
                      const String& prop,
                      const Color& c1, const Color& c2)
{
  this->ColorBy(eh.CreateFullView(),prop,c1,c2);
}

namespace {

float normalize(float v, float min_v, float max_v)
{
  return (v-min_v)/(max_v-min_v);
}
}

void GfxObj::render_labels() const
{
  if(labels_.empty()) return;
  for(TextPrimList::const_iterator it=labels_.begin();it!=labels_.end();++it) {
    Scene::Instance().RenderText(*it);
  }
}

void GfxObj::ReapplyColorOps(){
  if(c_ops_.size()>0){
    GfxObjP o=dyn_cast<GfxObj>(shared_from_this());
    for(boost::ptr_vector<gfx::ColorOp>::iterator it=c_ops_.begin();
      it!=c_ops_.end();++it) {
	    it->ApplyTo(o);
    }
  }
}

void GfxObj::AppendColorOp(gfx::ColorOp* op){
  c_ops_.push_back(op);
}

void GfxObj::CleanColorOps(){
  c_ops_.release();
}

void GfxObj::Debug(unsigned int flags)
{
  va_.DrawNormals(flags & 0x1);
}

}} // ns
