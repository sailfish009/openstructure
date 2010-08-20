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

#include <ost/message.hh>
#include <sstream>
#include <limits>
#include <algorithm>
#include <cstring>

#ifndef _MSC_VER
#include <sys/time.h>
#endif

#include <boost/bind.hpp>
#include <boost/shared_array.hpp>
#include <boost/filesystem/operations.hpp>
#include <boost/filesystem/fstream.hpp>
#include <ost/platform.hh>
#include <ost/log.hh>
#include <ost/dyn_cast.hh>
#include <ost/geom/geom.hh>

#include "gl_helper.hh"

#include <ost/config.hh>
#include "offscreen_buffer.hh"
#include "scene.hh"
#include "input.hh"
#include "gfx_node.hh"
#include "gfx_object.hh"
#include "gfx_test_object.hh"
#include "bitmap_io.hh"
#include "entity.hh"
#include "povray.hh"

#if OST_SHADER_SUPPORT_ENABLED
# include "shader.hh"
#endif

//#define DEBUG_SMAP

using boost::bind;

namespace ost {

using namespace geom;

namespace gfx {

namespace bf = boost::filesystem;

#ifndef uchar
#define uchar unsigned char
#endif

namespace {

Mat3 calc_light_rot(const Vec3& ldir)
{
  Mat3 nrvo=AxisRotation(Cross(-ldir,Vec3(0,0,-1)),
                                     -acos(Dot(ldir,Vec3(0,0,-1))));
  return nrvo;
}

}

// meyers singleton
Scene& Scene::Instance()
{
  static Scene inst;
  return inst;
}

Scene::Scene():
  win_(0),
  root_node_(new GfxNode("Scene")),
  observers_(),
  transform_(),
  fov_(30.0),
  znear_(-200.0),zfar_(200.0),
  fnear_(5.0), ffar_(0.0),
  vp_width_(100),vp_height_(100),
  scene_view_stack_(),
  aspect_ratio_(1.0),
  background_(Color(0.0,0.0,0.0,0.0)),
  light_dir_(Normalize(Vec3(-2,-1,-5))),
  light_rot_(calc_light_rot(light_dir_)),
  light_amb_(0.1,0.1,0.1),
  light_diff_(0.9,0.9,0.9),
  light_spec_(0.5,0.5,0.5),
  axis_flag_(false),
  fog_flag_(true),
  fog_color_(0.0,0.0,0.0,0.0),
  shadow_flag_(false),
  shadow_quality_(1),
  texture_id_(),
  auto_autoslab_(true),
  offscreen_flag_(false),
  selection_mode_(1),
  test_flag_(false),
  tmp_tex_(),
  glyph_tex_id_(0),
  glyph_map_(),
  def_text_size_(32.0),
  blur_count_(0),
  blur_buffer_(),
  stereo_(0),
  stereo_inverted_(false),
  stereo_eye_(0),
  stereo_eye_dist_(150.0),
  stereo_eye_offset_(10.0)
{
  transform_.SetTrans(Vec3(0,0,-100));
}

void Scene::SetFog(bool f)
{
  fog_flag_=f;
  if(f) {
    glEnable(GL_FOG);
  } else {
    glDisable(GL_FOG);
  }
  RequestRedraw();
}

void Scene::SetFogColor(const Color& c)
{
  GLfloat fogc[]={c.Red(),c.Green(),c.Blue(),1.0};
  glFogfv(GL_FOG_COLOR,fogc);
  fog_color_=c;
  RequestRedraw();
}

void Scene::SetShadow(bool f)
{
  shadow_flag_=f;
  RequestRedraw();
}

bool Scene::GetShadow()
{
  return shadow_flag_;
}

void Scene::SetShadowQuality(int q)
{
  shadow_quality_=std::min(3,std::max(0,q));
  RequestRedraw();
}

namespace {

void set_light_dir(Vec3 ld)
{
  GLfloat l_pos[]={0.0, 0.0, 0.0, 0.0};
  l_pos[0]=-ld[0]; l_pos[1]=-ld[1]; l_pos[2]=-ld[2];
  glLightfv(GL_LIGHT0, GL_POSITION, l_pos);
}

}

void Scene::InitGL()
{
  LOG_DEBUG("scene: initializing GL state" << std::endl);

#if OST_SHADER_SUPPORT_ENABLED
  Shader::Instance().PreGLInit();
#endif

  // TODO: add more lights
  glLightfv(GL_LIGHT0, GL_AMBIENT, light_amb_);
  glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diff_);
  glLightfv(GL_LIGHT0, GL_SPECULAR, light_spec_);
  set_light_dir(light_dir_);
  glEnable(GL_LIGHT0);
  glLightModeli(GL_LIGHT_MODEL_LOCAL_VIEWER,GL_FALSE);
  glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, GL_FALSE);
  GLfloat l_amb[]={0.0,0.0,0.0,0.0};
  glLightModelfv(GL_LIGHT_MODEL_AMBIENT, l_amb);
#ifndef _MSC_VER
  glLightModeli(GL_LIGHT_MODEL_COLOR_CONTROL, GL_SEPARATE_SPECULAR_COLOR);
#endif
  glEnable(GL_LIGHTING);   
  // set diffuse component to current color
  glColorMaterial(GL_FRONT_AND_BACK, GL_DIFFUSE);
  glEnable(GL_COLOR_MATERIAL);

  // depth buffer
  glEnable(GL_DEPTH_TEST);
  glDepthFunc(GL_LEQUAL);
  glClearDepth(1.0);

  // background
  SetBackground(background_);
  // polygon orientation setting
  glFrontFace(GL_CCW);
  // blending
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  glEnable(GL_BLEND);  
  // depth effect thru fog
  GLfloat fogc[]={fog_color_.Red(),fog_color_.Green(),fog_color_.Blue(),1.0};
  glFogfv(GL_FOG_COLOR,fogc);
  glFogi(GL_FOG_MODE,GL_LINEAR);
  if(fog_flag_) {
    glEnable(GL_FOG);
  } else {
    glDisable(GL_FOG);
  }

  // some polygon settings
  glPolygonMode(GL_FRONT_AND_BACK,GL_FILL);

  // smooth shading across polygon faces
  glShadeModel(GL_SMOOTH);

  // line and point anti-aliasing
  glEnable(GL_LINE_SMOOTH);
  glDisable(GL_POINT_SMOOTH);
  glDisable(GL_POLYGON_SMOOTH);
  // rendering hints
  glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
  glHint(GL_FOG_HINT, GL_NICEST);
  glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);
  glHint(GL_POINT_SMOOTH_HINT, GL_NICEST);
  // full color only
  glDisable(GL_DITHER);
  glDisable(GL_NORMALIZE);
  // double-buffer rendering requires this
  // glDrawBuffer(GL_BACK);
  // initialize shaders
#if OST_SHADER_SUPPORT_ENABLED
  Shader::Instance().Setup();
  Shader::Instance().Activate("fraglight");

  glGenTextures(1,&texture_id_);
