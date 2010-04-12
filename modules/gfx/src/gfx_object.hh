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

#include <ost/gfx/color_ops/color_op.hh>

#include <ost/geom/geom.hh>
#include <ost/mol/entity_handle.hh>
#include <ost/mol/entity_view.hh>
#include <ost/mol/transform.hh>


#if OST_IMG_ENABLED
#  include <ost/img/map.hh>
#endif

#include "gfx_object_fw.hh"
#include "gfx_node.hh"
#include "gfx_prim.hh"
#include "vertex_array.hh"
#include "input.hh"
#include "render_mode.hh"
#include "material.hh"
#include "gradient.hh"


namespace ost { namespace gfx {

class Scene; // fw decl

/// \brief Base class for all graphical rendering objects
class DLLEXPORT_OST_GFX GfxObj: public GfxNode
{

public:
  GfxObj(const String& name);
  virtual ~GfxObj();

  virtual GfxNodeP Copy() const;

  virtual void Apply(GfxNodeVisitor& v,GfxNodeVisitor::Stack st);

  virtual int GetType() const;

  virtual void RenderGL(RenderPass pass);

  virtual void RenderPov(PovState& pov);

  void Clear();
  
  /// \brief get transform
  const mol::Transform& GetTF() const;
  /// \brief set transform
  void SetTF(const mol::Transform& tf);
  
  virtual geom::Vec3 GetCenter() const;

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

  virtual void CustomRenderGL(RenderPass pass);

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
  
  virtual void DeepSwap(GfxObj& go);
  
  virtual void OnRenderModeChange();
  virtual void OnGLCleanup();

  /// \brief change render mode
  virtual void SetRenderMode(RenderMode::Type m);
  /// \brief current render mode
  RenderMode::Type GetRenderMode() const;

  /// \brief set line width
  void SetLineWidth(float w);
  
  /// \brief get line width 
  float GetLineWidth() const;

  void SetSphereDetail(unsigned int d);
  unsigned int GetSphereDetail() const;

  // subdivions per 90deg
  void SetArcDetail(unsigned int d);
  unsigned int GetArcDetail() const;

  void SetSplineDetail(unsigned int d);
  unsigned int GetSplineDetail() const;

  void SetPolyMode(unsigned int m);
  unsigned int GetPolyMode() const;

  // sophisticated line anti-aliasing, requires shader
  void SetAALines(bool f);
  bool GetAALines() const;

  void SetLineHalo(float f);
  float GetLineHalo() const;

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

  void SetMatAmb(const Color& c);
  void SetMatAmb(float c);
  void SetMatDiff(const Color& c);
  void SetMatDiff(float c);
  void SetMatSpec(const Color& c);
  void SetMatSpec(float c);
  void SetMatShin(float s);
  void SetMatEmm(const Color& c);
  void SetMatEmm(float c);
  void SetMat(const Color& amb, const Color& diff, const Color& spec, float shin);
  void SetMat(float amb, float diff, float spec, float shin);

  void SetOpacity(float o);
  float GetOpacity() const;

  static Color Ele2Color(const String& ele);

  void FlagRebuild();
  void FlagRefresh();

  void SetNormalSmoothFactor(float smoothf);
  float GetNormalSmoothFactor() const;

  Material GetMaterial() const;
  void SetMaterial(const Material& m);

  void SetOutlineMode(int m);
  void SetOutlineExpandFactor(float f);
  void SetOutlineExpandColor(const Color& c);

  // experimental, don't use
  void SmoothVertices(float smoothf);
  // experimental, don't use
  void AmbientOcclusion(bool f);
 
  void GLCleanup();

  /// \brief color each component based on the gradient-mapped property of 
  ///    the given entity
  virtual void ColorBy(const mol::EntityView& ev, 
                       const String& prop,
                       const Gradient& g, float minv, float maxv);
    
  /// \brief convenience method
  void ColorBy(const mol::EntityView& ev, 
               const String& prop,
               const Color& c1, const Color& c2, float minv, float maxv);

  /// \brief convenience method
  void ColorBy(const mol::EntityHandle& eh, 
               const String& prop,
               const Gradient& g, float minv, float maxv);
  /// \brief convenience method
  void ColorBy(const mol::EntityHandle& eh, 
               const String& prop,
               const Color& c1, const Color& c2, float minv, float maxv);
  // convenience method
  void ColorBy(const mol::EntityView& ev, 
               const String& prop,
               const Gradient& g);
  // convenience method
  void ColorBy(const mol::EntityView& ev, 
               const String& prop,
               const Color& c1, const Color& c2);
  // convenience method
  void ColorBy(const mol::EntityHandle& ev, 
               const String& prop,
               const Gradient& g);
  // convenience method
  void ColorBy(const mol::EntityHandle& ev, 
               const String& prop,
               const Color& c1, const Color& c2);
#if OST_IMG_ENABLED
  // convenience method
  void ColorBy(const img::MapHandle& mh,
               const String& prop,
               const Gradient& g);
  // convenience method
  void ColorBy(const img::MapHandle& mh,
               const String& prop,
               const Color& c1, const Color& c2);
  // dito for maps
  virtual void ColorBy(const img::MapHandle& mh,
                       const String& prop,
                       const Gradient& g,float minv, float maxv);
  // convenience method
  void ColorBy(const img::MapHandle& mh,
               const String& prop,
               const Color& c1, const Color& c2, float minv, float maxv);  
#endif

  unsigned int& Debug() {return debug_flags_;}

 protected:
  
  void PreRenderGL(bool flag);
  virtual void CustomPreRenderGL(bool flag);

  void RefreshVA(IndexedVertexArray& va);
  
 private:
  GfxObj(const GfxObj& o);
  GfxObj& operator=(const GfxObj&);

 protected:
  void AppendColorOp(gfx::ColorOp* op);
  void CleanColorOps();
  void ReapplyColorOps();

  IndexedVertexArray va_;
  RenderMode::Type render_mode_;
  unsigned int debug_flags_;
 
 private: 
  mol::Transform transform_;
  bool rebuild_;
  bool refresh_;
  float line_width_;
  unsigned int sphere_detail_;
  unsigned int arc_detail_;
  unsigned int spline_detail_;
  unsigned int poly_mode_;
  bool aalines_flag_;
  float line_halo_;

  Material mat_;
  unsigned int mat_dlist_;
  bool mat_update_;

  float opacity_;
  float smoothf_;
  int omode_;

  boost::ptr_vector<gfx::ColorOp> c_ops_;

  TextPrimList labels_;
  void render_labels() const;

  bool use_occlusion_;
};

}} //ns

#endif
