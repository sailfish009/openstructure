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
#ifndef OST_GUI_SCENE_WIN_CONTEXT_MENU_HH
#define OST_GUI_SCENE_WIN_CONTEXT_MENU_HH

/*
  Author: Stefan Scheuber
 */

#include <QFlags>
#include <QMap>
#include <QAbstractItemModel>
#include <QTreeView>

#include <ost/gui/module_config.hh>
#include <ost/gui/scene_win/scene_win_model.hh>

namespace ost { namespace gui {

enum ContextActionType
{
  GFX_OBJECT=0x1,
  ENTITY=0x2,
  ENTITY_VIEW=0x4,
  CUSTOM_VIEW=0x8,
  NOT_VISIBLE=0x10,
  NOT_HIDDEN=0x20,
  NOT_SCENE=0x40,
  SINGLE=0x80,
  MULTI=0x100
#if OST_IMG_ENABLED
  ,MAP=0x200
#endif
};
Q_DECLARE_FLAGS(ContextActionTypes, ContextActionType)


class DLLEXPORT_OST_GUI ContextMenu : public QObject {
  Q_OBJECT
public:
  ContextMenu(QTreeView* view, SceneWinModel* model);
  ~ContextMenu(){};

  void ShowMenu(const QPoint& pos);
  void Rename(QModelIndex index);

  void AddAction(QAction* action,ContextActionTypes type);

private slots:
  void AddViewFromEntity();
  void AddView();
  void Rename();
  void DeleteView();

private:
  QMap<QAction*, ContextActionTypes> actions_;

  QTreeView* view_;
  SceneWinModel* model_;
};

}}

Q_DECLARE_OPERATORS_FOR_FLAGS(ost::gui::ContextActionTypes)

#endif