#endif

  prep_glyphs();
}

void Scene::RequestRedraw()
{
  if (win_) {
    win_->DoRefresh();
  }
}

void Scene::StatusMessage(const String& s)
{
  if(win_) win_->StatusMessage(s);
}

void Scene::SetBackground(const Color& c)
{
  glClearColor(c.Red(),c.Green(),c.Blue(),c.Alpha());
  background_=c;
  SetFogColor(c);
  RequestRedraw();
}

Viewport Scene::GetViewport() const
{
  Viewport vp;
  glGetIntegerv(GL_VIEWPORT, reinterpret_cast<GLint*>(&vp));
  return vp;
}

void Scene::SetViewport(int w, int h)
{
  glViewport(0,0,w,h);
  vp_width_=w;
  vp_height_=h;
  aspect_ratio_=static_cast<float>(w)/static_cast<float>(h);
  ResetProjection();
}

void Scene::Resize(int w, int h)
{
  SetViewport(w,h);
  RequestRedraw();
}

void Scene::SetCenter(const Vec3& cen)
{
  Vec3 tcen=transform_.Apply(cen);
  float delta_z = tcen[2]-transform_.GetTrans()[2];

  transform_.SetCenter(cen);
  SetNearFar(znear_+delta_z,zfar_+delta_z);
  RequestRedraw();  
}

Vec3 Scene::GetCenter() const
{
  return transform_.GetCenter();
}

void Scene::CenterOn(const String& s)
{
  CenterOn((*this)[s]);
}

void Scene::CenterOn(const GfxObjP& go)
{
  if(go) {
    this->SetCenter(go->GetCenter());
  }
}

void Scene::RenderText(const TextPrim& t)
{
  if(t.str.empty() || t.points<=0.0) return;
  Vec3 ppos = Project(t.position,false);

  float gh = t.points*def_text_size_;
  float gw = gh*0.5;
  float tlen = static_cast<float>(t.str.size());

  Vec3 pos0 = t.position;
  Vec3 pos1 = UnProject(ppos+Vec3(0.0,gh,0.0),false);
  Vec3 pos2 = UnProject(ppos+Vec3(tlen*gw,gh,0.0),false);
  Vec3 pos3 = UnProject(ppos+Vec3(tlen*gw,0.0,0.0),false);

  Vec3 delta21 = (pos2-pos1)/tlen;
  Vec3 delta30 = (pos3-pos0)/tlen;

#if OST_SHADER_SUPPORT_ENABLED
  Shader::Instance().PushProgram();
  Shader::Instance().Activate("");
#endif

  glPushAttrib(GL_ENABLE_BIT | GL_COLOR_BUFFER_BIT);
  glDisable(GL_NORMALIZE);
  glDisable(GL_CULL_FACE);
  glDisable(GL_LIGHTING);
  glDisable(GL_COLOR_MATERIAL);
  glEnable(GL_TEXTURE_2D);
  glTexEnvf(GL_TEXTURE_ENV,GL_TEXTURE_ENV_MODE,GL_MODULATE);
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
  glBindTexture(GL_TEXTURE_2D,glyph_tex_id_);

  float tdx=1.0/16.0;
  float tdy=1.0/8.0;

  glBegin(GL_QUADS);
  glColor3fv(t.color);
  for(unsigned int c=0;c<t.str.size();++c) {
    float tx=glyph_map_[t.str[c]][0];
    float ty=glyph_map_[t.str[c]][1];
    Vec3 dir1 = static_cast<float>(c)*delta30;
    Vec3 dir2 = static_cast<float>(c)*delta21;

    glTexCoord2d(tx,ty);
    glVertex3v(Vec3(pos0+dir1).Data());

    glTexCoord2d(tx,ty+tdy);
    glVertex3v(Vec3(pos1+dir2).Data());

    glTexCoord2d(tx+tdx,ty+tdy);
    glVertex3v(Vec3(pos1+dir2+delta30).Data());

    glTexCoord2d(tx+tdx,ty);
    glVertex3v(Vec3(pos0+dir1+delta30).Data());
  }
  glEnd();

  glPopAttrib();

#if OST_SHADER_SUPPORT_ENABLED
  Shader::Instance().PopProgram();
#endif
}

float Scene::GetDefaultTextSize()
{
  return def_text_size_;
}

namespace {

void draw_lightdir(const Vec3& ldir, const mol::Transform& tf)
{
  glPushAttrib(GL_ENABLE_BIT);
  glMatrixMode(GL_MODELVIEW);
  glPushMatrix();
  glLoadIdentity();
  mol::Transform tmpt(tf);
  tmpt.SetRot(Mat3::Identity());
  glMultMatrix(tmpt.GetTransposedMatrix().Data());
  glDisable(GL_NORMALIZE);
  glDisable(GL_CULL_FACE);
  glDisable(GL_LIGHTING);
  glDisable(GL_COLOR_MATERIAL);
  glColor3f(1.0,0.0,1.0);
  glLineWidth(2.0);
  glBegin(GL_LINES);
  glVertex3v(tmpt.GetCenter().Data());
  glVertex3v((tmpt.GetCenter()-50.0*ldir).Data());
  glEnd();
  glPopMatrix();
  glPopAttrib();
}

}

void Scene::RenderGL()
{
  if(auto_autoslab_) Autoslab(false, false);

  #if OST_SHADER_SUPPORT_ENABLED
    if(shadow_flag_) {
      prep_shadow_map();
  #if DEBUG_SMAP
      return;
  #endif
    }
  #endif
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  prep_blur();

  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();

  set_light_dir(light_dir_);

  glMultMatrix(transform_.GetTransposedMatrix().Data());

  if(stereo_==2 || stereo_==3) {
    render_interlaced_stereo();
  } else if (stereo_==1) {
    this->render_quad_buffered_stereo();
  } else {
    this->render_scene_with_glow();
  }
  #if OST_SHADER_SUPPORT_ENABLED
  if(shadow_flag_) {
    glDisable(GL_TEXTURE_2D);
  }
  #endif
}

void Scene::Register(GLWinBase* win)
{
  LOG_DEBUG("scene: registered win @" << win << std::endl);
  win_=win;
}

void Scene::Unregister(GLWinBase* win)
{
  LOG_DEBUG("scene: unregistered win @" << win << std::endl);
  win_=0;
}

template <typename ACTION>
void Scene::NotifyObservers(const ACTION& action) {
  std::for_each(observers_.begin(), observers_.end(), action);
}

namespace {
  struct FindNode: public GfxNodeVisitor {
    FindNode(const String& n): nam(n), node() {}
    virtual bool VisitNode(GfxNode* n, const Stack& st) {
      if(n->GetName()==nam) node=n->shared_from_this();
      return true;
    }
    virtual void VisitObject(GfxObj* o, const Stack& st) {
      if(o->GetName()==nam) node=o->shared_from_this();
    }
    String nam;
    GfxNodeP node;
  };

