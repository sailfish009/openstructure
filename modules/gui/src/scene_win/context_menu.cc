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
  Authors: Stefan Scheuber
*/
#include <boost/pointer_cast.hpp>

#include <QAction>
#include <QMenu>
#include <QItemSelection>
#include <QItemSelectionModel>

#include <ost/gui/scene_selection.hh>
#include <ost/gui/query_dialog.hh>

#if OST_IMG_ENABLED
#include <ost/gfx/map_iso.hh>
#endif // OST_IMG_ENABLED

#include "custom_part_node.hh"
#include "entity_node.hh"
#include "entity_part_node.hh"
#include "gfx_scene_node.hh"

#include "context_menu.hh"

namespace ost { namespace gui {

ContextMenu::ContextMenu(QTreeView* view, SceneWinModel* model):
  QObject(model),view_(view),model_(model)
{
  QAction* action = new QAction("Center on Object",this);
  connect(action, SIGNAL(triggered()), SceneSelection::Instance(), SLOT(CenterOnObjects()));
  this->AddAction(action, GFX_OBJECT);

  action = new QAction("Copy",this);
  connect(action, SIGNAL(triggered()), SceneSelection::Instance(), SLOT(CopyViews()));
  this->AddAction(action, ENTITY);
  action = new QAction("Create Custom View",this);
  connect(action, SIGNAL(triggered()), this, SLOT(AddViewFromEntity()));
  this->AddAction(action, ENTITY);
  action = new QAction("Select..", this);
  connect(action, SIGNAL(triggered()), SceneSelection::Instance(), SLOT(Select()));
  this->AddAction(action, ENTITY);
  action = new QAction("Deselect",this);
  connect(action, SIGNAL(triggered()), SceneSelection::Instance(), SLOT(Deselect()));
  this->AddAction(action, ENTITY);

  action = new QAction("Delete",this);
  connect(action, SIGNAL(triggered()), SceneSelection::Instance(), SLOT(Delete()));
  this->AddAction(action, GFX_OBJECT|NOT_SCENE);

  action = new QAction("Show",this);
  connect(action, SIGNAL(triggered()), SceneSelection::Instance(), SLOT(Show()));
  this->AddAction(action, GFX_OBJECT|NOT_VISIBLE);
  action = new QAction("Hide",this);
  connect(action, SIGNAL(triggered()), SceneSelection::Instance(), SLOT(Hide()));
  this->AddAction(action, GFX_OBJECT|NOT_HIDDEN);

  action = new QAction("Show",this);
  connect(action, SIGNAL(triggered()), SceneSelection::Instance(), SLOT(MakeVisible()));
  this->AddAction(action, ENTITY_VIEW);
  action = new QAction("Hide",this);
  connect(action, SIGNAL(triggered()), SceneSelection::Instance(), SLOT(MakeHidden()));
  this->AddAction(action, ENTITY_VIEW);
  action = new QAction("Show Exclusive",this);
  connect(action, SIGNAL(triggered()), SceneSelection::Instance(), SLOT(ShowExclusive()));
  this->AddAction(action, ENTITY_VIEW);
  action = new QAction("Hide Exclusive",this);
  connect(action, SIGNAL(triggered()), SceneSelection::Instance(), SLOT(HideExclusive()));
  this->AddAction(action, ENTITY_VIEW);
  action = new QAction("Select All",this);
  connect(action, SIGNAL(triggered()), SceneSelection::Instance(), SLOT(SelectAllViews()));
  this->AddAction(action, ENTITY_VIEW);
  action = new QAction("Deselect All",this);
  connect(action, SIGNAL(triggered()), SceneSelection::Instance(), SLOT(DeselectAllViews()));
  this->AddAction(action, ENTITY_VIEW);
  action = new QAction("Select..",this);
  connect(action, SIGNAL(triggered()), SceneSelection::Instance(), SLOT(SelectViews()));
  this->AddAction(action, ENTITY_VIEW);
  action = new QAction("Create Custom View",this);
  connect(action, SIGNAL(triggered()), this, SLOT(AddView()));
  this->AddAction(action, ENTITY_VIEW);

  action = new QAction("Delete",this);
  connect(action, SIGNAL(triggered()), this, SLOT(DeleteView()));
  this->AddAction(action, ENTITY_VIEW | CUSTOM_VIEW);

  action = new QAction("Rename",this);
  connect(action, SIGNAL(triggered()), this, SLOT(Rename()));
  this->AddAction(action, GFX_OBJECT | SINGLE);

  action = new QAction("Rename",this);
  connect(action, SIGNAL(triggered()), this, SLOT(Rename()));
  this->AddAction(action, ENTITY_VIEW | SINGLE);

#if OST_IMG_ENABLED
  action = new QAction("View Density Slices",this);
  connect(action, SIGNAL(triggered()), SceneSelection::Instance(), SLOT(ViewDensitySlices()));
  this->AddAction(action, MAP);
#endif // OST_IMG_ENABLED

}

void ContextMenu::ShowMenu(const QPoint& pos)
{
  QModelIndexList indexes = view_->selectionModel()->selection().indexes();
  ContextActionTypes flags;
  flags = ~flags;

  if(indexes.size()>0){
    for(int i = 0; i < indexes.size(); i++){
      if(indexes[i].column()==0){
        GfxSceneNode* gfx_scene_node = qobject_cast<GfxSceneNode*>(model_->GetItem(indexes[i]));
        if(gfx_scene_node){
          gfx::GfxNodeP gfx_node = gfx_scene_node->GetGfxNode();
          if(gfx_node->IsVisible()){
            flags &= ~NOT_HIDDEN;
          }
          else{
            flags &= ~NOT_VISIBLE;
          }
          if(gfx_node->GetType()==0){flags &= ~NOT_SCENE;}
          if(!dynamic_cast<gfx::GfxObj*> (gfx_node.get())){flags &= ~GFX_OBJECT;}
          if(!dynamic_cast<gfx::Entity*> (gfx_node.get())){flags &= ~ENTITY;}
#if OST_IMG_ENABLED
          if(!dynamic_cast<gfx::MapIso*> (gfx_node.get())){flags &= ~MAP;}
#endif // OST_IMG_ENABLED
        }
        else{
          flags &= ~(GFX_OBJECT | ENTITY
#if OST_IMG_ENABLED
              | MAP
#endif
          );
          flags &= ~(NOT_VISIBLE | NOT_HIDDEN);
        }
        EntityPartNode* entity_part_node = qobject_cast<EntityPartNode*>(model_->GetItem(indexes[i]));
        if(!entity_part_node){
          flags &= ~(ENTITY_VIEW | CUSTOM_VIEW);
        }
        else{
          CustomPartNode* custom_part_node = qobject_cast<CustomPartNode*>(entity_part_node);
          if(!custom_part_node){
            flags &= ~CUSTOM_VIEW;
          }
        }
      }
    }

    QMenu* menu = new QMenu();
    QMapIterator<QAction*, ContextActionTypes> i(actions_);
    flags ^= NOT_VISIBLE;
    flags ^= NOT_HIDDEN;
    while (i.hasNext()) {
        i.next();
        if (i.value() == (i.value() & flags)){
          menu->addAction(i.key());
        }
    }

    if(menu->actions().size()>0){
      menu->popup(pos);
    }
  }
}

void ContextMenu::AddAction(QAction* action,ContextActionTypes type){
  actions_[action] = type;
}

void ContextMenu::AddViewFromEntity() {
  QueryDialog d;
  if (d.exec() == QDialog::Accepted) {
    QString query = d.GetQueryString();
    int node_count = SceneSelection::Instance()->GetActiveNodeCount();
    for(int i = 0; i < node_count; i++){
      gfx::GfxNodeP node = SceneSelection::Instance()->GetActiveNode(i);
      if (node) {
        EntityNode* ent_node = qobject_cast<EntityNode*>(model_->FindGfxNode(node));
        if (ent_node) {
          mol::Query q(query.toStdString());
          if (q.IsValid()) {
            gfx::EntityP entity = boost::dynamic_pointer_cast<gfx::Entity>(ent_node->GetGfxNode());
            if(entity){
              CustomPartNode* child_node = new CustomPartNode("New View", entity, mol::QueryViewWrapper(q,entity->GetView()),ent_node->GetCustomViewNode());
              model_->AddNode(ent_node->GetCustomViewNode(),child_node);
            }
          }
        }
     }
    }
  }
}

void ContextMenu::AddView(){
  gfx::EntityP entity = SceneSelection::Instance()->GetViewEntity();

  EntityNode* ent_node = qobject_cast<EntityNode*>(model_->FindGfxNode(entity));
  if(ent_node)
  {
    mol::EntityView view = SceneSelection::Instance()->GetViewUnion();
    SceneNode* child_node = new CustomPartNode("New View", entity, mol::QueryViewWrapper(view),ent_node->GetCustomViewNode());
    model_->AddNode(ent_node->GetCustomViewNode(),child_node);
    this->Rename(model_->GetIndexOf(child_node,1));
  }
}

void ContextMenu::Rename(QModelIndex index){
  if(index.isValid()){
    view_->expand(index.parent());
    view_->edit(index);
  }
}

void ContextMenu::Rename(){
  QModelIndexList indexes = view_->selectionModel()->selection().indexes();
  if(indexes.size() == 2)
  {
    this->Rename(indexes[1]);
  }
}

void ContextMenu::DeleteView(){
  QModelIndexList indexes = view_->selectionModel()->selection().indexes();
  for(int i = 0; i < indexes.size(); i++){
    if(indexes[i].column()==0){
      model_->RemoveNode(model_->GetItem(indexes[i]));
    }
  }
}

}} // ns

