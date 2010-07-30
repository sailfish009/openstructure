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
#ifndef OST_SEQUENCE_VIEWER_SEQUENCE_VIEWER
#define OST_SEQUENCE_VIEWER_SEQUENCE_VIEWER

/*
  Author: Stefan Scheuber
 */

#include <QWidget>
#include <QActionGroup>
#include <QToolBar>

#include <ost/seq/alignment_handle.hh>

#include <ost/gfx/scene.hh>
#include <ost/gfx/gfx_object.hh>

#include <ost/gui/widget.hh>

#include <ost/gui/module_config.hh>

#include "sequence_search_bar.hh"
#include "sequence_model.hh"
#include "sequence_table_view.hh"

namespace ost { namespace gui {

/// \brief QTableView with first column not moving
class DLLEXPORT_OST_GUI SequenceViewer : public Widget, public gfx::SceneObserver  {
  Q_OBJECT
public:
  SequenceViewer(bool stand_alone=true, QWidget* parent=NULL);
  ~SequenceViewer();

  virtual void SelectionChanged(const gfx::GfxObjP& o, const mol::EntityView& view);

  virtual void AddEntity(const gfx::EntityP& entity);
  virtual void RemoveEntity(const gfx::EntityP& entity);

  virtual void AddAlignment(const seq::AlignmentHandle& alignment);
  virtual void RemoveAlignment(const seq::AlignmentHandle& alignment);

  virtual bool Restore(const QString&){return true;};
  virtual bool Save(const QString&){return true;};

  virtual const QStringList& GetDisplayModes();
  virtual const QStringList& GetDisplayModes(const seq::AlignmentHandle& alignment);
  virtual const QStringList& GetDisplayModes(const gfx::EntityP& entity);

  virtual const QString& GetCurrentDisplayMode();
  virtual const QString& GetCurrentDisplayMode(const seq::AlignmentHandle& alignment);
  virtual const QString& GetCurrentDisplayMode(const gfx::EntityP& entity);

  virtual ActionList GetActions();

public slots:
  void ChangeDisplayMode(const QString&);
  void ChangeDisplayMode(const seq::AlignmentHandle&, const QString&);
  void ChangeDisplayMode(const gfx::EntityP&, const QString&);
  void DisplayMenu();
  //internal
  void OnSearchBarUpdate(const QString&, bool, const QString&);

private:
  virtual void NodeAdded(const gfx::GfxNodeP& node);
  virtual void NodeRemoved(const gfx::GfxNodeP& node);
  void FitToContents();
  void InitActions();
  void InitView();
  void InitSearchBar();
  void InitMenuBar();
  void UpdateSearchBar();
  void SelectList(const QModelIndexList& list);
  QToolBar* toolbar_;
  SeqSearchBar* seq_search_bar_;
  SequenceModel* model_;
  SequenceTableView* seq_table_view_;

  ActionList action_list_;

  QActionGroup* display_mode_actions_;

private slots:
  void ChangeDisplayMode();
  void FindInSequence();
  void SelectionModelChanged(const QItemSelection&, const QItemSelection&);
  void DoubleClicked(const QModelIndex& index);
  void MouseWheelEvent(QWheelEvent* event);
  void CopyEvent(QKeyEvent* event);

};

}}

#endif
