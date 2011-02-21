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
#ifndef OST_GFX_OBJ_HH
#define OST_GFX_OBJ_HH

/*
  Author: Ansgar Philippsen
*/
// I have really no clue who defines this macro, but apparently  its there.
#ifdef check
#undef check
#endif
#include <boost/ptr_container/ptr_vector.hpp>

#include <vector>

#include <ost/config.hh>
#include <ost/gfx/module_config.hh>

#include <ost/mol/transform.hh>

#include "gfx_object_fw.hh"
#include "gfx_object_base.hh"
#include "gfx_prim.hh"
#include "vertex_array.hh"
#include "input.hh"

namespace ost { namespace gfx {

class Scene; // fw decl

/// \brief main class for all graphic objects
class DLLEXPORT_OST_GFX GfxObj: public GfxObjBase
{
public:
  GfxObj(const String& name);

  // gfx node interface
  virtual GfxNodeP Copy() const;
  virtual void DeepSwap(GfxObj& go);
  virtual void RenderGL(RenderPass pass);
  virtual void RenderPov(PovState& pov);
  virtual void Apply(GfxNodeVisitor& v,GfxNodeVisitor::Stack st);
  virtual int GetType() const;
  //

  // gfx obj base interface
  virtual void SetMatAmb(const Color& c);
  virtual void SetMatDiff(const Color& c);
  virtual void SetMatSpec(const Color& c);
  virtual void SetMatShin(float s);
  virtual void SetMatEmm(const Color& c);
  virtual void ContextSwitch();
  virtual void SetRenderMode(RenderMode::Type m);
  virtual RenderMode::Type GetRenderMode() const;
  virtual geom::Vec3 GetCenter() const;
  virtual void SetLineWidth(float w);
  virtual void SetPolyMode(unsigned int m);
  virtual void SetAALines(bool f);
  virtual void SetLineHalo(float f);
  virtual void SetOutline(bool f);
  virtual void SetOutlineMode(int m);
  virtual void SetOutlineWidth(float f);
  virtual void SetOutlineExpandFactor(float f);
  virtual void SetOutlineExpandColor(const Color& c);
  virtual void SetOpacity(float f);
  virtual float GetOpacity() const {return opacity_;}
  virtual void ColorBy(const mol::EntityView& ev, 
                       const String& prop,
                       const Gradient& g, float minv, float maxv);
#if OST_IMG_ENABLED
  virtual void ColorBy(const img::MapHandle& mh,
                       const String& prop,
                       const Gradient& g,float minv, float maxv);
#endif

  // end of gfx obj base interface

  // new gfx obj virtual interface starts here

  /// \brief returns the left-bottom-front and the right-top-back corner
  /// that encompasses all graphical elements in this object
  /// 
  /// the bounding box is in local coordinates. to obtain the coordinates in
  /// the scene, multiply the bounding box by the object's transformation
  ///  matrix.
  virtual geom::AlignedCuboid GetBoundingBox() const;

  /// \brief adjust the given limits according to the represented data
  virtual void ProcessLimits(geom::Vec3& minc, geom::Vec3& maxc, 
                             const mol::Transform& tf) const;

  // implemented in derived classes for the actual GL rendering
  /*
    note: only STANDARD_RENDER_PASS and GLOW_RENDER_PASS need to 
    be implemented, the rest is taken care of by GfxObj::RenderGL
  */
  virtual void CustomRenderGL(RenderPass pass);

  // implemented in derived classes for the actual POVray export
  virtual void CustomRenderPov(PovState& pov);

  // handle pick (select) event
  /*
    should return the position of the picked object in order for the caller
    to determine the closest one, and indicate with the boolean return wether
    the selection was successful
    the boolean flag indicates whether a true pick or just a feedback is requested
    the given zlim is the projected z of the best selection so far, and should be
    used for both picking as well as feedback if the selection is in front of the
    currently best one
  */
  virtual bool OnSelect(const geom::Line3& l, geom::Vec3& result, float zlim, bool pick_flag);

  // input event entry point
  virtual void OnInput(const InputEvent& e);

  // informs derived class that render mode has changes
  virtual void OnRenderModeChange();
  virtual void OnGLCleanup();
  //

  /// \brief removes all graphical elements
  void Clear();
  
  /// \brief get transform
  const mol::Transform& GetTF() const;
  /// \brief set transform
  void SetTF(const mol::Transform& tf);
  
  // add a label at the given position
  void AddLabel(const String& s, const geom::Vec3& pos, const Color& col, float psize);
  // convenience method
  void AddLabel(const String& s, const geom::Vec3& pos, const Color& col);
  // convenience method
  void AddLabel(const String& s, const geom::Vec3& pos, float psize);
  // convenience method
  void AddLabel(const String& s, const geom::Vec3& pos);
  // remove all labels
  void ClearLabels();

  void FlagRebuild();
  void FlagRefresh();

  bool GetAALines() const {return aalines_flag_;}
  float GetLineWidth() const {return line_width_;}
  float GetLineHalo() const {return line_halo_;}

  void SetNormalSmoothFactor(float smoothf);
  float GetNormalSmoothFactor() const;

  Material GetMaterial() const;
  void SetMaterial(const Material& m);

  // experimental, don't use
  void SmoothVertices(float smoothf);
 
  void GLCleanup();

  // this really should not be here
  static Color Ele2Color(const String& ele);

  void Debug(unsigned int flags);

 protected:
  
  void PreRenderGL(bool flag);
  virtual void CustomPreRenderGL(bool flag);

 private:
  GfxObj(const GfxObj& o);
  GfxObj& operator=(const GfxObj&);

 protected:
  void AppendColorOp(gfx::ColorOp* op);
  void CleanColorOps();
  void ReapplyColorOps();
  void render_labels() const;
  void render_depth_only();

  IndexedVertexArray va_;
  RenderMode::Type render_mode_;
  unsigned int debug_flags_;
 
  mol::Transform transform_;
  bool rebuild_;
  bool refresh_;
  float line_width_;
  unsigned int poly_mode_;
  bool aalines_flag_;
  float line_halo_;

  Material mat_;
  unsigned int mat_dlist_;
  bool mat_update_;

  float opacity_;
  float smoothf_;
  bool outline_flag_;
  int outline_mode_;

  boost::ptr_vector<gfx::ColorOp> c_ops_;

  TextPrimList labels_;

  bool use_occlusion_;
};

}} //ns

#endif
