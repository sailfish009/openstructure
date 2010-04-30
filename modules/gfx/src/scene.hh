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
#ifndef OST_SCENE_HH
#define OST_SCENE_HH

/*
  Author: Ansgar Philippsen
*/

#include <map>
#include <stack>
#include <vector>

#include <boost/shared_array.hpp>

#include <ost/gfx/module_config.hh>
#include <ost/mol/transform.hh>

#include "color.hh"
#include "gfx_object_fw.hh"
#include "gfx_node_fw.hh"
#include "gfx_node_visitor.hh"
#include "selection.hh"
#include "glwin_base.hh"
#include "scene_observer.hh"
#include "gfx_prim.hh"
#include "gl_include.hh"
#include "povray_fw.hh"

namespace ost { namespace gfx {

class InputEvent;
class OffscreenBuffer;

typedef std::vector<SceneObserver*>  SceneObserverList;

struct Viewport {
  int bottom;
  int left;
  int width;
  int height;
};

/// \brief main class for organization and root for the graphical display
/// 
/// The scene manages graphical objects for rendering. Typical graphical objects 
/// include \ref gfx::Entity "entities", \ref gfx::MapIso "isocontoured maps", 
/// \ref gfx::MapSlab "density slabs", \ref gfx::Surface "molecular surfaces", 
/// or \ref gfx::Primitive "primitives" such as \ref gfx::Cuboid "cuboids", 
/// \ref gfx::Quad "quads" and \ref gfx::PrimList "lines". The nodes are 
/// organized in a tree-like structure whose root 
/// can obtained with GetRootNode().
/// 
/// The center of the eye is controlled with SetCenter() and CenterOn().
/// 
/// By default, the near and far clipping planes are adjusted such that they 
/// contain all visible objects in the scene. This behaviour can be turned of by
/// disabling the AutoAutoslab(). The near and far clipping plane can then be 
/// adjusted manually.
class DLLEXPORT_OST_GFX Scene {
 private:
   
  // TODO: this struct may be the seed for a proper
  // refactoring of the scene it into a management
  // and a view part
  struct SceneViewStackEntry {
    mol::Transform transform;
    float fov;
    float znear,zfar;
  };

  typedef std::stack<SceneViewStackEntry> SceneViewStack;

 public:
  static Scene& Instance();

  /// \brief turn fog on or off
  void SetFog(bool f);

  /// \brief check fog status
  bool GetFog() const;

  /// \brief set the fog color
  void SetFogColor(const Color& c);

  /// \brief get the fog color
  Color GetFogColor() const;

  /// \brief turn shadow on and off
  void SetShadow(bool f);

  /// \brief returns true if shadow is on
  bool GetShadow();

  /// \brief shadow quality from 0 (low) to 3 (high), default=1
  void SetShadowQuality(int q);
  
  /// \name clipping planes
  //@{
  /// \brief get near clipping plane
  float GetNear() const;
  
  /// \brief set near clipping plane
  void SetNear(float n);
  
  /// \brief get far clipping plane  
  float GetFar() const;
  
  /// \brief set far clipping plane  
  void SetFar(float f);
  
  /// \brief set near and far clipping plane at once
  void SetNearFar(float n, float f);

  /// \brief set field of view angle
  void SetFOV(float f);

  // \brief get the field of view
  float GetFOV() const;

  /// \brief offset between near clipping plane and start of fog
  void SetFogNearOffset(float o);

  /// \sa SetFogNearOffset
  float GetFogNearOffset() const;

  /// \brief offset between far clipping plane and end of fog
  void SetFogFarOffset(float o);

  /// \sa SetFogFarOffset
  float GetFogFarOffset() const;

  /// \brief convenciene function to set fog near and far offset
  void SetFogOffsets(float no, float fo);
  
  /// \brief adjust near and far clipping plane to fit visible objects
  void Autoslab(bool fast=false, bool redraw=true);
  // \brief adjust clipping planes to fix maximal extent of all objects
  //        even under rotation
  void AutoslabMax();

  /// \brief turn on automatic auto-slabbing (using the fast bounding box alg)
  void AutoAutoslab(bool f);
  //@}
  
  /// \brief switch stereo mode
  /*
    0=off
    1=quad-buffered
    2=interlaced variant #1
    3=interlaced variant #2
  */
  void Stereo(unsigned int);

