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
#ifndef OST_GUI_ENTITY_EXPLORER_MODEL_HH
#define OST_GUI_ENTITY_EXPLORER_MODEL_HH


#include <ost/mol/mol.hh>

#include <ost/gfx/scene_observer.hh>
#include <ost/gfx/gfx_object.hh>
#include <ost/gui/module_config.hh>

// Qt includes must come last
#include <QAbstractItemModel>


namespace ost { namespace gui {

class MenuItem;

namespace lvl {
  typedef enum { Root=0, Entity, Chain, Residue, Atom } Level;
}

/// \brief data model for the scene menu tree view
class DLLEXPORT_OST_GUI EntityExplorerModel : public QAbstractItemModel,
                                         public gfx::SceneObserver {
  Q_OBJECT
public:
  EntityExplorerModel(QObject* parent);
  ~EntityExplorerModel();
  QVariant data(const QModelIndex& index, int role) const;
  Qt::ItemFlags flags(const QModelIndex& index) const;
  QVariant headerData(int section, Qt::Orientation orientation,
                      int role=Qt::DisplayRole) const;
  QModelIndex index(int row, int column,
                    const QModelIndex& parent=QModelIndex()) const;
  QModelIndex parent(const QModelIndex& index) const;
  int rowCount(const QModelIndex& parent=QModelIndex()) const;
  int columnCount(const QModelIndex& parent=QModelIndex()) const;
public:
  virtual void ObjectAdded(const gfx::GfxObjP& object);
private:
  mol::EntityView GetEntity(const QModelIndex& index) const;
  mol::ChainView GetChain(const QModelIndex& index) const;  
  mol::ResidueView GetResidue(const QModelIndex& index) const;
  MenuItem*  root_;
};

}}

#endif
