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
#ifndef OST_GFX_PRIM_LIST_HH
#define OST_GFX_PRIM_LIST_HH

/*
  Author: Ansgar Philippsen
*/

#include <boost/shared_ptr.hpp>

#include <ost/geom/geom.hh>

#include "gfx.hh"

namespace ost { namespace gfx {

class PrimList;

typedef boost::shared_ptr<PrimList> PrimListP;

/// \brief rudimentary graphical lines rendering.
/// 
/// The primitives may be rendered either with gfx::RenderMode::SIMPLE or 
/// gfx::RenderMode::CUSTOM. The render mode can be changed with
/// GfxObj::SetRenderMode().
/// 
/// \see gfx::Cuboid, \ref primitives.py "Displaying Lines and Quads",
///      gfx::Quad, \ref gradient.py "Gradient Example",
///      \ref random_lines.py "Random Lines"
class DLLEXPORT_OST_GFX PrimList: public GfxObj 
{
  struct PointEntry {
    PointEntry(const geom::Vec3& p, const Color& c):
      pos(p), color(c) {}
    geom::Vec3 pos;
    Color color;
    geom::Mat3 rotmat;
  };

  typedef std::vector<PointEntry> PointEntryList;

  struct LineEntry {
    LineEntry(const geom::Vec3& p1, const geom::Vec3& p2, const Color& c):
      pos1(p1), pos2(p2), color(c) {}
    geom::Vec3 pos1,pos2;
    Color color;
    geom::Mat3 rotmat;
  };

  typedef std::vector<LineEntry> LineEntryList;

 public:
  /// \brief create new prim list
  PrimList(const String& name);

  virtual void ProcessLimits(geom::Vec3& minc, geom::Vec3& maxc, 
                             const mol::Transform& tf) const;
  /// \brief get center      
  virtual geom::Vec3 GetCenter() const;

  virtual void CustomRenderPov(PovState& pov);

  virtual void CustomRenderGL(RenderPass pass);

  virtual void OnRenderModeChange();

  /// \brief  clear all prims
  void Clear();

  /// \brief add line as defined by two endpoints
  void AddPoint(geom::Vec3& p, const Color& col=Color());

  /// \brief add line as defined by two endpoints
  void AddLine(geom::Vec3& p1, geom::Vec3& p2, const Color& col=Color());

  /// \brief cylinder diameter for custom rendering mode
  void SetDiameter(float d);
  
  /// \brief sphere radius for points in custom rendering mode
  void SetRadius(float r);

  /// \brief set global prims color, overriding individual ones
  void SetColor(const Color& c);

  void SetSphereDetail(unsigned int d);
  unsigned int GetSphereDetail() const {return sphere_detail_;}

  void SetArcDetail(unsigned int d);
  unsigned int GetArcDetail() const {return arc_detail_;}

 protected:
  virtual void CustomPreRenderGL(bool flag);

 private:
  PointEntryList points_;
  LineEntryList lines_;
  float radius_;
  float diameter_;
  unsigned int sphere_detail_;
  unsigned int arc_detail_;
  
  void render_simple();
  void render_custom();
};

/// \example primitives.py
/// 
/// Shows how to create grapical objects to render lines and rectangles in 3D 
/// space.
/// \sa \ref bbox.py "Bounding Box Example", \ref random_lines.py "Random Lines"

/// \example random_lines.py
/// 
///  Draw lines with random orientation in 3D space
/// \sa \ref bbox.py "Bounding Box Example", \ref primitives.py "Lines And Quads" 
}} // ns

#endif