  /// \brief invert stereo eyes for stereo mode=0
  void SetStereoInverted(bool f);

  /// \brief eye mode, 0=center, 1=left, 2=right
  void SetStereoEye(unsigned int);

  /// \brief set main light direction
  void SetLightDir(const geom::Vec3& dir);
  /// \brief set ambient, diffuse and specular light color
  void SetLightProp(const Color& amb, const Color& diff, const Color& spec);
  /// \brief set ambient, diffuse and specular light intensity
  void SetLightProp(float amb, float diff, float spec);

  /// \brief set the selection mode
  /*
    bad style for now: 0=(reserved), 1=atom, 2=residue, 3=chain, 4=bond, 5=torsion
  */
  void SetSelectionMode(uint m);
  uint GetSelectionMode() const;

  /// \name Export
  //@}
  /// \brief export scene into a bitmap, rendering into offscreen of given size
  /// if a main offscreen buffer is active (\sa StartOffscreenMode), then the
  /// dimensions here are ignored
  void Export(const String& fname, unsigned int w,
              unsigned int h, bool transparent=true);

  /// \brief export snapshot of current scene
  void Export(const String& fname, bool transparent=true);

  /// \brief export scene into povray files named fname.pov and fname.inc
  void ExportPov(const std::string& fname, const std::string& wdir=".");
  //@}
  /// \brief entry point for gui events (internal use)
  void OnInput(const InputEvent& e);
  
  /// \brief initialize OpenGL after context has been setup (internal use)
  void InitGL();

  /// \brief handle new viewport size (internal use)
  void Resize(int w, int h);

  /// \brief pick at given mouse coords
  void Pick(int mx, int my, int mask);
  
  float GetDefaultTextSize();

  /// \brief pick atom at given mouse coord
  std::pair<GfxObjP, mol::AtomHandle> PickAtom(int mx, int my);
  
  /// \brief render all gl objects (internal use)
  void RenderGL();

  /// \brief request redraw of gl scene
  void RequestRedraw();
  
  /// \brief send status message to gui
  void StatusMessage(const String& s);

  /// \brief set the viewport; the mapping to the visible window (internal use)
  void SetViewport(int w, int h);

  /// \brief set background color
  void SetBackground(const Color& c);

  /// \brief get background color
  Color GetBackground() const;

  /// \brief center rotation on the given point
  void SetCenter(const geom::Vec3& cen);
  
  /// \brief retrieve center
  geom::Vec3 GetCenter() const;

  /// \brief center on object of given name
  void CenterOn(const String& s);

  /// \brief center given object
  void CenterOn(const GfxObjP& s);

  /// \brief calculate projection of a point into the scene
  geom::Vec3 Project(const geom::Vec3& v, bool ignore_vp=false) const;
  
  /// \brief calculate unprojected point out of the scene
  geom::Vec3 UnProject(const geom::Vec3& v, bool ignore_vp=false) const;

  /// \brief return bounding box of scene under given transform
  geom::AlignedCuboid GetBoundingBox(const mol::Transform& tf) const;

  /// \brief get full underlying transformation
  mol::Transform GetTransform() const;

  /// \brief set transform
  void SetTransform(const mol::Transform& t);

  /// \brief returns a compact, internal representation of the scene orientation
  geom::Mat4 GetRTC() const;

  /// \brief sets a previously retrieved orientation
  void SetRTC(const geom::Mat4& rtc);
  
  /// \brief push the current orientation onto a stack
  void PushView();

  /// \brief retrieve a previously pushed orientation
  void PopView();

  /// brief re-generates the projection matrix (internal use)
  void ResetProjection();

  /// \brief gui glue interface (internal use)
  void Register(GLWinBase* win);
  /// \brief gui glue interface (internal use)
  void Unregister(GLWinBase* win);
  
  /// \name scene graph
  //@{
  /// \brief add graphical object to scene
  void Add(const GfxNodeP& go, bool redraw=true);
  /// \brief remove graphical object from scene
  /// remove graphical object from the scene
  void Remove(const GfxNodeP& go);
  /// remove graphical object from the scene
  void Remove(const String& name);
  /// \brief rename an existing graphical object
  /// defunct for now
  bool Rename(const String& old_name, const String& new_name);

  /// \brief retrieve gfx object by name
  GfxObjP operator[](const String& name);

