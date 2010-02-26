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
#ifndef OST_GUI_SCENE_WIN_SCENE_WIN_MODEL_HH
#define OST_GUI_SCENE_WIN_SCENE_WIN_MODEL_HH

/*
  Author: Stefan Scheuber, Marco Biasini, Ansgar Philippsen
 */

#include <QAbstractItemModel>

#include <ost/gfx/gfx_node_fw.hh>
#include <ost/gfx/scene_observer.hh>

#include <ost/gui/module_config.hh>
#include <ost/gui/scene_win/scene_node.hh>

namespace ost { namespace gui {

/// \brief data model for scene win
class DLLEXPORT_OST_GUI SceneWinModel : public QAbstractItemModel,
                       public gfx::SceneObserver {
  Q_OBJECT
public:
  SceneWinModel(QObject* parent=NULL);
  ~SceneWinModel();

  gfx::NodePtrList GetGfxNodes(QModelIndexList indexes);

  virtual void Update();

  // abstract item model interface
  QModelIndex index(int row, int col, const QModelIndex& parent = QModelIndex()) const;

  QModelIndex parent(const QModelIndex& index) const;  

  int rowCount(const QModelIndex& parent=QModelIndex()) const;

  int columnCount(const QModelIndex& parent=QModelIndex()) const;

  QVariant data(const QModelIndex& index, int role=Qt::DisplayRole) const;

  QVariant headerData(int section, Qt::Orientation orientation, 
                      int role=Qt::DisplayRole) const; 

  virtual Qt::ItemFlags flags(const QModelIndex& index=QModelIndex()) const;
  
  virtual bool setData(const QModelIndex& index=QModelIndex(),
                       const QVariant& value=QVariant(), 
                       int role=Qt::DisplayRole);

  // scene observer interface
  virtual void NodeAdded(const gfx::GfxNodeP& node);
  virtual void NodeRemoved(const gfx::GfxNodeP& node);

private:
  SceneNode* GetItem(const QModelIndex &index) const;

  SceneNode* root_node_;
  SceneNode* scene_node_;
};

}}

#endif