  struct FindObj: public GfxNodeVisitor {
    FindObj(const String& n): nam(n), node() {}
    virtual bool VisitNode(GfxNode* n, const Stack& st) {
      return true;
    }
    virtual void VisitObject(GfxObj* o, const Stack& st) {
      if(o->GetName()==nam) {
        GfxNodeP nn = o->shared_from_this();
        node = dyn_cast<GfxObj>(nn);
      }
    }
    String nam;
    GfxObjP node;
  };

  struct FindNode2: public GfxNodeVisitor {
    FindNode2(GfxNodeP n): node(n), found(false) {}
    virtual bool VisitNode(GfxNode* n, const Stack& st) {
      if(node.get()==n) found=true;
      return true;
    }
    virtual void VisitObject(GfxObj* o, const Stack& st) {
      if(node.get()==o) found=true;
    }
    GfxNodeP node;
    bool found;
  };
  struct NodeCount: public GfxNodeVisitor {
    NodeCount(): count_(0) {}
    virtual bool VisitNode(GfxNode* n, const Stack& st) {
      count_++;
      return true;
    }
    size_t GetCount() { return count_; }
  private:
    size_t count_;
  };
}

size_t Scene::GetNodeCount() const
{
  NodeCount nc;
  this->Apply(nc);
  return nc.GetCount();
}

void Scene::Add(const GfxNodeP& n, bool redraw)
{
  if(!n) return;
  if(!this->IsNameAvailable(n->GetName())){
    throw Error("Scene already has a node with name '"+n->GetName()+"'");
  }

  LOG_DEBUG("scene: graphical object added @" << n.get() << std::endl);
  root_node_->Add(n);
  if (redraw) {
    this->RequestRedraw();
  }
}

bool Scene::IsNameAvailable(String name)
{
  FindNode fn(name);
  Apply(fn);
  if(fn.node) {
    LOGN_MESSAGE(name << " already exists as a scene node");
    return false;
  }
  return true;
}

void Scene::NodeAdded(const GfxNodeP& node)
{
  this->NotifyObservers(bind(&SceneObserver::NodeAdded, _1, node));
}

void Scene::ObjectChanged(const String& name)
{
  FindObj fn(name);
  Apply(fn);
  if(fn.node) {
    this->NotifyObservers(bind(&SceneObserver::NodeChanged, _1, fn.node));
  }
}

void Scene::NodeTransformed(const GfxObjP& object)
{
  this->NotifyObservers(bind(&SceneObserver::NodeTransformed, _1, object));
}

void Scene::SelectionChanged(const String& name, const mol::EntityView& sel)
{
  FindObj fn(name);
  Apply(fn);
  if(fn.node) {
    this->NotifyObservers(bind(&SceneObserver::SelectionChanged, _1, fn.node, sel));
  }
}

void Scene::RenderModeChanged(const String& name)
{
  FindObj fn(name);
  Apply(fn);
  if(fn.node) {
    this->NotifyObservers(bind(&SceneObserver::RenderModeChanged, _1, fn.node));
  }
}

void Scene::Remove(const GfxNodeP& go)
{
  if(!go) return;
  root_node_->Remove(go);
  this->NotifyObservers(bind(&SceneObserver::NodeRemoved, _1,go));
  this->RequestRedraw();
}

void Scene::RemoveAll()
{
  root_node_->RemoveAll();
  this->RequestRedraw();
}

void Scene::Remove(const String& name)
{
  FindNode fn(name);
  Apply(fn);
  if(fn.node) {
    root_node_->Remove(name);
    if(GfxObjP go = dyn_cast<GfxObj>(fn.node)) {
      this->NotifyObservers(bind(&SceneObserver::NodeRemoved, _1,go));
      this->RequestRedraw();
    }
  }
}

bool Scene::Rename(const String& old_name, const String& new_name)
{
  if(this->IsNameAvailable(new_name)){
    FindNode fn(old_name);
    Apply(fn);
    if(fn.node) {
      if(GfxNodeP go = dyn_cast<GfxObj>(fn.node)) {
        go->Rename(new_name);
        return true;
      }
    }
  }
  return false;
}

GfxObjP Scene::operator[](const String& name)
{
  FindNode fn(name);
  Apply(fn);
  if(!fn.node) {
    LOGN_ERROR("error: " << name << " not found");
    return GfxObjP();
  }
  GfxObjP nrvo = dyn_cast<GfxObj>(fn.node);
  if(!nrvo) {
    LOGN_ERROR("error: " << name << " points to invalid entry");
  }
  return nrvo;
}

bool Scene::HasNode(const String& name) const
{
  FindNode fn(name);
  this->Apply(fn);
  return fn.node;
}

void Scene::Apply(const InputEvent& e, bool request_redraw)
{
  LOG_TRACE("Scene: received input: c=" << e.GetCommand() << " i=" << e.GetIndex() << " t=" << e.GetTarget() << std::endl);
  /*
    TODO
    distribute event to transform that
    is meant to receive events from the
    particular device/index combo
    AND
    which is flagged as the edit entry
  */

  if(e.GetTarget()==TRANSFORM_VIEW) {
    // scene
    OnInput(e);
  }
  if(request_redraw) {
    RequestRedraw();
  }
}

void Scene::Apply(GfxNodeVisitor& v) const
{
  v.OnStart();
  GfxNodeVisitor::Stack st;
  // this is the reason for the mutable hack: root_node_ can't be const
  root_node_->Apply(v,st);
  v.OnEnd();
}


void Scene::OnInput(const InputEvent& e)
{
  float sf=1.0;
  if(e.GetCommand()==INPUT_COMMAND_AUTOSLAB) {
    Autoslab();
  } else if(e.GetCommand()==INPUT_COMMAND_TOGGLE_FOG) {
    SetFog(!fog_flag_);
  } else if(e.GetCommand()==INPUT_COMMAND_ROTX) {
    transform_.ApplyXAxisRotation(e.GetDelta());
  } else if(e.GetCommand()==INPUT_COMMAND_ROTY) {
    transform_.ApplyYAxisRotation(e.GetDelta());
  } else if(e.GetCommand()==INPUT_COMMAND_ROTZ) {
    transform_.ApplyZAxisRotation(e.GetDelta());
  } else if(e.GetCommand()==INPUT_COMMAND_TRANSX ||
            e.GetCommand()==INPUT_COMMAND_TRANSY) {

    // adjust translation by current distance of camera from center of rotation
    Vec3 trans = transform_.GetTrans();
    double mm[]={1,0,0,0,
                 0,1,0,0,
                 0,0,1,0,
                 trans[0],trans[1],trans[2],1};
    double pm[16];
    glGetDoublev(GL_PROJECTION_MATRIX,pm);
    GLint vp[4];
    glGetIntegerv(GL_VIEWPORT,vp);
    double wx,wy,wz;
    gluProject(0.0,0.0,0.0,mm,pm,vp,&wx,&wy,&wz);
    double ox,oy,oz;
    gluUnProject(wx+2.0,wy+2.0,wz,mm,pm,vp,&ox,&oy,&oz);
    Vec2 fxy = Vec2(ox,oy);

    if(e.GetCommand()==INPUT_COMMAND_TRANSX) {
      transform_.ApplyXAxisTranslation(e.GetDelta()*fxy[0]);
    } else {
      transform_.ApplyYAxisTranslation(e.GetDelta()*fxy[1]);
    }
  } else if(e.GetCommand()==INPUT_COMMAND_TRANSZ) {
    float delta=e.GetDelta()*sf;
    transform_.ApplyZAxisTranslation(delta);
    SetNearFar(znear_-delta,zfar_-delta);
  } else if(e.GetCommand()==INPUT_COMMAND_SLABN) {
    SetNear(znear_+e.GetDelta());
  } else if(e.GetCommand()==INPUT_COMMAND_SLABF) {
    SetFar(zfar_+e.GetDelta());
  }
}

