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
#ifndef OST_GFX_ENTITY_RENDERER_HH
#define OST_GFX_ENTITY_RENDERER_HH

/*
  Author: Stefan Scheuber, Marco Biasini
*/


#if defined(GetAtomProps)
#undef GetAtomProps
#endif
#include <vector>

#include <ost/mol/query_view_wrapper.hh>
#include <ost/mol/entity_view.hh>
#include <ost/mol/atom_handle.hh>
#include <ost/mol/entity_property_mapper.hh>

#include <ost/gfx/color.hh>
#include <ost/gfx/gfx_object.hh>
#include <ost/gfx/module_config.hh>
#include <ost/gfx/render_pass.hh>
#include <ost/gfx/vertex_array.hh>
#include <ost/gfx/render_options/render_options.hh>
#include <ost/gfx/impl/mapped_property.hh>

#include <ost/gfx/color_ops/color_op.hh>
#include <ost/gfx/color_ops/by_element_color_op.hh>
#include <ost/gfx/color_ops/by_chain_color_op.hh>
#include <ost/gfx/color_ops/uniform_color_op.hh>
#include <ost/gfx/color_ops/gradient_level_color_op.hh>
#include <ost/gfx/color_ops/entity_view_color_op.hh>
#if OST_IMG_ENABLED
#include <ost/gfx/color_ops/map_handle_color_op.hh>
#endif //OST_IMG_ENABLED
#include <ost/gfx/impl/entity_renderer_fw.hh>

namespace ost { namespace gfx { namespace impl {

typedef enum {
  DIRTY_VIEW   =1,
  DIRTY_VA     =2,
} DirtyFlag;

typedef char DirtyFlags;

/// \internal
class DLLEXPORT_OST_GFX EntityRenderer {
public:
  EntityRenderer();
  virtual ~EntityRenderer(){}

  /// \brief add view
  void AddView(const mol::EntityView& view);
  
  void SubstractView(const mol::EntityView& view);
  /// \brief reset rendering views to zero
  void ClearViews();
  
  virtual bool HasDataToRender() const;
  
  virtual const String& GetName() const;

  void SetEnabled(bool enabled=true);

  bool IsEnabled() const;

  /// \brief whether the renderer has a non-empty selection
  bool HasSelection() const;
  
  /// \brief
  void SetSelection(const mol::EntityView& sel);
  
  /// \brief hide / show part of the rendered data
  void SetVisible(const mol::EntityView& view, bool visible);

  const Color& GetSelectionColor() const;
  
  const Color& GetSelectionColorOutline() const;
  
  /// \brief get view to be rendered by this renderer.
  /// 
  /// Upon calling this method, the union of all views added to the renderer with
  /// AddView() is calculated by first reevaluating all queries and then 
  /// merging them into one view.
  mol::EntityView GetFullView();

  mol::EntityView GetEffectiveView();

  /// \brief update views
  virtual void UpdateViews()=0;
  
  //// \brief prepare render data, i.e vertex arrays
  virtual void PrepareRendering()=0;

  /// \brief render the entity
  /// 
  /// This is the place where the actual rendering should happen, i.e sending of
  /// the geometric primitives to the graphics card.
  /// 
  /// The rendering buffers and vertex arrays should be prepared in 
  /// PrepareRendering()
  virtual void Render(RenderPass pass=STANDARD_RENDER_PASS);
  
  ///\brief povray rendering call
  virtual void RenderPov(PovState& pov, const std::string& name);

  virtual bool CanSetOptions(RenderOptionsPtr& render_options)=0;
  virtual void SetOptions(RenderOptionsPtr& render_options)=0;
  
  virtual RenderOptionsPtr GetOptions()=0;

  virtual geom::AlignedCuboid GetBoundingBox() const=0;
  
  /// \brief pick atom
  /// 
  /// If picked_atom is a valid handle, picked_atom is only replaced if the 
  /// candidate picked atom is closer to the near clipping plane.
  virtual void PickAtom(const geom::Line3& line, Real line_width,
                        mol::AtomHandle& picked_atom)=0;

