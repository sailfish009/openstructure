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
#ifndef OST_VERTEX_ARRAY_HH
#define OST_VERTEX_ARRAY_HH

/*
  encapsulation of vertex-array based OpenGL rendering

  Author: Ansgar Philippsen
*/

#include <vector>
#include <cassert>
#include <iostream>
#include <cmath>

#include <ost/log.hh>
#include <ost/geom/geom.hh>


#include "color.hh"
#include "material.hh"
#include "gfx_prim.hh"
#include "povray_fw.hh"

namespace ost { namespace gfx {

typedef unsigned int VertexID;
typedef unsigned int LineID;
typedef unsigned int TriID;
typedef unsigned int QuadID;
class DLLEXPORT_OST_GFX IndexedVertexArray {
 public:
  struct Entry {
    Entry();
    Entry(const geom::Vec3& vv, const geom::Vec3& nn, const Color& cc, const geom::Vec2& tt);
    float t[2];
    float c[4];
    float n[3];
    float v[3];
  };

  struct NormalizerVertexEntry {
    geom::Vec3 pos;
    TriID tri_list[50];  // this is a hack until the iso-contouring is fixed
    uint tri_count;
    float weight;
  };

  struct NormalizerTriEntry {
    TriID tid;
    VertexID id0,id1,id2;
    geom::Vec3 norm;
    float weight;
  };

  typedef std::vector<Entry> EntryList;
  typedef std::vector<VertexID> IndexList;
  typedef std::vector<NormalizerVertexEntry> NVEntryList;
  typedef std::vector<NormalizerTriEntry> NTEntryList;

  IndexedVertexArray();
  ~IndexedVertexArray();

  IndexedVertexArray(const IndexedVertexArray& va);
  IndexedVertexArray& operator=(const IndexedVertexArray& va);

  static unsigned int GetFormat();

  void Cleanup();

  /*
    bitmask : 0 = render points only
              1 = render lines
              2 = render tris and quads
   */
  void SetMode(int m);
  int GetMode() const;

  // polygon mode, 2:fill, 1:line, 0:point (different to overall mode)
  void SetPolyMode(int m);

  void SetLighting(bool f);
  void SetTwoSided(bool f);
  void SetCullFace(bool f);
  void SetColorMaterial(bool f);
  void SetLineWidth(float lw);
  void SetAALines(bool f);
  void SetPointSize(float ps);
  void SetLineHalo(float lh);

  void SetOutlineMode(int m);
  void SetOutlineWidth(float f);
  void SetOutlineMaterial(const Material& m);
  void SetOutlineExpandFactor(float f);
  void SetOutlineExpandColor(const Color& c);

  // vertex, normal, color and texcoord (T2F_C4F_N3F_V3F)
  VertexID Add(const geom::Vec3& vert, const geom::Vec3& norm, const Color& col, const geom::Vec2& tex=geom::Vec2());

  unsigned int GetVertexCount() const;
  void DumpVertices() const;

  // add line given two vertex ids 
  LineID AddLine(VertexID id0, VertexID id1);

  // add triangle given three vertex ids
  TriID AddTri(VertexID id0, VertexID id1, VertexID id2);

  // as above, but generate a normal based on the given ids
  TriID AddTriN(VertexID id0, VertexID id1, VertexID id2);

  // add quad given four vertex ids
  QuadID AddQuad(VertexID id0, VertexID id1, VertexID id2, VertexID id3);

  // add a normal sphere
  void AddSphere(const SpherePrim& prim, unsigned int detail);

  // add an icosahedral based sphere with the given params to the va
  void AddIcoSphere(const SpherePrim& prim, unsigned int detail);

  void AddCylinder(const CylinderPrim& prim, unsigned int detail,bool cap=false);

  geom::Vec3 GetVert(VertexID id) const;
  void SetVert(VertexID id, const geom::Vec3& vert);

  geom::Vec3 GetNormal(VertexID id) const;
  void SetNormal(VertexID id, const geom::Vec3& norm);

  Color GetColor(VertexID id) const;
  void SetColor(VertexID id, const Color& col);

  geom::Vec2 GetTexCoord(VertexID id) const;
  void SetTexCoord(VertexID id, const geom::Vec2& tex);

  void SetOpacity(float o);

  // OpenGL rendering call
  void RenderGL();

  // POVray export
  void RenderPov(PovState& pov, const std::string& name);

  // only removes the drawing elements
  void Clear();
  // removes all elements and resets internal state to default
  void Reset();

  // forces re-calculation of some buffered features
  void FlagRefresh();

  // for debugging, draw all normals
  void DrawNormals(bool f);

  // NOTE: all methods below could be delegated to the outside, 
  // using the GetEntries() and Get*Indices() member functions

  // experimental, do not use
  void CalcNormals(float smoothf);
  // experimental, do not use
  void CalcFullNormals();
  // experimental, do not use
  void SmoothNormals(float smoothf);
  // experimental, do not use
  void NPatch();
  // experimental, do not use
  void SmoothVertices(float smoothf);

  void UseTex(bool b) {use_tex_=b;}
  uint& TexID() {return tex_id_;}

  const EntryList& GetEntries() const {return entry_list_;}
  const IndexList& GetQuadIndices() const {return quad_index_list_;}
  const IndexList& GetTriIndices() const {return tri_index_list_;}
  const IndexList& GetLineIndices() const {return line_index_list_;}

 private:
  bool initialized_;
  
  EntryList entry_list_;
  IndexList quad_index_list_;
  IndexList tri_index_list_;
  IndexList line_index_list_;
  NTEntryList ntentry_list_;

  bool dirty_;

  int mode_;
  int poly_mode_;
  bool lighting_;
  bool cull_face_;
  bool two_sided_;
  bool color_mat_;
  float line_width_;
  bool aalines_flag_;
  float point_size_;
  float line_halo_;

  int outline_mode_;
  float outline_width_;
  Material outline_mat_;
  bool outline_mat_update_;
  unsigned int outline_mat_dlist_;
  float outline_exp_factor_;
  Color outline_exp_color_;

  bool draw_normals_;

  bool use_tex_;
  uint tex_id_;

  unsigned int buffer_id_[7]; // magic number related to the .cc buffer use

  void copy(const IndexedVertexArray& va);
  bool prep_buff();
  void draw_ltq(bool use_buff);
  void draw_p(bool use_buff);
  void draw_aalines();
  void draw_line_halo(bool use_buff);
};

}} // ns

#endif
