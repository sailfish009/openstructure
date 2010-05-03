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
#ifndef OST_SEQUENCE_VIEWER_SEQUENCE_TABLE_VIEW
#define OST_SEQUENCE_VIEWER_SEQUENCE_TABLE_VIEW

/*
  Author: Stefan Scheuber
 */

#include <QTableView>

#include <ost/gui/module_config.hh>

#include "sequence_delegate.hh"

namespace ost { namespace gui {

/// \brief QTableView with first column not moving
class DLLEXPORT_OST_GUI SequenceTableView : public QTableView {
  Q_OBJECT
public:
  SequenceTableView(QAbstractItemModel * model);
  ~SequenceTableView();

public slots:
  void columnCountChanged(const QModelIndex& index, int old_count, int new_count);
  void resizeColumnsToContents();
  QTableView* GetFirstRow();

protected:
  virtual void mouseDoubleClickEvent(QMouseEvent* event);
  virtual void mouseReleaseEvent(QMouseEvent* event);
  virtual void resizeEvent(QResizeEvent *event);
  virtual QModelIndex moveCursor(CursorAction cursorAction, Qt::KeyboardModifiers modifiers);
  void scrollTo (const QModelIndex & index, ScrollHint hint = EnsureVisible);

private slots:
  void ResizeWidth(int index, int, int size);
  void ResizeHeight(int index, int, int size);

private:
  QTableView* column_not_move_;
  SequenceDelegate* delegate_;
  QModelIndex last_double_click_;
  void updateNotMoveColumn();
};

}}

#endif
