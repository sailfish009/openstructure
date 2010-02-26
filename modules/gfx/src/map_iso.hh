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
#ifndef OST_GFX_MAP_ISO_HH
#define OST_GFX_MAP_ISO_HH

/*
  Author: Ansgar Philippsen, Marco Biasini
*/

#include <boost/shared_ptr.hpp>

#include <ost/iplt/map.hh>
#include <ost/gfx/impl/map_octree.hh>
#include "gfx_object.hh"
#include "map_iso_prop.hh"

namespace ost { namespace gfx {

class MapIso;
typedef boost::shared_ptr<MapIso> MapIsoP;

/// \brief isocontour rendering for \ref iplt::ImageHandle "3D image data"
/// 
/// Two render modes are supported: gfx::RenderMode::SIMPLE renders the map in
/// wireframe mode, gfx::RenderMode::FILL renders a shaded isocontoured map.
/// 
/// \sa gfx::MapSlab
class DLLEXPORT_OST_GFX MapIso: public GfxObj {
public:
  MapIso(const ost::String& name, const iplt::MapHandle& mh,float level);

  MapIso(const ost::String& name, const iplt::MapHandle& mh,
         float level, uint a);

  virtual geom::AlignedCuboid GetBoundingBox() const;
                             
  virtual geom::Vec3 GetCenter() const;
  
  virtual void CustomRenderGL(RenderPass pass);

  virtual void CustomRenderPov(PovState& pov);

  virtual void OnInput(const InputEvent& e);

  virtual void OnRenderModeChange();

  void Rebuild();
  
  /// \brief set isocontouring level
  /// 
  /// Will force rebuild of the vertex buffers/indices
  void SetLevel(float l);
  
  /// \brief get current isocontouring level
  float GetLevel() const;
  
  /// \brief get mean value of map
  float GetMean() const;
  
  /// \brief get std dev of map.
  float GetStdDev() const;

  void SetNSF(float smoothf);
  void SetDebugOctree(bool flag) { debug_octree_=flag; }
protected:
  virtual void CustomPreRenderGL(bool flag);

private:
  iplt::MapHandle  mh_;
  impl::MapOctree  octree_;
  float level_;
  bool normals_calculated_;
  uint alg_;
  float smoothf_;
  float min_;
  float max_;
  float std_dev_;
  float min_max_;
  bool debug_octree_;

};

}}

#endif
