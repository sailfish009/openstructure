//------------------------------------------------------------------------------
// This file is part of the OpenStructure project <www.openstructure.org>
//
// Copyright (C) 2008-2009 by the OpenStructure authors
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
#include <QFont>

#include <ost/gfx/scene.hh>
#include <ost/gfx/gfx_node.hh>

#include <ost/gui/gosty_app.hh>
#include <ost/gui/scene_win/scene_win.hh>

#include "current_selection_node.hh"

namespace ost { namespace gui {

CurrentSelectionNode::CurrentSelectionNode(gfx::EntityP entity, SceneNode* parent):GfxSceneNode(entity,parent),visible_(true){
  //GostyApp::Instance()->GetSceneWin()->GetModel()->AttachSelectionObserver(this);
}

QVariant CurrentSelectionNode::GetData(int column, int role){
  if(column<0 || column > 2)return QVariant();

  if (role==Qt::CheckStateRole && column==0) {
    return QVariant(visible_ ? Qt::Checked : Qt::Unchecked);
  } else if(column==1) {
    if (role==Qt::DisplayRole) {
      return QVariant("Current Selection");
    } else if(role==Qt::FontRole) {
      QFont f("Helvetica");
      return QVariant(f);
    }
  }
  return QVariant();
}

int CurrentSelectionNode::GetColumnCount() const{
  return 2;
}

bool CurrentSelectionNode::SetData(int column, const QVariant& value, int role){
  if (column==0 && role == Qt::CheckStateRole) {
    gfx::EntityP entity =boost::dynamic_pointer_cast<gfx::Entity>(this->GetGfxNode());
    if (value.toBool()) {
      this->GetParent()->SetData(0,value,Qt::CheckStateRole);
      entity->SetVisible(entity->GetSelection(), true);
      visible_=true;
    } else {
      entity->SetVisible(entity->GetSelection(), false);
      visible_=false;
    }
    return true;
  }
  return false;
}

Qt::ItemFlags CurrentSelectionNode::Flags(int column) const{
  if(column==0){
    return Qt::ItemIsSelectable|Qt::ItemIsUserCheckable|Qt::ItemIsEnabled;
  }
  else if(column==1){
    return Qt::ItemIsSelectable|Qt::ItemIsEnabled;
  }
  return Qt::NoItemFlags;
}

void CurrentSelectionNode::SelectionChanged(){
  if(visible_){
    gfx::EntityP entity = boost::dynamic_pointer_cast<gfx::Entity>(this->GetGfxNode());
    entity->SetVisible(entity->GetSelection(),true);
  }
}

}}