namespace {

class Selector: public GfxNodeVisitor
{
public:
  void VisitObject(GfxObj* obj, const Stack& st) {
    if(obj->OnSelect(line,picked_pos,zlim,pick_flag)) {
      Vec3 proj_pos=Scene::Instance().Project(picked_pos);
      zlim = proj_pos[2];
    }

  }
  Line3 line;
  bool pick_flag;
  float zlim;
  Vec3 picked_pos;
};

} // anon ns

void Scene::Pick(int mx, int my, int mask)
{
  this->ResetProjection();
  Vec3 v1=UnProject(Vec3(mx,my,0.0));
  Vec3 v2=UnProject(Vec3(mx,my,1.0));

  LOG_DUMP("Scene pick: " << v1 << " " << v2 << " " << mask << std::endl);

  Scene::Instance().StatusMessage("");

  Selector stor;

  stor.line=Line3(v1,v2);
  stor.pick_flag = (mask==8) ? true : false;
  stor.zlim = std::numeric_limits<float>::max();

  Apply(stor);

  RequestRedraw();
}

namespace {


class AtomPicker : public GfxNodeVisitor {
public:
  AtomPicker(const Line3& line):
    zlim_(std::numeric_limits<float>::max()), line_(line), obj_(NULL)
  { }

  void VisitObject(GfxObj* obj, const Stack& st)
  {
    if (Entity* ec=dynamic_cast<Entity*>(obj)) {
      if(mol::AtomHandle pa=ec->PickAtom(line_)) {
        Vec3 proj_pos=Scene::Instance().Project(pa.GetPos());
        if (proj_pos[2]<zlim_) {
          zlim_=proj_pos[2];
          picked_atom_=pa;
          obj_=obj;
        }
      }
    }
  }
  mol::AtomHandle GetPickedAtom() { return picked_atom_; }

  GfxObjP GetObj()
  {
    if (obj_) {
      return dyn_cast<GfxObj>(obj_->shared_from_this());
    } else {
      return GfxObjP();
    }
  }
private:
  float zlim_;
  Line3 line_;
  mol::AtomHandle picked_atom_;
  GfxObj* obj_;
};
}


std::pair<GfxObjP, mol::AtomHandle> Scene::PickAtom(int mx, int my)
{
  Vec3 v1=UnProject(Vec3(mx,my,0.0));
  Vec3 v2=UnProject(Vec3(mx,my,1.0));
  AtomPicker picker(Line3(v1, v2));
  this->Apply(picker);
  return std::make_pair(picker.GetObj(), picker.GetPickedAtom());
}


void Scene::AttachObserver(SceneObserver* o) {
  observers_.push_back(o);
}

void Scene::DetachObserver(SceneObserver* o) {
  SceneObserverList::iterator i;
  i=std::remove(observers_.begin(), observers_.end(), o);
  observers_.erase(i, observers_.end());
}

Vec3 Scene::Project(const Vec3& v, bool ignore_vp) const
{
  GLdouble gl_mmat[16];
  glGetDoublev(GL_MODELVIEW_MATRIX,gl_mmat);
  GLdouble gl_pmat[16];
  glGetDoublev(GL_PROJECTION_MATRIX,gl_pmat);
  GLint gl_vp[]={0,0,1,1};
  if(!ignore_vp) {
    glGetIntegerv(GL_VIEWPORT,gl_vp);
  }

  double res[3];
  gluProject(v[0],v[1],v[2],
       gl_mmat,gl_pmat,gl_vp,
       &res[0],&res[1],&res[2]);
  return geom::Vec3(float(res[0]),
                    float(res[1]),
                    float(res[2]));
}

Vec3 Scene::UnProject(const Vec3& v, bool ignore_vp) const
{
  GLdouble gl_mmat[16];
  glGetDoublev(GL_MODELVIEW_MATRIX,gl_mmat);
  GLdouble gl_pmat[16];
  glGetDoublev(GL_PROJECTION_MATRIX,gl_pmat);
  GLint gl_vp[]={0,0,1,1};
  if(!ignore_vp) {
    glGetIntegerv(GL_VIEWPORT,gl_vp);
  }

  double res[3];
  gluUnProject(v[0],v[1],v[2],
         gl_mmat,gl_pmat,gl_vp,
         &res[0],&res[1],&res[2]);
  return geom::Vec3(float(res[0]),
                    float(res[1]),
                    float(res[2]));
}

namespace {

class BBCalc: public GfxNodeVisitor
{
public:
  bool VisitNode(GfxNode* node, const Stack& st) {
    return node->IsVisible(); // only descend into visible nodes
  }
  void VisitObject(GfxObj* obj, const Stack& st) {
    if(obj->IsVisible()) {
      geom::AlignedCuboid bb=obj->GetBoundingBox();
      Vec3 t1 = tf.Apply(Vec3(bb.GetMin()[0],bb.GetMin()[1],bb.GetMin()[2]));
      Vec3 t2 = tf.Apply(Vec3(bb.GetMin()[0],bb.GetMax()[1],bb.GetMin()[2]));
      Vec3 t3 = tf.Apply(Vec3(bb.GetMax()[0],bb.GetMax()[1],bb.GetMin()[2]));
      Vec3 t4 = tf.Apply(Vec3(bb.GetMax()[0],bb.GetMin()[1],bb.GetMin()[2]));
      Vec3 t5 = tf.Apply(Vec3(bb.GetMin()[0],bb.GetMin()[1],bb.GetMax()[2]));
      Vec3 t6 = tf.Apply(Vec3(bb.GetMin()[0],bb.GetMax()[1],bb.GetMax()[2]));
      Vec3 t7 = tf.Apply(Vec3(bb.GetMax()[0],bb.GetMax()[1],bb.GetMax()[2]));
      Vec3 t8 = tf.Apply(Vec3(bb.GetMax()[0],bb.GetMin()[1],bb.GetMax()[2]));
      minc = Min(minc,Min(t1,Min(t2,Min(t3,Min(t4,Min(t5,Min(t6,Min(t7,t8))))))));
      maxc = Max(maxc,Max(t1,Max(t2,Max(t3,Max(t4,Max(t5,Max(t6,Max(t7,t8))))))));
    }
  }

