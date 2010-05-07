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

#include <ost/img/map.hh>
#include <ost/gfx/impl/map_octree.hh>
#include "gfx_object.hh"
#include "map_iso_prop.hh"

namespace ost { namespace gfx {

enum MapIsoType {
  ORIGINAL_MAP,
  DOWNSAMPLED_MAP
};

class MapIso;
typedef boost::shared_ptr<MapIso> MapIsoP;

/// \brief isocontour rendering for \ref img::ImageHandle "3D image data"
/// 
/// Two render modes are supported: gfx::RenderMode::SIMPLE renders the map in
/// wireframe mode, gfx::RenderMode::FILL renders a shaded isocontoured map.
/// 
/// \sa gfx::MapSlab
class DLLEXPORT_OST_GFX MapIso: public GfxObj {
public:
  MapIso(const String& name, const img::MapHandle& mh,float level);

  MapIso(const String& name, const img::MapHandle& mh,
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
  
  /// \brief get the map handle of the currently displayed map
  // The following is a hack. For the DataViewer I need to pass a reference to an ImagHandle
  // that never goes out of scope, so I get a reference from here
  img::ImageHandle& GetMap();

  /// \brief get the map handle of the original map
  // The following is a hack. For the DataViewer I need to pass a reference to an ImagHandle
  // that never goes out of scope, so I get a reference from here
  img::ImageHandle& GetOriginalMap();

  /// \brief get the map handle of the downsampled map
  // The following is a hack. For the DataViewer I need to pass a reference to an ImagHandle
  // that never goes out of scope, so I get a reference from here
  img::ImageHandle& GetDownsampledMap();

  /// \brief sets the donwsampled map to active
  void ShowDownsampledMap();

  /// \brief sets the original map to active
  void ShowOriginalMap();

  /// \brief checks if the downsampled map is available
  bool IsDownsampledMapAvailable() const ;

  /// \brief returns the type of map currently being show
  MapIsoType GetShownMapType() const;

  /// \brief set  color
  /// 
  /// By default, the color is white.
  /// \sa GetColor()
  void SetColor(const Color& color) 
  { 
    color_=color; 
    this->FlagRebuild(); 
  }
  /// \brief get color
  /// \sa SetColor()
  const Color& GetColor() const { return color_; }
  void SetNSF(float smoothf);
  void SetDebugOctree(bool flag) { debug_octree_=flag; }

  /// \brief flags the octree to be rebuilt
  void MakeOctreeDirty();

  /// \brief checks is the octree needs to be rebuilt
  bool IfOctreeDirty() const;

protected:
  virtual void CustomPreRenderGL(bool flag);
  static img::ImageHandle DownsampleMap(const img::ImageHandle& mh);

private:
  img::MapHandle   original_mh_;
  img::MapHandle   downsampled_mh_;
  img::MapHandle   mh_;
  impl::MapOctree  octree_;
  float            level_;
  bool             normals_calculated_;
  uint             alg_;
  float            smoothf_;
  float            min_;
  float            max_;
  float            std_dev_;
  float            min_max_;
  bool             debug_octree_;
  Color            color_; 
  bool             dirty_octree_;
};

}}

#endif
