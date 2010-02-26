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
#include "scene_node.hh"

#include <iostream>

namespace ost { namespace gui {

SceneNode::SceneNode(SceneNode* parent):QObject(parent){
  parent_ = parent;
}
SceneNode::SceneNode(QObject* parent):QObject(parent){
  parent_ = NULL;
}

SceneNode* SceneNode::GetParent() const{
  return parent_;
}

void SceneNode::AddChild(SceneNode* node){
  nodes_.append(node);
}

void SceneNode::RemoveChild(SceneNode* node){
  nodes_.removeAll(node);
}

int SceneNode::GetChildRow(const SceneNode* node) const{
  for(int i = 0; i < nodes_.size(); i++){
    if(this == node)return i;
  }
  return -1;
}

int SceneNode::GetChildCount() const{
  return nodes_.size();
}

int SceneNode::GetRow() const{
  if(parent_){
    return parent_->GetChildRow(this);
  }
  return 0;
}

bool SceneNode::IsParent(SceneNode* node) const{
  if(node == this)return true;
  for(int i = 0; i< nodes_.size(); i++){
    if(nodes_[i]==node){
      return true;
    }
    if(nodes_[i]->IsParent(node))
      return true;
  }
  return false;
}

SceneNode* SceneNode::GetChild(int row) const{
  if(row>=0 && row<nodes_.size()){
    return nodes_[row];
  }
  return NULL;
}

QVariant SceneNode::GetData(int column, int role){
  return QVariant();
}

int SceneNode::GetColumnCount() const{
  return 1;
}

bool SceneNode::SetData(int column, const QVariant& value, int role){
  return false;
}

Qt::ItemFlags SceneNode::Flags(int column) const{
  return Qt::ItemIsEnabled;
}

}}

