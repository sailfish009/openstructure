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
class DLLEXPORT_OST_GUI SequenceViewerV2 : public Widget, public gfx::SceneObserver  {
  Q_OBJECT
public:
  SequenceViewerV2(bool listen_scene=true, QWidget* parent=NULL);
  ~SequenceViewerV2();

  virtual void NodeAdded(const gfx::GfxNodeP& node);
  virtual void NodeRemoved(const gfx::GfxNodeP& node);
  virtual void SelectionChanged(const gfx::GfxObjP& o, const mol::EntityView& view);

  virtual void AddAlignment(const seq::AlignmentHandle& alignment);
  virtual void RemoveAlignment(const seq::AlignmentHandle& alignment);

  virtual bool Restore(const QString&){return true;};
  virtual bool Save(const QString&){return true;};

public slots:
  void ChangeDisplayMode(const QString&);
  void ChangeDisplayMode(const seq::AlignmentHandle&, const QString&);
  void ChangeDisplayMode(const gfx::EntityP&, const QString&);
  //internal
  void OnSearchBarUpdate(const QString&, bool, const QString&);

private:
  void UpdateSearchBar();
  void SelectList(const QModelIndexList& list);
  SeqSearchBar* seq_search_bar_;
  SequenceModel* model_;
  SequenceTableView* seq_table_view_;

private slots:
  /// \brief show sequence search bar
  void FindInSequence();
  void SelectionModelChanged(const QItemSelection&, const QItemSelection&);
  void DoubleClicked(const QModelIndex& index);
  void MouseWheelEvent(QWheelEvent* event);
  void CopyEvent(QKeyEvent* event);

};

}}

#endif
