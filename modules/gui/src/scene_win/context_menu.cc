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

}

void ContextMenu::ShowMenu(const QPoint& pos)
{
  QModelIndexList indexes = view_->selectionModel()->selection().indexes();

  bool all_visible = true;
  bool all_not_scene = true;
  bool all_hidden = true;
  bool all_entities = true;
  bool all_gfx_objects = true;
  bool all_entity_views = true;
  bool all_custom_views = true;
  bool all_maps = true;

  if(indexes.size()>0){
    for(int i = 0; i < indexes.size(); i++){
      if(indexes[i].column()==0){
        GfxSceneNode* gfx_scene_node = qobject_cast<GfxSceneNode*>(model_->GetItem(indexes[i]));
        if(gfx_scene_node){
          gfx::GfxNodeP gfx_node = gfx_scene_node->GetGfxNode();
          if(gfx_node->IsVisible()){all_hidden=false;}
          else{all_visible=false;}
          if(gfx_node->GetType()==0){all_not_scene = false;}
          if(!dynamic_cast<gfx::GfxObj*> (gfx_node.get())){all_gfx_objects = false;}
          if(!dynamic_cast<gfx::Entity*> (gfx_node.get())){all_entities = false;}
          if(!dynamic_cast<gfx::MapIso*> (gfx_node.get())){all_maps = false;}
        }
        else{
          all_gfx_objects = false;
          all_entities = false;
          all_visible = true;
          all_hidden = true;
        }
        EntityPartNode* entity_part_node = qobject_cast<EntityPartNode*>(model_->GetItem(indexes[i]));
        if(!entity_part_node){
          all_entity_views = false;
          all_custom_views = false;
        }
        else{
          CustomPartNode* custom_part_node = qobject_cast<CustomPartNode*>(entity_part_node);
          if(!custom_part_node){
            all_custom_views = false;
          }
        }
      }
    }

    QMenu* menu = new QMenu();

    QAction* action;
    if(all_gfx_objects){
      action = menu->addAction("Center on Object");
      connect(action, SIGNAL(triggered()), SceneSelection::Instance(), SLOT(CenterOnObjects()));

      if(all_entities){
        action = menu->addAction("Copy");
        connect(action, SIGNAL(triggered()), SceneSelection::Instance(), SLOT(CopyViews()));
        action = menu->addAction("Create Custom View");
        connect(action, SIGNAL(triggered()), this, SLOT(AddViewFromEntity()));
        action = menu->addAction("Select..");
        connect(action, SIGNAL(triggered()), SceneSelection::Instance(), SLOT(Select()));
        action = menu->addAction("Deselect");
        connect(action, SIGNAL(triggered()), SceneSelection::Instance(), SLOT(Deselect()));
      }

      if(all_not_scene){
                  action = menu->addAction("Delete");
                  connect(action, SIGNAL(triggered()), SceneSelection::Instance(), SLOT(Delete()));
      }
    }

    if(!all_visible){
      action = menu->addAction("Show");
      connect(action, SIGNAL(triggered()), SceneSelection::Instance(), SLOT(Show()));
    }
    if(!all_hidden){
      action = menu->addAction("Hide");
      connect(action, SIGNAL(triggered()), SceneSelection::Instance(), SLOT(Hide()));
    }

    if(all_entity_views){
      action = menu->addAction("Show");
      connect(action, SIGNAL(triggered()), SceneSelection::Instance(), SLOT(MakeVisible()));

      action = menu->addAction("Hide");
      connect(action, SIGNAL(triggered()), SceneSelection::Instance(), SLOT(MakeHidden()));

      action = menu->addAction("Show Exclusive");
      connect(action, SIGNAL(triggered()), SceneSelection::Instance(), SLOT(ShowExclusive()));

      action = menu->addAction("Hide Exclusive");
      connect(action, SIGNAL(triggered()), SceneSelection::Instance(), SLOT(HideExclusive()));

      action = menu->addAction("Select All");
      connect(action, SIGNAL(triggered()), SceneSelection::Instance(), SLOT(SelectAllViews()));

      action = menu->addAction("Deselect All");
      connect(action, SIGNAL(triggered()), SceneSelection::Instance(), SLOT(DeselectAllViews()));

      action = menu->addAction("Select..");
      connect(action, SIGNAL(triggered()), SceneSelection::Instance(), SLOT(SelectViews()));

      action = menu->addAction("Create Custom View");
      connect(action, SIGNAL(triggered()), this, SLOT(AddView()));

    }

    #if OST_IMG_ENABLED

    if(all_maps){
      action = menu->addAction("View Density Slices");
      connect(action, SIGNAL(triggered()), SceneSelection::Instance(), SLOT(ViewDensitySlices()));
    }

    #endif // OST_IMG_ENABLED


    if(all_entity_views){
      if(all_custom_views){
        action = menu->addAction("Delete");
        connect(action, SIGNAL(triggered()), this, SLOT(DeleteView()));
      }
    }

    if((all_gfx_objects || all_custom_views) && indexes.size()==2){
      action = menu->addAction("Rename");
      connect(action, SIGNAL(triggered()), this, SLOT(Rename()));
    }

    if(menu->actions().size()>0){
      menu->popup(pos);
    }
  }
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

