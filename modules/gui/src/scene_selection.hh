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
#ifndef OST_GUI_SCENE_SELECTION_HH
#define OST_GUI_SCENE_SELECTION_HH

#include <QObject>
#include <QMenu>
#include <QPoint>

#include <ost/gfx/gfx_node_fw.hh>
#include <ost/gui/module_config.hh>

namespace ost { namespace gui {

class DLLEXPORT_OST_GUI SceneSelection : public QObject {
  Q_OBJECT
public:
  static SceneSelection* Instance();
  gfx::GfxNodeP GetActiveNode(unsigned int pos);
  int GetActiveNodeCount();
  void ShowMenu(const QPoint & p);

public slots:
  void CenterOnObjects();
  void Delete();
  void CopyViews();
  void Select();
  void Show();
  void Hide();

private slots:
  void SetActiveNodes(gfx::NodePtrList nodes);

private:
  SceneSelection();
  gfx::NodePtrList nodes_;
  static SceneSelection* scene_selection_;
  QMenu* menu_;
};

}}

#endif
