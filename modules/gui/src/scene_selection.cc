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
#include <ost/dyn_cast.hh>

#include <ost/geom/vec3.hh>
#include <ost/geom/vecmat3_op.hh>

#include <ost/gfx/scene.hh>
#include <ost/gfx/gfx_object.hh>
#include <ost/gfx/entity.hh>

#include <ost/gui/gosty_app.hh>
#include <ost/gui/scene_win/scene_win.hh>
#include <ost/gui/query_dialog.hh>
#include <ost/gui/query_dialog.hh>

#include "scene_selection.hh"

namespace ost { namespace gui {
SceneSelection* SceneSelection::scene_selection_ = NULL;

SceneSelection::SceneSelection():nodes_()
{
  SceneWin* scene_win = GostyApp::Instance()->GetSceneWin();
  QObject::connect(scene_win, SIGNAL(ActiveNodesChanged(gfx::NodePtrList)),
               this, SLOT(SetActiveNodes(gfx::NodePtrList)));
}

void SceneSelection::SetActiveNodes(gfx::NodePtrList nodes){
  nodes_ = nodes;
}

gfx::GfxNodeP SceneSelection::GetActiveNode(unsigned int pos){
  if(pos >= 0 && pos < nodes_.size()){
    return nodes_[pos];
  }
  else{
    throw Error("Index out of bounds: There is no active node at the given position");
  }
}

int SceneSelection::GetActiveNodeCount(){
  return nodes_.size();
}

SceneSelection* SceneSelection::Instance() {
  if (!SceneSelection::scene_selection_) {
    SceneSelection::scene_selection_=new SceneSelection;
  }
  return SceneSelection::scene_selection_;
}

void SceneSelection::CenterOnObjects() {
  geom::Vec3 min=geom::Vec3(std::numeric_limits<Real>::max(),
                  std::numeric_limits<Real>::max(),
                  std::numeric_limits<Real>::max());
  geom::Vec3 max=geom::Vec3(-std::numeric_limits<Real>::max(),
                  -std::numeric_limits<Real>::max(),
                  -std::numeric_limits<Real>::max());
  bool changed=false;
  for(unsigned int i = 0; i < nodes_.size(); i++){
    gfx::GfxNodeP node = nodes_[i];
    if (node) {
      gfx::GfxObj* obj = dynamic_cast<gfx::GfxObj*> (node.get());
      if (obj) {
        geom::AlignedCuboid pair = obj->GetBoundingBox();
        min = geom::Min(min, pair.GetMin());
        max = geom::Max(max, pair.GetMax());
        changed=true;
      }
    }
  }
  if(changed){
    gfx::Scene::Instance().SetCenter(geom::Vec3(((max.GetX()+min.GetX())/2),
                                                ((max.GetY()+min.GetY())/2),
                                                ((max.GetZ()+min.GetZ())/2)));
  }
}

void SceneSelection::Delete() {
  QList<gfx::GfxObjP> selected_objects;
  for(unsigned int i = 0; i < nodes_.size(); i++){
    gfx::GfxNodeP node = nodes_[i];
    if (node) {
      gfx::GfxObjP obj = dyn_cast<gfx::GfxObj> (node);
      if (obj) {
        selected_objects.append(obj);
      }
    }
  }
  for(int i=0; i < selected_objects.size(); i++){
      gfx::Scene::Instance().Remove(selected_objects[i]);
  }
}

void SceneSelection::Select() {
  QueryDialog d;
  if (d.exec() == QDialog::Accepted) {
    QString query = d.GetQueryString();
    for(unsigned int i = 0; i < nodes_.size(); i++){
      gfx::GfxNodeP node = nodes_[i];
      if (node) {
        gfx::Entity* obj = dynamic_cast<gfx::Entity*> (node.get());
        if (obj) {
          mol::Query q(query.toStdString());
          if (q.IsValid()) {
              mol::EntityView ent = obj->GetView().Select(q);
              obj->SetSelection(ent);
          }
        }
     }
    }
  }
}

void SceneSelection::CopyViews() {
  QueryDialog d;
  QList<gfx::GfxObjP> objects_to_add;
  if (d.exec() == QDialog::Accepted) {
    QString query = d.GetQueryString();
    for(unsigned int i = 0; i < nodes_.size(); i++){
      gfx::GfxNodeP node = nodes_[i];
      if (node) {
        gfx::Entity* obj = dynamic_cast<gfx::Entity*> (node.get());
        if (obj) {
          mol::Query q(query.toStdString());
          if (q.IsValid()) {
            mol::EntityView ent = obj->GetView().Select(q);
            gfx::EntityP new_obj(new gfx::Entity(obj->GetName()
                                 + " *", ent));
            objects_to_add.append(new_obj);
          }
        }
      }
    }
    for(int i=0; i < objects_to_add.size(); i++){
      gfx::Scene::Instance().Add(objects_to_add[i]);
    }
  }
}

void SceneSelection::Show() {
  gfx::Scene::Instance().GetRootNode()->Show();
  for(unsigned int i = 0; i < nodes_.size(); i++){
    gfx::GfxNodeP node = nodes_[i];
    if (node) {
      node->Show();
      GostyApp::Instance()->GetSceneWin()->Update();
    }
  }
}

void SceneSelection::Hide() {
  for(unsigned int i = 0; i < nodes_.size(); i++){
    gfx::GfxNodeP node = nodes_[i];
    if (node) {
      node->Hide();            
      GostyApp::Instance()->GetSceneWin()->Update();      
    }
  }
}

void SceneSelection::ShowMenu(const QPoint & p){
  QMenu* menu = new QMenu();
  bool all_visible = true;
  bool all_not_scene = true;
  bool all_hidden = true;
  bool all_entities = true;
  bool all_gfx_objects = true;
  for(unsigned int i = 0; i < nodes_.size(); i++){
    if(nodes_[i]){
      if(nodes_[i]->IsVisible()){all_hidden=false;}
      else{all_visible=false;}
      if(nodes_[i]->GetType()==0){all_not_scene = false;}
      if(!dynamic_cast<gfx::GfxObj*> (nodes_[i].get())){all_gfx_objects = false;}
      if(!dynamic_cast<gfx::Entity*> (nodes_[i].get())){all_entities = false;}
    }
  }
  QAction* action;
  if(all_gfx_objects){
    action = menu->addAction("Center on Object");
    connect(action, SIGNAL(triggered()), this, SLOT(CenterOnObjects()));
  }
  if(all_entities){
    action = menu->addAction("Copy");
    connect(action, SIGNAL(triggered()), this, SLOT(CopyViews()));
    action = menu->addAction("Select");
    connect(action, SIGNAL(triggered()), this, SLOT(Select()));
  }
  if(all_not_scene){
    action = menu->addAction("Delete");
    connect(action, SIGNAL(triggered()), this, SLOT(Delete()));
  }
  if(!all_visible){
    action = menu->addAction("Show");
    connect(action, SIGNAL(triggered()), this, SLOT(Show()));
  }
  if(!all_hidden){
    action = menu->addAction("Hide");
    connect(action, SIGNAL(triggered()), this, SLOT(Hide()));
  }
  menu->popup(p);
}
}}

