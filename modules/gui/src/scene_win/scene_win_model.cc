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
/*
  Authors: Marco Biasini, Ansgar Philippsen
*/

#include "scene_win_model.hh"

#include <boost/pointer_cast.hpp>

#include <ost/gfx/scene.hh>
#include <ost/gfx/gfx_object.hh>
#include <ost/gfx/entity_fw.hh>
#include <ost/gfx/entity.hh>

#include <ost/gui/scene_win/root_node.hh>
#include <ost/gui/scene_win/gfx_scene_node.hh>
#include <ost/gui/scene_win/entity_node.hh>

#include <QSize>
#include <QFont>

namespace ost { namespace gui {

SceneWinModel::SceneWinModel(QObject* parent):
  QAbstractItemModel(parent)
{
  root_node_ = new RootNode(this);
  this->beginInsertRows(QModelIndex(),0,0);
  scene_node_ = new GfxSceneNode(gfx::Scene::Instance().GetRootNode(),root_node_);
  root_node_->AddChild(scene_node_);
  this->endInsertRows();
  gfx::Scene::Instance().AttachObserver(this);
}

SceneWinModel::~SceneWinModel()
{
  gfx::Scene::Instance().DetachObserver(this);
}

gfx::NodePtrList SceneWinModel::GetGfxNodes(QModelIndexList indexes){
  gfx::NodePtrList selected_nodes;
  for(int i = 0; i < indexes.size(); i++){
    if(indexes[i].column()==0){
      GfxSceneNode* gfx_scene_node = qobject_cast<GfxSceneNode*>(GetItem(indexes[i]));
      if(gfx_scene_node){
        gfx::GfxNodeP node = gfx_scene_node->GetGfxNode();
        selected_nodes.push_back(node);
      }
    }
  }
  return selected_nodes;
}

void SceneWinModel::Update(){
  emit this->dataChanged(QModelIndex(),QModelIndex());
}

QModelIndex SceneWinModel::index(int row, int col, const QModelIndex& parent) const
{
  SceneNode* parent_node = GetItem(parent);

  SceneNode* child_node = parent_node->GetChild(row);
  if (child_node)
      return createIndex(row, col, child_node);
  else
      return QModelIndex();
}

QModelIndex SceneWinModel::parent(const QModelIndex& index) const
{
  if(!index.isValid())return QModelIndex();

  SceneNode* node = GetItem(index);
  assert(node);
  SceneNode* parent_node = node->GetParent();

  if(parent_node==root_node_ || parent_node==NULL) return QModelIndex();

  return createIndex(parent_node->GetRow(), 0, parent_node);
}

int SceneWinModel::rowCount(const QModelIndex& parent) const
{
  SceneNode* parent_node = GetItem(parent);

  return parent_node->GetChildCount();
}

int SceneWinModel::columnCount(const QModelIndex& parent) const
{
  SceneNode* parent_node = GetItem(parent);

  return parent_node->GetColumnCount();
}

QVariant SceneWinModel::data(const QModelIndex& index, int role) const
{
  if (!index.isValid()) return QVariant();

  SceneNode* node = GetItem(index);
  if(!node) return QVariant();
  QVariant data = node->GetData(index.column(),role);
  return data;
}

QVariant SceneWinModel::headerData(int section, Qt::Orientation orientation,
                                    int role) const
{
  if (orientation == Qt::Horizontal && role == Qt::DisplayRole) {
    if(section==0) return QVariant("Visible");
    else if(section==1) return QVariant("Node");
  }
  return QVariant();
}

Qt::ItemFlags SceneWinModel::flags(const QModelIndex& index) const
{
  if (!index.isValid())
    return Qt::NoItemFlags;
  SceneNode* node = reinterpret_cast<SceneNode*>(index.internalPointer());
  if(node){
    return node->Flags(index.column());
  }

  return QAbstractItemModel::flags(index);
}

bool SceneWinModel::setData(const QModelIndex& index,
                             const QVariant& value, int role)
{
  if (!index.isValid()) {
    return false;
  }

  SceneNode* node = reinterpret_cast<SceneNode*>(index.internalPointer());
  if(node){
    bool ret = node->SetData(index.column(),value,role);
    if(ret && role==Qt::CheckStateRole){
      this->Update();
    }
    return ret;

  }
  return false;
}

void SceneWinModel::NodeAdded(const gfx::GfxNodeP& node)
{
  QModelIndex scene_node_model_index = this->index(0,0,QModelIndex());

  SceneNode* scene_node = NULL;
  gfx::EntityP e=boost::dynamic_pointer_cast<gfx::Entity>(node);
  if(e){
    scene_node = new EntityNode(e,scene_node_);
  }
  else{
    scene_node = new GfxSceneNode(node, scene_node_);
    this->AddNode(scene_node_,scene_node);
  }
}

void SceneWinModel::NodeRemoved(const gfx::GfxNodeP& node)
{
  int i=0;
  SceneNode* found = NULL;
  while(!found && i<scene_node_->GetChildCount()){
    GfxSceneNode* gfx_scene_node = qobject_cast<GfxSceneNode*>(scene_node_->GetChild(i));
    if(gfx_scene_node && node==gfx_scene_node->GetGfxNode()){
      found = gfx_scene_node;
    }
    else{
      i++;
    }
  }
  if(found){
    QModelIndex scene_node_model_index = this->index(0,0,QModelIndex());
    this->beginRemoveRows(scene_node_model_index,i,i);
    scene_node_->RemoveChild(found);
    this->endRemoveRows();
  }
}

SceneNode* SceneWinModel::GetItem(const QModelIndex &index) const
{
  if (index.isValid()) {
    SceneNode *node = reinterpret_cast<SceneNode*>(index.internalPointer());
    if (node) return node;
  }
  return root_node_;
}

bool SceneWinModel::AddNode(SceneNode* parent, SceneNode* child){
  QModelIndex parent_index = GetIndexOf(parent);
  if(parent_index.isValid()){
    this->beginInsertRows(parent_index,0,0);
    parent->AddChild(child);
    this->endInsertRows();
    return true;
  }
  return false;
}

QModelIndex SceneWinModel::GetIndexOf(SceneNode* node){
  return GetIndex(node,index(0,0,QModelIndex()));
}

QModelIndex SceneWinModel::GetIndex(SceneNode* node, QModelIndex parent){
  if(parent.isValid()){
    SceneNode* parent_node =reinterpret_cast<SceneNode*>(parent.internalPointer());
    if(parent_node == node)return parent;
    int i=parent_node->GetChildCount()-1;
    while(i>=0){
      SceneNode* child = parent_node->GetChild(i);
      if(child == node){
        return index(i,0,parent);
      }
      QModelIndex found = GetIndex(node, index(i,0,parent));
      if (found.isValid()) {
        return found;
      }
    }
  }
  return QModelIndex();
}

}} // ns

