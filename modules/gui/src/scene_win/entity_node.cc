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
#include <boost/pointer_cast.hpp>

#include <QFont>
#include <QString>

#include <ost/gui/gosty_app.hh>
#include <ost/gui/scene_win/scene_win.hh>
#include <ost/mol/query_view_wrapper.hh>

#include "current_selection_node.hh"
#include "entity_part_node.hh"
#include "label_node.hh"
#include "render_modes_node.hh"

#include "entity_node.hh"



namespace ost { namespace gui {

EntityNode::EntityNode(gfx::EntityP& entity, SceneNode* parent):
    GfxSceneNode(entity,parent),custom_view_(NULL){
  SceneWinModel* model = GostyApp::Instance()->GetSceneWin()->GetModel();
  model->AddNode(parent, this);

  new RenderModesNode(entity, this);

  SceneNode* quick_selection = new LabelNode("Quick Selection",this);
  model->AddNode(this, quick_selection);

  SceneNode* node = new EntityPartNode("Backbone", entity, mol::QueryViewWrapper(entity->GetView().Select("aname=CA,C,N,O and peptide=true")), quick_selection);
  model->AddNode(quick_selection, node);
  node = new EntityPartNode("Ligands", entity, mol::QueryViewWrapper(entity->GetView().Select("ishetatm=1 and ele=C")), quick_selection);
  model->AddNode(quick_selection, node);

  custom_view_ = new LabelNode("Custom Views", this);
  model->AddNode(this, custom_view_);

  node = new EntityPartNode("Full View", entity, mol::QueryViewWrapper(entity->GetView()), this);
  model->AddNode(this, node);

  node = new CurrentSelectionNode(entity, this);
  model->AddNode(this, node);
}

SceneNode* EntityNode::GetCustomViewNode(){
  return custom_view_;
}

}}

