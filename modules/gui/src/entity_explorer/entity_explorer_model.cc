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
#include <ost/gfx/scene.hh>
#include <ost/gfx/entity.hh>

#include "entity_explorer_model.hh"
#include "menu_item.hh"


#include <QDebug>

namespace ost { namespace gui {
  
EntityExplorerModel::EntityExplorerModel(QObject* parent)
  : QAbstractItemModel(parent), root_(new MenuItem(NULL)) {
  gfx::Scene::Instance().AttachObserver(this);
}

EntityExplorerModel::~EntityExplorerModel()
{
  gfx::Scene::Instance().DetachObserver(this);
  delete root_;
}

QVariant EntityExplorerModel::data(const QModelIndex& index, int role) const {
  if (role==Qt::DisplayRole) {
    MenuItem* item=static_cast<MenuItem*>(index.internalPointer());
    if (item)
      return item->Data();
  }
  return QVariant();
}

Qt::ItemFlags EntityExplorerModel::flags(const QModelIndex& index) const {
  if (index.isValid())
    return Qt::ItemIsEnabled|Qt::ItemIsSelectable|Qt::ItemIsUserCheckable;
  return Qt::ItemIsEnabled;
}

QVariant EntityExplorerModel::headerData(int section, Qt::Orientation orientation,
                                    int role) const {
  return QVariant(QString("Name"));
}


QModelIndex EntityExplorerModel::index(int row, int column,  
                                  const QModelIndex& parent) const {
  MenuItem* item=root_;
  if (parent.isValid())
    item=static_cast<MenuItem*>(parent.internalPointer());
  assert(item);
  return createIndex(row, column, (item->Children())[row]);
}

QModelIndex EntityExplorerModel::parent(const QModelIndex& index) const {
  if (!index.isValid())
    return QModelIndex();
  MenuItem* item=static_cast<MenuItem*>(index.internalPointer());
  assert(item!=NULL);
  if (item->Parent()==root_) {
    return QModelIndex();    
  }
  return createIndex(item->Parent()->Row(), 0, item->Parent());
}

int EntityExplorerModel::rowCount(const QModelIndex& parent) const {
  MenuItem* item=root_;
  if (parent.isValid())
    item=static_cast<MenuItem*>(parent.internalPointer());
  assert(item!=NULL);    
  return item->Children().size();
}

int EntityExplorerModel::columnCount(const QModelIndex& parent) const {
  return 1;
}

void EntityExplorerModel::ObjectAdded(const gfx::GfxObjP& object) {
#if 0
  // needs new GfxNodeVisitor implementation
  gfx::Scene& scene=gfx::Scene::Instance();
  this->beginInsertRows(QModelIndex(),scene.SceneObjects().size()-1, 
                        scene.SceneObjects().size()-1);
  root_->Children().push_back(new GfxObjMenuItem(root_, object));
  this->endInsertRows();
#endif
}

mol::EntityView EntityExplorerModel::GetEntity(const QModelIndex& index) const {
#if 0
  // needs new GfxNodeVisitor implementation
  gfx::GfxObjList& ol=gfx::Scene::Instance().SceneObjects();
  gfx::EntityP e=dyn_cast<gfx::Entity>(ol[index.row()]);
  if (e)
    return e->GetView();
  else
#endif
    return mol::EntityView();
}

mol::ChainView EntityExplorerModel::GetChain(const QModelIndex& index) const {
  return (this->GetEntity(index.parent()).GetChainList())[index.row()];
}

mol::ResidueView EntityExplorerModel::GetResidue(const QModelIndex& index) const {
  return mol::ResidueView();
}


}}