  Vec3 minc,maxc;
  mol::Transform tf;
};

}

geom::AlignedCuboid Scene::GetBoundingBox(const mol::Transform& tf) const
{
  BBCalc bbcalc;

  bbcalc.tf = tf;
  bbcalc.minc = Vec3(std::numeric_limits<float>::max(),
                           std::numeric_limits<float>::max(),
                           std::numeric_limits<float>::max());
  bbcalc.maxc = Vec3(-std::numeric_limits<float>::max(),
                           -std::numeric_limits<float>::max(),
                           -std::numeric_limits<float>::max());

  Apply(bbcalc);

  return geom::AlignedCuboid(bbcalc.minc,bbcalc.maxc);
}

mol::Transform Scene::GetTransform() const
{
  return transform_;
}

void Scene::SetTransform(const mol::Transform& t)
{
  transform_=t;
  this->RequestRedraw();
}

Mat4 Scene::GetRTC() const
{
  Mat4 nrvo(transform_.GetRot());
  for(int i=0;i<3;++i) {
    nrvo(3,i) = transform_.GetTrans()[i];
    nrvo(i,3) = transform_.GetCenter()[i];
  }
  return nrvo;
}

void Scene::SetRTC(const Mat4& rtc)
{
  transform_.SetRot(Mat3(rtc(0,0), rtc(0,1), rtc(0,2),
                               rtc(1,0), rtc(1,1), rtc(1,2),
                               rtc(2,0), rtc(2,1), rtc(2,2)));
  transform_.SetTrans(Vec3(rtc(3,0),rtc(3,1),rtc(3,2)));
  transform_.SetCenter(Vec3(rtc(0,3),rtc(1,3),rtc(2,3)));
  RequestRedraw();
}

void Scene::PushView()
{
  SceneViewStackEntry se = {transform_,fov_,znear_,zfar_};
  scene_view_stack_.push(se);
}

void Scene::PopView()
{
  if(!scene_view_stack_.empty()) {
    transform_=scene_view_stack_.top().transform;
    fov_=scene_view_stack_.top().fov;
    znear_=scene_view_stack_.top().znear;
    zfar_=scene_view_stack_.top().zfar;
    scene_view_stack_.pop();
    ResetProjection();
    update_fog();
    RequestRedraw();
  }
}

float Scene::GetNear() const
{
  return znear_;
}

void Scene::SetNear(float n)
{
  set_near(n);
  ResetProjection();
  RequestRedraw();
}

float Scene::GetFar() const
{
  return zfar_;
}

void Scene::SetFar(float f)
{
  set_far(f);
  ResetProjection();
  RequestRedraw();
}

void Scene::SetNearFar(float n, float f)
{
  // this apparent setting duplication is necessary
  // due to the limit check in set_*
  znear_=n;
  zfar_=f;
  set_near(n);
  set_far(f);
  ResetProjection();
  RequestRedraw();
}

void Scene::SetFOV(float f)
{
  fov_=std::max(1.0f,f);
  ResetProjection();
  RequestRedraw();
}

float Scene::GetFOV() const
{
  return fov_;
}

void Scene::SetFogNearOffset(float o)
{
  fnear_=o;
  update_fog();
  RequestRedraw();
}

float Scene::GetFogNearOffset() const
{
  return fnear_;
}

void Scene::SetFogFarOffset(float o)
{
  ffar_=o;
  update_fog();
  RequestRedraw();
}

float Scene::GetFogFarOffset() const
{
  return ffar_;
}

void Scene::SetFogOffsets(float no, float fo)
{
  fnear_=no;
  ffar_=fo;
  update_fog();
  RequestRedraw();
}

void Scene::Stereo(unsigned int m)
{
  if(m==1) {
    stereo_=m;
    if(win_) win_->SetStereo(true);
  } else if(m==2 || m==3) {
    stereo_=m;
    if(win_) win_->SetStereo(false);
  } else {
    stereo_=0;
    if(win_) win_->SetStereo(false);
  }
  RequestRedraw();
}

void Scene::SetStereoInverted(bool f)
{
  stereo_inverted_=f;
}

void Scene::SetStereoEye(unsigned int m)
{
  stereo_eye_= (m>2) ? 0: m;
  ResetProjection();
  RequestRedraw();
}


void Scene::SetLightDir(const Vec3& dir)
{
  if(Length2(dir)>0.0) {
    light_dir_=Normalize(dir);
    light_rot_=calc_light_rot(light_dir_);
    set_light_dir(light_dir_);
    RequestRedraw();
  }
}

void Scene::SetLightProp(const Color& amb, const Color& diff, 
                         const Color& spec)
{
  light_amb_=amb;
  light_diff_=diff;
  light_spec_=spec;
  glLightfv(GL_LIGHT0, GL_AMBIENT, light_amb_);
  glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diff_);
  glLightfv(GL_LIGHT0, GL_SPECULAR, light_spec_);
  RequestRedraw();
}

void Scene::SetLightProp(float amb, float diff, float spec)
{
  light_amb_=Color(amb,amb,amb,1.0);
  light_diff_=Color(diff,diff,diff,1.0);
  light_spec_=Color(spec,spec,spec,1.0);
  glLightfv(GL_LIGHT0, GL_AMBIENT, light_amb_);
  glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diff_);
  glLightfv(GL_LIGHT0, GL_SPECULAR, light_spec_);
  RequestRedraw();
}

void Scene::SetSelectionMode(uint m)
{
  if (selection_mode_!=m) {
    selection_mode_=m;
    this->NotifyObservers(bind(&SceneObserver::SelectionModeChanged, _1,
                               selection_mode_));
  }

}

uint Scene::GetSelectionMode() const
{
  return selection_mode_;
}

namespace {

struct OffscreenSwitcher
{
  OffscreenSwitcher()
  {
    LOGN_TRACE("offscreen begin");
    OffscreenBuffer::Instance().Begin();
  }

  ~OffscreenSwitcher()
  {
    LOGN_TRACE("offscreen end");
    OffscreenBuffer::Instance().End();
  }
};

} // anon ns

