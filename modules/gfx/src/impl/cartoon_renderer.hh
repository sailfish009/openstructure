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
#ifndef OST_GFX_CARTOON_RENDERER_HH
#define OST_GFX_CARTOON_RENDERER_HH

#include <ost/geom/geom.hh>

#include <ost/gfx/entity_fw.hh>

#include <ost/gfx/impl/trace_renderer_base.hh>
#include <ost/gfx/impl/entity_detail.hh>
#include <ost/gfx/render_options/cartoon_render_options.hh>
#include <ost/gfx/impl/backbone_trace.hh>

namespace ost { namespace gfx { namespace impl {

/// \brief display entity in cartoon mode
/// \internal
class DLLEXPORT_OST_GFX CartoonRenderer: public TraceRendererBase {
public:
  CartoonRenderer(BackboneTrace& trace, bool force_tube=false);

  virtual void PrepareRendering();
  virtual void Render();

  virtual bool CanSetOptions(RenderOptionsPtr& render_options);
  virtual void SetOptions(RenderOptionsPtr& render_options);
  virtual RenderOptionsPtr GetOptions();


  virtual void SetForceTube(bool force_tube);


  virtual ~CartoonRenderer();
  
private:
  void RebuildSplineObj(const SplineEntryList& l, IndexedVertexArray& va,
                        SplineEntryListList& spline_list_list,
                        const TraceSubset& subset, bool is_sel);
  
  void CapProfile(const impl::TraceProfile& p, 
                  const impl::SplineEntry& se, 
                  bool flipn, IndexedVertexArray& va);
                  
  void AssembleProfile(const TraceProfile& prof1,
                       const TraceProfile& prof2, 
                       IndexedVertexArray& va);

  TraceProfile TransformAndAddProfile(const std::vector<TraceProfile>& profiles, 
                                      const SplineEntry& se, 
                                      IndexedVertexArray& va);

  void PrepareRendering(TraceSubset& subset, IndexedVertexArray& va, 
                        SplineEntryListList& spline_list_list, bool is_sel);
  static TraceProfile GetCircProfile(unsigned int detail, float rx, float ry, 
                                     float ex);
  bool force_tube_;
  CartoonRenderOptionsPtr options_;
  SplineEntryListList    spline_list_list_;
  SplineEntryListList    sel_spline_list_list_;
};

}}}

#endif // OST_GFX_CARTOON_RENDERER_HH
