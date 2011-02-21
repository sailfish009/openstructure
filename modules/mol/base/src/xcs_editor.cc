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
#include "xcs_editor.hh"
#include <ost/mol/mol.hh>

#include "impl/atom_impl.hh"
#include "impl/entity_impl.hh"

#include <ost/invalid_handle.hh>

/*
  Author: Marco Biasini
 */
namespace ost { namespace mol {


XCSEditor::XCSEditor():
  EditorBase(EntityHandle(), UNBUFFERED_EDIT)
{
}

XCSEditor::XCSEditor(const EntityHandle& ent, EditMode mode):
  EditorBase(ent, mode)
{
  if (ent.IsValid()) {
    ent.Impl()->IncXCSEditorCount();
  }
}
  
XCSEditor::~XCSEditor()
{
  if (ent_.IsValid()) {
    ent_.Impl()->DecXCSEditorCount();
  }
}

XCSEditor::XCSEditor(const XCSEditor& rhs):
  EditorBase(rhs.ent_, rhs.mode_)
{
  if (ent_.IsValid()) {
    ent_.Impl()->IncXCSEditorCount();
  }  
}

XCSEditor& XCSEditor::operator=(const XCSEditor& rhs)
{
  ent_=rhs.ent_;
  if (ent_.IsValid()) {
    ent_.Impl()->IncXCSEditorCount();
  }
  return *this;
}

void XCSEditor::SetAtomPos(const AtomHandle& atom,
                           const geom::Vec3& position)
{
  CheckHandleValidity(atom);
  atom.Impl()->SetTransformedPos(position);
  geom::Mat4 inv_transformation_matrix = ent_.Impl()->GetInvTransfMatrix();
  geom::Vec3 original_pos = geom::Vec3(inv_transformation_matrix*geom::Vec4(position));
  atom.Impl()->SetOriginalPos(original_pos);
  ent_.Impl()->MarkICSDirty();
  ent_.Impl()->MarkOrganizerDirty();
  this->Update();
}


void XCSEditor::SetAtomOriginalPos(const AtomHandle& atom,
                           const geom::Vec3& position)
{
  CheckHandleValidity(atom);
  atom.Impl()->SetOriginalPos(position);
  geom::Mat4 transformation_matrix = atom.GetEntity().GetTransformationMatrix();
  geom::Vec3 transformed_pos = geom::Vec3(transformation_matrix*geom::Vec4(position));
  atom.Impl()->SetTransformedPos(transformed_pos);
  ent_.Impl()->MarkICSDirty();
  ent_.Impl()->MarkOrganizerDirty();
  this->Update();
}

void XCSEditor::ApplyTransform(const geom::Mat4& transform)
{
  ent_.Impl()->ApplyTransform(transform);
  ent_.Impl()->UpdateTransformedPos();
  ent_.Impl()->MarkICSDirty();
  ent_.Impl()->MarkOrganizerDirty();
  this->Update();
}


void XCSEditor::SetTransform(const geom::Mat4& transform)
{
  ent_.Impl()->SetTransform(transform);
  ent_.Impl()->UpdateTransformedPos();
  ent_.Impl()->MarkICSDirty();
  ent_.Impl()->MarkOrganizerDirty();
  this->Update();
}


void XCSEditor::Update()
{
  if (GetMode()==UNBUFFERED_EDIT) {
    ent_.Impl()->UpdateICSIfNeeded();
    ent_.Impl()->UpdateOrganizerIfNeeded();
  }
}

void XCSEditor::UpdateICS()
{
  ent_.Impl()->UpdateICSIfNeeded();
}

}} // ns