  /// \brief pick bond
  /// 
  /// \note calling PickBond on a trace-based renderer is a no-op as the notion 
  /// of bonds is not meaningful for these renders
  virtual void PickBond(const geom::Line3& line, Real line_width,
                        mol::BondHandle& picked_bond)=0;

  virtual void RenderOptionsChanged();

  virtual void Apply(const gfx::ByElementColorOp& op)=0;
  virtual void Apply(const gfx::ByChainColorOp& op)=0;
  virtual void Apply(const gfx::UniformColorOp& op)=0;
  virtual void Apply(const gfx::GradientLevelColorOp& op)=0;
  virtual void Apply(const gfx::EntityViewColorOp& op)=0;
#if OST_IMG_ENABLED
  virtual void Apply(const gfx::MapHandleColorOp& op)=0;
#endif

  bool IsDirty() const;
  
  void FlagPositionsDirty();

  void Debug(unsigned int flags);

  IndexedVertexArray& VA() {return va_;}
protected:
  virtual void SetName(const String& name);

  String                name_;
  bool                  enabled_;

  mol::EntityView       full_view_;
  mol::EntityView       effective_view_;
  mol::EntityView       hidden_view_;
  IndexedVertexArray    va_;
  
  mol::EntityView       sel_;
  mol::EntityView       full_sel_;
  IndexedVertexArray    sel_va_;
  
  DirtyFlags            sel_state_;
  DirtyFlags            state_;
  unsigned int          debug_flags_;
  float                 opacity_;
};

//Simplify color ops
struct ByElementGetCol {
  Color ColorOfAtom(mol::AtomHandle& atom) const{
    return GfxObj::Ele2Color(atom.GetElement());
  }
};

struct ByChainGetCol {
  ByChainGetCol(const ByChainColorOp& op):op_(op){}
  Color ColorOfAtom(mol::AtomHandle& atom) const{
    return op_.GetColor(atom.GetResidue().GetChain().GetName());
  }
  const ByChainColorOp& op_;
};

struct UniformGetCol {
  UniformGetCol(const Color& col):col_(col){ }
  Color ColorOfAtom(mol::AtomHandle& atom) const{
    return col_;
  }
  const Color& col_;
};

struct GradientLevelGetCol {
  GradientLevelGetCol(const GradientLevelColorOp& op):property_(op.GetProperty()),
      epm_(property_, op.GetLevel()),
      gradient_(op.GetGradient()),
      minv_(op.GetMinV()),
      maxv_(op.GetMaxV()){}
  Color ColorOfAtom(mol::AtomHandle& atom) const{
    try{
      float n=Normalize(epm_.Get(atom), minv_, maxv_);
      return gradient_.GetColorAt(n);
    }catch(std::exception&){
      LOGN_DEBUG("property " << property_ << " not found");
      return Color();
    }
  }
  String property_;
  mol::EntityPropertyMapper epm_;
  Gradient gradient_;
  float minv_, maxv_;
};

struct EntityViewGetCol {
  EntityViewGetCol(const EntityViewColorOp& op):property_(op.GetProperty()),
      ev_(op.GetEntityView()),
      gradient_(op.GetGradient()),
      minv_(op.GetMinV()),
      maxv_(op.GetMaxV()){}
  Color ColorOfAtom(mol::AtomHandle& atom) const{
    return MappedProperty(ev_,property_,gradient_,minv_,maxv_,atom.GetPos());
  }
  String property_;
  mol::EntityView ev_;
  Gradient gradient_;
  float minv_, maxv_;
};

#if OST_IMG_ENABLED
struct MapHandleGetCol {
  MapHandleGetCol(const MapHandleColorOp& op):property_(op.GetProperty()),
      mh_(op.GetMapHandle()),
      gradient_(op.GetGradient()),
      minv_(op.GetMinV()),
      maxv_(op.GetMaxV()){}
  Color ColorOfAtom(mol::AtomHandle& atom) const{
    return MappedProperty(mh_,property_,gradient_,minv_,maxv_,atom.GetPos());
  }
  String property_;
  img::MapHandle mh_;
  Gradient gradient_;
  float minv_, maxv_;
};
#endif

}}} //ns

#endif /* ENTITYRENDERER_HH_ */