void Scene::Export(const String& fname, unsigned int width,
                   unsigned int height, bool transparent)
{
  int d_index=fname.rfind('.');
  if (d_index==-1) {
    LOGN_ERROR("no file extension specified");
    return;
  }
  String ext = fname.substr(d_index);
  if(!(ext==".png")) {
    LOGN_ERROR("unknown file format (" << ext << ")");
    return;
  }

  offscreen_flag_=true;
  LOGN_TRACE("offscreen resize");
  OffscreenBuffer::Instance().Resize(width, height);
  try {
    // ensures that context is switched back when this goes out of scope
    OffscreenSwitcher offscreen_switch;

#if OST_SHADER_SUPPORT_ENABLED
    String shader_name = Shader::Instance().GetCurrentName();
#endif
    LOGN_TRACE("initializing GL");
    this->InitGL();
    LOGN_TRACE("setting viewport");
    SetViewport(width,height);
    LOGN_TRACE("reseting projection");
    ResetProjection();
    LOGN_TRACE("updating fog settings");
    update_fog();
    glDrawBuffer(GL_FRONT);
    //this->flag_all_dirty();
#if OST_SHADER_SUPPORT_ENABLED
    LOGN_TRACE("activating shader");
    Shader::Instance().Activate(shader_name);
#endif
    LOGN_TRACE("doing rendering");
    this->RenderGL();
    // make sure drawing operations are finished
    glFinish();
    boost::shared_array<uchar> img_data(new uchar[width*height*4]);

    LOGN_TRACE("setting background transparency");
    if (transparent) {
      glPixelTransferf(GL_ALPHA_BIAS, 0.0);
    } else {
      // shift alpha channel by one to make sure pixels are read out as opaque
      glPixelTransferf(GL_ALPHA_BIAS, 1.0);
    }

    LOGN_TRACE("reading framebuffer pixels");
    glReadBuffer(GL_FRONT);
    glReadPixels(0,0,width,height,GL_RGBA,GL_UNSIGNED_BYTE,img_data.get());

    LOGN_DEBUG("calling bitmap export");
    BitmapExport(fname,ext,width,height,img_data.get());
  } catch (...) {
    // noop
  } // offscreen_switch goes out of scope
  offscreen_flag_=false;
  glDrawBuffer(GL_BACK);
  LOGN_TRACE("updating fog");
  update_fog();
}

void Scene::Export(const String& fname, bool transparent)
{
  int d_index=fname.rfind('.');
  if (d_index==-1) {
    LOGN_ERROR("no file extension specified");
    return;
  }
  String ext = fname.substr(d_index);
  if(ext!=".png") {
    LOGN_ERROR("unknown file format (" << ext << ")");
    return;
  }
  GLint vp[4];
  glGetIntegerv(GL_VIEWPORT,vp);

  if (transparent) {
    glPixelTransferf(GL_ALPHA_BIAS, 0.0);
  } else {
    // shift alpha channel by one to make sure pixels are read out as opaque    
    glPixelTransferf(GL_ALPHA_BIAS, 1.0);    
  }
  boost::shared_array<uchar> img_data(new uchar[vp[2]*vp[3]*4]);
  glReadPixels(0,0,vp[2],vp[3],GL_RGBA,GL_UNSIGNED_BYTE,img_data.get());
  glFinish();
  BitmapExport(fname,ext,vp[2],vp[3],img_data.get());
  glPixelTransferf(GL_ALPHA_BIAS, 0.0);  
}

void Scene::ExportPov(const std::string& fname, const std::string& wdir)
{
  std::string wdir2=wdir;
  if(wdir2=="") wdir2=".";
  PovState pov(fname+".pov",fname+".inc",wdir2+"/");
  pov.tf=transform_;
  pov.use_tf=false;

  pov.write_preamble();
  pov.write_background(background_);
  pov.write_camera(fov_,-transform_.GetTrans()[2]*aspect_ratio_*1.3);
  pov.write_default_light();
  pov.write_fog(-transform_.GetTrans()[2],znear_+fnear_,zfar_+ffar_,fog_color_,fog_flag_);

  // descend into scene graph
  root_node_->RenderPov(pov);
  pov.write_postamble();
}

void Scene::ResetProjection()
{
  LOGN_TRACE("scene: projection matrix " << fov_ << " " << znear_ << " " << zfar_);
  stereo_projection(stereo_eye_);
}

void Scene::SetBlur(uint n)
{
  blur_count_=std::min(n,3u);
  glClearAccum(0.0,0.0,0.0,0.0);
  glClear(GL_ACCUM_BUFFER_BIT);
  RequestRedraw();
}

void Scene::BlurSnapshot()
{
  if(blur_count_==0) return;
  glFinish();
  glAccum(GL_MULT, 0.5);
  glAccum(GL_ACCUM, 0.5);
  RequestRedraw();
}

void Scene::AutoAutoslab(bool f)
{
  auto_autoslab_=f;
  RequestRedraw();
}

namespace {

class LimCalc: public GfxNodeVisitor
{
public:
  void VisitObject(GfxObj* obj, const Stack& st) {
    if(obj->IsVisible()) {
      obj->ProcessLimits(minc,maxc, transform);
    }
  }
  Vec3 minc,maxc;
  mol::Transform transform;
};

} // anon ns

void Scene::Autoslab(bool fast, bool update)
{
  if(fast) {
    geom::AlignedCuboid bb =this->GetBoundingBox(transform_);
    // necessary code duplication due to awkward slab limit impl
    znear_=-(bb.GetMax()[2]-1.0);
    zfar_=-(bb.GetMin()[2]+1.0);
    set_near(-(bb.GetMax()[2]-1.0));
    set_far(-(bb.GetMin()[2]+1.0));
    ResetProjection();
  } else {
    LimCalc limcalc;
    limcalc.transform=transform_;
    limcalc.minc = Vec3(std::numeric_limits<float>::max(),
                              std::numeric_limits<float>::max(),
                              std::numeric_limits<float>::max());
    limcalc.maxc = Vec3(-std::numeric_limits<float>::max(),
                              -std::numeric_limits<float>::max(),
                              -std::numeric_limits<float>::max());
    this->Apply(limcalc);
    float mynear=std::max(float(0.0), std::min(float(-limcalc.minc[2]),float(-limcalc.maxc[2])))-float(2.0);
    float myfar=std::max(float(-limcalc.minc[2]),float(-limcalc.maxc[2]))+float(2.0);
    znear_=mynear;
    zfar_=myfar;
    set_near(znear_);
    set_far(zfar_);
    ResetProjection();
  }
  if (update) {
    this->RequestRedraw();
  }
}

void Scene::AutoslabMax()
{
  geom::AlignedCuboid bb =this->GetBoundingBox(transform_);

  Vec3 cen = transform_.Apply(transform_.GetCenter());

  float bmax = std::max(std::abs(cen[0]-bb.GetMin()[0]),
                        std::abs(cen[0]-bb.GetMax()[0]));
  bmax = std::max(bmax,float(std::abs(cen[1]-bb.GetMin()[1])));
  bmax = std::max(bmax,float(std::abs(cen[1]-bb.GetMax()[1])));
  bmax = std::max(bmax,float(std::abs(cen[2]-bb.GetMin()[2])));
  bmax = std::max(bmax,float(std::abs(cen[2]-bb.GetMax()[2])));

  float nnear = -(cen[2]+bmax*1.5);
  float nfar = -(cen[2]-bmax*1.5);

  // necessary code duplication due to awkward slab limit impl
  znear_=nnear;
  zfar_=nfar;
  set_near(nnear);
  set_far(nfar);
  ResetProjection();
}


void Scene::set_near(float n)
{
  znear_=n;
  if(znear_>zfar_-0.1) {
    znear_=zfar_-0.1;
  }
  update_fog();
}

