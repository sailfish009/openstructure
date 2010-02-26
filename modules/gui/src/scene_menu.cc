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
#include <ost/mol/view_op.hh>
#include <ost/gui/gosty_app.hh>
#include <ost/gfx/scene.hh>
#include <ost/gfx/entity.hh>

#include "scene_menu.hh"

namespace ost { namespace gui {

SceneMenu::SceneMenu() {
  context_menu_=new QMenu();
  select_menu_=context_menu_->addMenu("Select");
  QAction* a=select_menu_->addAction("Around Atom");
  connect(a, SIGNAL(triggered()), this, SLOT(SelectAroundAtom()));
  a=select_menu_->addAction("Backbone");
  connect(a, SIGNAL(triggered()), this, SLOT(SelectBackbone()));
  a=select_menu_->addAction("Sidechains");
  connect(a, SIGNAL(triggered()), this, SLOT(SelectSidechains()));
  a=select_menu_->addAction("Ligands");
  connect(a, SIGNAL(triggered()), this, SLOT(SelectLigands()));
  a=select_menu_->addAction("Nothing");
  connect(a, SIGNAL(triggered()), this, SLOT(UnSelect()));  
  center_menu_=context_menu_->addMenu("Center Camera On");
  a=center_menu_->addAction("Atom");
  connect(a, SIGNAL(triggered()), this, SLOT(CenterOnAtom()));
  a=center_menu_->addAction("Residue");
  connect(a, SIGNAL(triggered()), this, SLOT(CenterOnResidue()));
  a=center_menu_->addAction("Chain");
  connect(a, SIGNAL(triggered()), this, SLOT(CenterOnChain()));
  a=center_menu_->addAction("View");
  connect(a, SIGNAL(triggered()), this, SLOT(CenterOnView()));
  a=center_menu_->addAction("Selection");
  connect(a, SIGNAL(triggered()), this, SLOT(CenterOnSelection()));
}

void SceneMenu::SelectBackbone()
{
  this->Select("peptide=true and aname=CA,C,N,O");
}

void SceneMenu::SelectSidechains()
{
  this->Select("peptide=true and aname!=CA,C,N,O");
}

void SceneMenu::SelectLigands()
{
    this->Select("ishetatm=true and rname!=HOH");
}

void SceneMenu::SelectAroundAtom()
{
  gfx::EntityP e=dyn_cast<gfx::Entity>(picked_.first);
  mol::AtomViewList within=e->GetView().FindWithin(picked_.second.GetPos(),6.0);
  mol::EntityView v=CreateViewFromAtomList(within);
  v.AddAllInclusiveBonds();
  e->SetSelection(v);
}

void SceneMenu::Select(const String& str)
{
  gfx::EntityP e=dyn_cast<gfx::Entity>(picked_.first);
  e->SetSelection(e->GetView().Select(str));
}

void SceneMenu::UnSelect()
{
  gfx::EntityP e=dyn_cast<gfx::Entity>(picked_.first);  
  e->SetSelection(e->GetView().CreateEmptyView());
}

void SceneMenu::CenterOnAtom()
{
  gfx::EntityP e=dyn_cast<gfx::Entity>(picked_.first);
  gfx::Scene::Instance().SetCenter(picked_.second.GetPos());
}

void SceneMenu::CenterOnResidue()
{
  gfx::Scene::Instance().SetCenter(picked_.second.GetPos());
}

void SceneMenu::CenterOnChain()
{
  mol::ChainHandle chain=picked_.second.GetResidue().GetChain();
  gfx::Scene::Instance().SetCenter(chain.GetGeometricCenter());
}

void SceneMenu::CenterOnView()
{
  mol::ResidueHandle res=picked_.second.GetResidue();
  gfx::Scene::Instance().SetCenter(res.GetGeometricCenter());
}

void SceneMenu::CenterOnSelection()
{
  gfx::EntityP e=dyn_cast<gfx::Entity>(picked_.first);
  if (e->GetSelection() && e->GetSelection().GetAtomCount()>0) {
    geom::Vec3 center=e->GetSelection().GetGeometricCenter();
    gfx::Scene::Instance().SetCenter(center);
  }
}

void SceneMenu::PrepareContextMenu(const QPoint& pos)
{
  center_menu_->setEnabled(picked_.second.IsValid());
  select_menu_->setEnabled(picked_.second.IsValid());
}

void SceneMenu::SetPickPoint(const QPoint& pos)
{
  picked_=gfx::Scene::Instance().PickAtom(pos.x(),pos.y());
}

void SceneMenu::ShowMenu(const QPoint& pos)
{
  this->PrepareContextMenu(pos);
  context_menu_->popup(pos);
}

SceneMenu::~SceneMenu() {

}

}}//ns
