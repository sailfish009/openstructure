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
#include <ost/mol/atom_handle.hh>
#include <ost/mol/residue_handle.hh>
#include <ost/mol/chain_handle.hh>
#include <ost/mol/bond_handle.hh>

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

void XCSEditor::SetAtomTransformedPos(const AtomHandle& atom,
                                      const geom::Vec3& position)
{
  CheckHandleValidity(atom);
  atom.Impl()->TransformedPos()=position;
  if(ent_.Impl()->IsTransfIdentity()) {
    atom.Impl()->OriginalPos()=position;
  } else {
    atom.Impl()->OriginalPos() = geom::Vec3(ent_.Impl()->GetInvTransfMatrix()*geom::Vec4(position));
  }
  ent_.Impl()->MarkICSDirty();
  ent_.Impl()->MarkOrganizerDirty();
  this->Update();
}

namespace {
  template<typename T>
  void set_transformed_pos(impl::EntityImpl* ent, const AtomHandleList& alist, T *positions)
  {
    bool has_tf=ent->IsTransfIdentity();
    for(AtomHandleList::const_iterator ait=alist.begin();ait!=alist.end();++ait) {
      if(ait->IsValid()) {
        ait->Impl()->TransformedPos()[0]=static_cast<Real>(positions[0]);
        ait->Impl()->TransformedPos()[1]=static_cast<Real>(positions[1]);
        ait->Impl()->TransformedPos()[2]=static_cast<Real>(positions[2]);
        if(has_tf) {
          ait->Impl()->OriginalPos()=ait->Impl()->TransformedPos();
        } else {
          ait->Impl()->OriginalPos() = geom::Vec3(ent->GetInvTransfMatrix()*geom::Vec4(ait->Impl()->TransformedPos()));
        }
      }
      positions+=3;
    }
    ent->MarkICSDirty();
    ent->MarkOrganizerDirty();
  }
} // anon ns

void XCSEditor::SetAtomTransformedPos(const AtomHandleList& alist, float *positions)
{
  set_transformed_pos<float>(ent_.Impl().get(),alist,positions);
  this->Update();
}

void XCSEditor::SetAtomTransformedPos(const AtomHandleList& alist, double *positions)
{
  set_transformed_pos<double>(ent_.Impl().get(),alist,positions);
  this->Update();
}

void XCSEditor::SetAtomOriginalPos(const AtomHandle& atom,
                                   const geom::Vec3& position)
{
  CheckHandleValidity(atom);
  atom.Impl()->OriginalPos()=position;
  if(ent_.Impl()->IsTransfIdentity()) {
    atom.Impl()->TransformedPos()=position;
  } else {
    atom.Impl()->TransformedPos() = geom::Vec3(ent_.Impl()->GetTransfMatrix()*geom::Vec4(position));
  }
  ent_.Impl()->MarkICSDirty();
  ent_.Impl()->MarkOrganizerDirty();
  this->Update();
}

namespace {
  template<typename T>
  void set_original_pos(impl::EntityImpl* ent, const AtomHandleList& alist, T *positions)
  {
    bool has_tf=ent->IsTransfIdentity();
    for(AtomHandleList::const_iterator ait=alist.begin();ait!=alist.end();++ait) {
      if(ait->IsValid()) {
        ait->Impl()->OriginalPos()[0]=static_cast<Real>(positions[0]);
        ait->Impl()->OriginalPos()[1]=static_cast<Real>(positions[1]);
        ait->Impl()->OriginalPos()[2]=static_cast<Real>(positions[2]);
        if(has_tf) {
          ait->Impl()->TransformedPos()=ait->Impl()->OriginalPos();
        } else {
          ait->Impl()->TransformedPos() = geom::Vec3(ent->GetTransfMatrix()*geom::Vec4(ait->Impl()->OriginalPos()));
        }
      }
      positions+=3;
    }
    ent->MarkICSDirty();
    ent->MarkOrganizerDirty();
  }
} // anon ns

void XCSEditor::SetAtomOriginalPos(const AtomHandleList& alist, float *positions)
{
  set_original_pos<float>(ent_.Impl().get(),alist,positions);
  this->Update();
}

void XCSEditor::SetAtomOriginalPos(const AtomHandleList& alist, double *positions)
{
  set_original_pos<double>(ent_.Impl().get(),alist,positions);
  this->Update();
}

void XCSEditor::SetAtomPos(const AtomHandle& atom, const geom::Vec3& position)
{
  this->SetAtomTransformedPos(atom,position);
}

void XCSEditor::SetAtomPos(const AtomHandleList& alist, float *positions)
{
  this->SetAtomTransformedPos(alist,positions);
}

void XCSEditor::SetAtomPos(const AtomHandleList& alist, double *positions)
{
  this->SetAtomTransformedPos(alist,positions);
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