void Scene::set_far(float f)
{
  zfar_=f;
  if(znear_>zfar_-0.1) {
    zfar_=znear_+0.1;
  }
  update_fog();
}

void Scene::update_fog()
{
  glFogf(GL_FOG_START,znear_+fnear_);
  glFogf(GL_FOG_END,zfar_+ffar_);
}

void Scene::prep_shadow_map()
{
#if OST_SHADER_SUPPORT_ENABLED
  GLint smap_size=256 << shadow_quality_;

  // modelview transform for the lightsource pov
  mol::Transform ltrans(transform_);
  ltrans.SetRot(light_rot_*transform_.GetRot());

  // calculate encompassing box for ortho projection
  geom::AlignedCuboid bb=this->GetBoundingBox(ltrans);
  const Vec3& tmin=bb.GetMin();
  const Vec3& tmax=bb.GetMax();

  // render pass 1 - without shadows

  // turn shadowing off for subsequent rendering
  glUniform1i(glGetUniformLocation(Shader::Instance().GetCurrentProgram(),
              "shadow_flag"),0);
  // save overall gl settings
  glPushAttrib(GL_VIEWPORT_BIT | GL_ENABLE_BIT);
  // maximize rendering for depth-only information
#ifndef DEBUG_SMAP
  glDisable(GL_LIGHTING);
  glDisable(GL_FOG);
  glDisable(GL_COLOR_MATERIAL);
  glDisable(GL_NORMALIZE);
  glColorMask(GL_FALSE,GL_FALSE,GL_FALSE,GL_FALSE);
#endif

  // render scene with only depth components
  glViewport(0,0,smap_size,smap_size);
  glClear(GL_DEPTH_BUFFER_BIT);

  glMatrixMode(GL_PROJECTION);
  glPushMatrix();
  glLoadIdentity();
  glOrtho(tmin[0],tmax[0],tmin[1],tmax[1],-tmax[2],-tmin[2]);
  //glFrustum(tmin[0],tmax[0],tmin[1],tmax[1],-tmax[2],-tmin[2]);
  float glpmat[16];
  glGetv(GL_PROJECTION_MATRIX, glpmat);
  Mat4 pmat(Transpose(Mat4(glpmat)));

  glMatrixMode(GL_MODELVIEW);
  glPushMatrix();
  glLoadIdentity();
  glMultMatrix(ltrans.GetTransposedMatrix().Data());

  // only render non-transparent objects for the shadow map
  root_node_->RenderGL(OPAQUE_RENDER_PASS);

  // now get the shadow map
  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, texture_id_);

  glCopyTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT,
                   0,0, smap_size,smap_size, 0);

  ////////////////
  // all of the following texture and shader params need to be moved
  // to a one-time initialization place
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);

  GLuint cpr=Shader::Instance().GetCurrentProgram();

  // assign tex unit 0 to shadow map
  glUniform1i(glGetUniformLocation(Shader::Instance().GetCurrentProgram(),"shadow_map"),0);
  //int depth_bits;
  //glGetIntegerv(GL_DEPTH_BITS, &depth_bits);
  //float depth_bias = 1.0/static_cast<float>(1<<depth_bits);
  glUniform1f(glGetUniformLocation(cpr,"depth_bias"),0.008);

  glUniform1f(glGetUniformLocation(cpr,"epsilon"),0.002);

  glUniform1f(glGetUniformLocation(cpr,"shadow_multiplier"),0.4);

  //
  //////////////////

  // restore settings
  glPopMatrix();
  glMatrixMode(GL_PROJECTION);
  glPopMatrix();
  glMatrixMode(GL_MODELVIEW);

  glPopAttrib();
  glColorMask(GL_TRUE,GL_TRUE,GL_TRUE,GL_TRUE);

  glEnable(GL_TEXTURE_2D);

  // and turn shadowing on for subsequent rendering
  glUniform1i(glGetUniformLocation(cpr,"shadow_flag"),1);

#ifndef DEBUG_SMAP
  // set up appropriate texture matrix
  Mat4 bias(0.5,0.0,0.0,0.5,
                  0.0,0.5,0.0,0.5,
                  0.0,0.0,0.5,0.5,
                  0.0,0.0,0.0,1.0);

  Mat4 texm = bias*pmat*ltrans.GetMatrix();

  glMatrixMode(GL_TEXTURE);
  // make explicit object instead of temporary to avoid potential crash with Data()
  Mat4 ttmp=Transpose(texm);
  glLoadMatrix(ttmp.Data());
  glMatrixMode(GL_MODELVIEW);
#else
  // this debug code draws the depth map across the screen
  Shader::Instance().Activate("");
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  glDisable(GL_CULL_FACE);
  glDisable(GL_LIGHTING);
  glDisable(GL_FOG);
  glEnable(GL_TEXTURE_2D);
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  glOrtho(-1,11,-1,11,-1,1);
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
  glColor3f(1.0,1.0,1.0);
  glBegin(GL_QUADS);
  glTexCoord2f(0.0,0.0);
  glVertex2f(0.0,0.0);
  glTexCoord2f(0.0,1.0);
  glVertex2f(0.0,10.0);
  glTexCoord2f(1.0,1.0);
  glVertex2f(10.0,10.0);
  glTexCoord2f(1.0,0.0);
  glVertex2f(10.0,0.0);
  glEnd();
  Shader::Instance().Activate("basic_shadow");
#endif
#endif
}

namespace {
class DirtyAll: public GfxNodeVisitor
{
public:
  void VisitObject(GfxObj* obj, const Stack& st) {obj->FlagRebuild();}
};

} // anon ns

void Scene::flag_all_dirty()
{
  static DirtyAll da;
  Apply(da);
}

GfxNodeP Scene::GetRootNode() const
{
  return root_node_;
}

bool Scene::InOffscreenMode() const
{
  return offscreen_flag_;
}

void Scene::SetOffscreenMode()
{
  OffscreenBuffer::Instance().Begin();
}

float Scene::ElapsedTime() const
{
#ifndef _MSC_VER
  static timeval tm;
  gettimeofday(&tm,NULL);
  return static_cast<float>(tm.tv_sec)+static_cast<float>(tm.tv_usec)*1.0e-6;
#else
  return 0.0;
#endif
}

void Scene::SetTestMode(bool f)
{
  test_flag_=f;
  if(f) {
    Add(GfxObjP(new GfxTestObj()));
    Autoslab();
  } else {
    Remove("GfxTestObj");
  }
}

// temporary interface
void Scene::ActivateShader(const String& name)
{
#if OST_SHADER_SUPPORT_ENABLED
  Shader::Instance().Activate(name);
#endif
}

