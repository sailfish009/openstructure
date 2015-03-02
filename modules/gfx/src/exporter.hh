//------------------------------------------------------------------------------
// This file is part of the OpenStructure project <www.openstructure.org>
//
// Copyright (C) 2008-2015 by the OpenStructure authors
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
#ifndef OST_GFX_EXPORTER_HH
#define OST_GFX_EXPORTER_HH

#include <ost/gfx/module_config.hh>

namespace ost { namespace gfx {

class Scene;

class DLLEXPORT_OST_GFX Exporter
{
public:
  enum NodeType {
    ROOT=1,
    GROUP=2,
    OBJ=3
  };

  virtual ~Exporter() {}
  virtual void SceneStart(const Scene* scene) {}
  virtual void SceneEnd(const Scene* scene) {}

  virtual void NodeStart(const std::string& name, NodeType t) {}
  virtual void NodeEnd(const std::string& name) {}
  
  // this indicates beginning of new data, including a reset of the indices
  // may occur more than once for a given node
  virtual void WriteVertexData(const float* v, const float* n, const float* c, const float* t,
                               size_t stride, size_t count) {}
  virtual void WritePointData(const unsigned int* i, size_t count) {}
  virtual void WriteLineData(const unsigned int* ij, size_t count) {}
  virtual void WriteTriData(const unsigned int* ijk, size_t count) {}
  virtual void WriteQuadData(const unsigned int* ijkl, size_t count) {}
};

}} // ns

#endif
