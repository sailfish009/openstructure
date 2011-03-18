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
#ifndef OST_XCS_EDITOR_HE
#define OST_XCS_EDITOR_HE
/*
  Author: Marco Biasini
 */

#include <ost/mol/module_config.hh>
#include <ost/geom/geom.hh>
#include <ost/mol/handle_type_fw.hh>

#include "editor_base.hh"

namespace ost { namespace mol {

/// \brief external coordinate system editor
/// 
/// For an introduction to editors go \ref editors "here".
class DLLEXPORT_OST_MOL XCSEditor : public EditorBase {
public:
  friend class EntityHandle;
    
  /// \brief create invalid xcs editor
  XCSEditor();
  
  ~XCSEditor();
  
  XCSEditor(const XCSEditor& rhs);
  
  XCSEditor& operator=(const XCSEditor& rhs);
  
  /// \brief set original position of atom
  /// This function also updates the stores transformed coordinates
  void SetAtomOriginalPos(const AtomHandle& atom,
                          const geom::Vec3& position);
                   
  /// \brief set transformed position of atom
  /// 
  /// This function also updates the original coordinates
  void SetAtomPos(const AtomHandle& atom,
                   const geom::Vec3& position);

  /// \brief apply additional transformation to all atoms
  /// 
  /// This transformation is applied \em after the transformation
  /// already stored in the entity
  void ApplyTransform(const geom::Mat4& transform); 

  /// \brief apply a new transformation to all atoms

  void SetTransform(const geom::Mat4& transform);

  /// \brief immediately update internal coordinate system
  void UpdateICS();  
protected:
   XCSEditor(const EntityHandle& ent, EditMode mode);
   
   void Update();
};

}} //ns

#endif