void Scene::prep_glyphs()
{
  glGenTextures(1,&glyph_tex_id_);
  String ost_root =GetSharedDataPath();
  bf::path ost_root_dir(ost_root);
  bf::path tex_file(ost_root_dir / "textures/glyph_texture.png");
  Bitmap bm = BitmapImport(tex_file.string(),".png");
  if(!bm.data) return;

  LOGN_DEBUG("importing glyph tex with id " << glyph_tex_id_);
  glBindTexture(GL_TEXTURE_2D, glyph_tex_id_);
  glPixelStorei(GL_UNPACK_ALIGNMENT,1);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  if(bm.channels==1) {
    glTexImage2D(GL_TEXTURE_2D,0,GL_INTENSITY,bm.width,bm.height,0,GL_LUMINANCE,GL_UNSIGNED_BYTE,bm.data.get());
  } else if(bm.channels==3) {
    glTexImage2D(GL_TEXTURE_2D,0,GL_INTENSITY,bm.width,bm.height,0,GL_RGB,GL_UNSIGNED_BYTE,bm.data.get());
  } else if(bm.channels==4) {
    glTexImage2D(GL_TEXTURE_2D,0,GL_INTENSITY,bm.width,bm.height,0,GL_RGBA,GL_UNSIGNED_BYTE,bm.data.get());
  } else {
    LOGN_ERROR("unsupported glyph texture channel count of " << bm.channels);
    return;
  }
  float ir = 1.0/8.0;
  float ic = 1.0/16.0;
  glyph_map_.resize(256);
  for(int cc=0;cc<17;++cc) glyph_map_[cc]=Vec2(0.0,0.0);
  for(int r=0;r<7;++r) {
    for(int c=0;c<16;++c) {
      glyph_map_[r*16+c+17]=Vec2(ic*static_cast<float>(c), 1.0-ir*static_cast<float>(r)-ir);
    }
  }
  for(int cc=128;cc<256;++cc) glyph_map_[cc]=Vec2(0.0,0.0);

  LOGN_VERBOSE("done loading glyphs");

}

void Scene::prep_blur()
{
  if(blur_count_==0) return;
  glAccum(GL_RETURN, 0.5);
  glFlush();
}

void Scene::render_scene_with_glow()
{
  glDepthFunc(GL_LEQUAL);    
  glDepthMask(1);
  glEnable(GL_DEPTH_TEST);
  root_node_->RenderGL(STANDARD_RENDER_PASS);
  glPushAttrib(GL_ENABLE_BIT);
  glEnable(GL_COLOR_MATERIAL);
  glShadeModel(GL_FLAT);
  glDisable(GL_FOG);
  glDisable(GL_LIGHTING);
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

#if OST_SHADER_SUPPORT_ENABLED
  Shader::Instance().PushProgram();
  Shader::Instance().Activate("");
#endif  
  float eps=0.005;
  glDepthRange(-eps, 1.0-eps);
  root_node_->RenderGL(GLOW_RENDER_PASS);
  glDepthRange(0.0, 1.0);
  glPopAttrib();
  glShadeModel(GL_SMOOTH);
#if OST_SHADER_SUPPORT_ENABLED
  Shader::Instance().PopProgram();
#endif  
}

void Scene::stereo_projection(unsigned int view)
{
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  
  float zn=std::max(float(1.0),znear_);
  float zf=std::max(float(1.1),zfar_);

  gluPerspective(fov_,aspect_ratio_,zn,zf);

  if(view>0) {
    float iod = (view==1) ? -stereo_eye_dist_ : stereo_eye_dist_;
    //float fr=(-stereo_eye_offset_-zn)/(zf-zn);
    float angle=180.0f/M_PI*std::atan(stereo_eye_offset_/(2.0*iod));
    glTranslated(0.0,0.0,transform_.GetTrans()[2]);
    glRotated(-angle,0.0,1.0,0.0);
    glTranslated(0.0,0.0,-transform_.GetTrans()[2]);
  }
}

void Scene::render_interlaced_stereo()
{
  // set up stencil buffer
  glPushAttrib(GL_STENCIL_BUFFER_BIT| GL_COLOR_BUFFER_BIT | GL_ENABLE_BIT);
  glMatrixMode(GL_PROJECTION);
  glPushMatrix();
  glLoadIdentity();
  glOrtho(0,vp_width_,0,vp_height_,-1,1);
  glMatrixMode(GL_MODELVIEW);
  glPushMatrix();
  glLoadIdentity();
  glDrawBuffer(GL_NONE);
  glDisable(GL_DEPTH_TEST);
  glDisable(GL_LIGHTING);
  glDisable(GL_COLOR_MATERIAL);
  glDisable(GL_FOG);
  glDisable(GL_CULL_FACE);
  glDisable(GL_BLEND);
  glDisable(GL_LINE_SMOOTH);
  glDisable(GL_POINT_SMOOTH);
  glLineWidth(1.0);
  glEnable(GL_STENCIL_TEST);
  glStencilMask(0x1);
  glClearStencil(0x0);
  glClear(GL_STENCIL_BUFFER_BIT);
  glStencilFunc(GL_ALWAYS,0x1,0x1);
  glStencilOp(GL_REPLACE,GL_REPLACE,GL_REPLACE);
  glBegin(GL_LINES);
  glColor3f(1.0,1.0,1.0);
  for(unsigned int i=0;i<vp_height_;i+=2) {
    glVertex2i(0,i);
    glVertex2i(vp_width_-1,i);
  } 
  glEnd();
  glPopMatrix();
  glMatrixMode(GL_PROJECTION);
  glPopMatrix();
  glMatrixMode(GL_MODELVIEW);
  glPopAttrib();

  glEnable(GL_STENCIL_TEST);
  glStencilOp(GL_KEEP,GL_KEEP,GL_KEEP);

  glStencilFunc(GL_EQUAL,0x1,0x1);
  stereo_projection(stereo_==2 ? 1 : 2);
  root_node_->RenderGL(STANDARD_RENDER_PASS);
  root_node_->RenderGL(GLOW_RENDER_PASS);

  glStencilFunc(GL_EQUAL,0x0,0x1);
  stereo_projection(stereo_==2 ? 2 : 1);
  root_node_->RenderGL(STANDARD_RENDER_PASS);
  root_node_->RenderGL(GLOW_RENDER_PASS);
  glDisable(GL_STENCIL_TEST);
  glPopAttrib();
}

void Scene::render_quad_buffered_stereo()
{
  glDrawBuffer(GL_BACK);
  glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
  glDrawBuffer(GL_BACK_LEFT);
  glClear(GL_DEPTH_BUFFER_BIT);
  if (stereo_inverted_) {
    stereo_projection(stereo_==1 ? 2 : 1);
  } else {
    stereo_projection(stereo_==1 ? 1 : 2);
  }
  this->render_scene_with_glow();
  glDrawBuffer(GL_BACK_RIGHT);
  glClear(GL_DEPTH_BUFFER_BIT);
  if (stereo_inverted_) {
    stereo_projection(stereo_==1 ? 1 : 2);
  } else {
    stereo_projection(stereo_==1 ? 2 : 1);
  }
  this->render_scene_with_glow();
  glDrawBuffer(GL_BACK);
}

}} // ns
