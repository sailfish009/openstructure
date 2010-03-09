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

#include <QMap>
#include <QAbstractItemModel>
#include <QTreeView>

#include <ost/gui/module_config.hh>
#include <ost/gui/scene_win/scene_win_model.hh>

namespace ost { namespace gui {

class DLLEXPORT_OST_GUI ContextMenu : public QObject {
  Q_OBJECT
public:
  ContextMenu(QTreeView* view, SceneWinModel* model);
  ~ContextMenu(){};

  void ShowMenu(const QPoint& pos);
  void Rename(QModelIndex index);

private slots:
  void AddViewFromEntity();
  void AddView();
  void Rename();
  void DeleteView();


private:
  QTreeView* view_;
  SceneWinModel* model_;
};

}}

#endif
