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
#include <ost/gui/gl_canvas.hh>

#include <ost/gfx/scene.hh>


#include "entity_explorer_model.hh"
#include "entity_explorer.hh"
#include "menu_item.hh"

// Qt includes must come last
#include <QDebug>
#include <QMetaProperty>
#include <QLayout>

namespace ost { namespace gui {

class MenuItem;

EntityExplorer::EntityExplorer(QWidget* p)
  : QTreeView(p) {
  this->setSizePolicy(QSizePolicy(QSizePolicy::MinimumExpanding, 
                                  QSizePolicy::MinimumExpanding));
  EntityExplorerModel* model=new EntityExplorerModel(this);
  this->setModel(model);
  QObject::connect(this, SIGNAL(doubleClicked(const QModelIndex&)),
                   this, SLOT(DoubleClicked(const QModelIndex&)));                   
}

void EntityExplorer::OnAction(QAction* action) {
  
}

void EntityExplorer::DoubleClicked(const QModelIndex& index) {
  MenuItem* item=static_cast<MenuItem*>(index.internalPointer());
  if (AtomViewMenuItem* atom_item=dynamic_cast<AtomViewMenuItem*>(item)) {
    gfx::Scene& scene=gfx::Scene::Instance();
    scene.SetCenter(atom_item->Atom().GetPos());
    scene.RequestRedraw();
  }
}


}} // ns