  /// \brief actual event handling for scene (internal use)
  void Apply(const InputEvent& ie, bool request_redraw=true);

  /// \brief apply node visitor to root node
  void Apply(GfxNodeVisitor& v) const;
  
  /// \brief get total number of nodes in scene
  /// 
  /// To obtain the number of top-level nodes, use GfxNode::GetChildCount() of
  /// the root node
  size_t GetNodeCount() const;
  
  /// \brief get root node of scene graph
  GfxNodeP GetRootNode() const;
  //@}
  
  /// \brief observer interface (internal use)
  void AttachObserver(SceneObserver* o);
  /// \brief observer interface (internal use)
  void DetachObserver(SceneObserver* o);
  
  bool InOffscreenMode() const;

  /// \brief switch into test mode (internal use)
  void SetTestMode(bool t);

  float ElapsedTime() const;

  Viewport GetViewport() const;

  /*!
    This method has two different tasks. 

    During interactive rendering, it facilitates export 
    into an offscreen buffer with Scene::Export(file,width,height)
    by avoiding repeated initializations of the GL state, e.g.
    during animation rendering.

    During batch mode, this is the only way to get meaningful
    functionality with the gfx module
  */
  void StartOffscreenMode(unsigned int w, unsigned int h);
  /// \brief stops offline rendering in interactive mode
  void StopOffscreenMode();
  
  // temporary interface
  void ActivateShader(const String& name);

  void SetBlur(uint n);
  void BlurSnapshot();

  void RenderText(const TextPrim& t);

protected:
  friend class GfxObj; 
  friend class GfxNode;

  // TODO: this is really a hack and not clean communication
  friend class Entity;
  
  void ObjectChanged(const String& name);
  void SelectionChanged(const String& name, const mol::EntityView& sel);
  void NodeTransformed(const GfxObjP& object);
  void NodeAdded(const GfxNodeP& node);
  void RenderModeChanged(const String& name);

private:  
  template <typename ACTION>
  void NotifyObservers(const ACTION& action);  
  Scene();
  Scene(const Scene&) {}
  Scene& operator=(const Scene&) {return *this;}

  GLWinBase*           win_; // target gl window

  mutable GfxNodeP     root_node_; // mutable is slightly hackish
  SceneObserverList    observers_;

  mol::Transform transform_; // overall modelview transformation

  float fov_; // field of view
  float znear_,zfar_; // near and far clipping plane
  float fnear_,ffar_; // fog near and far offsets

  unsigned int vp_width_,vp_height_; // viewport

  SceneViewStack scene_view_stack_;

  float aspect_ratio_; // aspect ratio for viewport

  Color background_; // background (clear) color

  geom::Vec3 light_dir_; // infinite light source direction
  geom::Mat3 light_rot_; // transform needed for the shadow map
  Color light_amb_;
  Color light_diff_;
  Color light_spec_;

  bool axis_flag_;
  bool fog_flag_;
  Color fog_color_;
  bool shadow_flag_;
  int shadow_quality_;
  GLuint shadow_tex_id_;
  GLuint depth_tex_id_;
  GLuint kernel_tex_id_;
  GLuint scene_tex_id_;
  bool auto_autoslab_;

  bool offscreen_flag_; // a simple indicator whether in offscreen mode or not
  OffscreenBuffer* main_offscreen_buffer_; // not null if a main offscreen buffer is present
  uint old_vp_[2]; // used by the offline rendering code

  uint selection_mode_;

  bool test_flag_;
  std::vector<unsigned char> tmp_tex_;

  GLuint glyph_tex_id_;
  std::vector<geom::Vec2> glyph_map_;
  float def_text_size_;

  uint blur_count_;
  std::vector<boost::shared_array<unsigned char> > blur_buffer_;
  GLuint  glow_tex_;
  unsigned int stereo_;
  bool stereo_inverted_;
  unsigned int stereo_eye_;
  float stereo_eye_dist_,stereo_eye_offset_;

  void set_near(float n);
  void set_far(float f);
  void update_fog();
  void prep_shadow_map();
  void prep_depth_map();
  void flag_all_dirty();
  void prep_glyphs();
  void prep_blur();
  void stereo_projection(unsigned int view);
  void render_interlaced_stereo();
  void render_quad_buffered_stereo();
  void render_scene_with_glow();
  bool IsNameAvailable(String name);
};

}} // ns

#endif
