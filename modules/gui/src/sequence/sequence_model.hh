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
#ifndef OST_SEQUENCE_VIEWER_SEQUENCE_MODEL
#define OST_SEQUENCE_VIEWER_SEQUENCE_MODEL

/*
  Author: Stefan Scheuber
 */

#include <QAbstractTableModel>
#include <QItemSelection>

#include <ost/mol/chain_view.hh>

#include <ost/seq/sequence_list.hh>

#include "sequence_model.hh"
#include "view_object.hh"

namespace ost { namespace gui {

class SequenceModel : public QAbstractTableModel
{
  Q_OBJECT

public:
  SequenceModel(QObject *parent = 0);

  void InsertGfxEntity(gfx::EntityP& entity);
  void InsertChain(QString& name, mol::ChainView& view);
  void InsertSequence(QString& name, seq::SequenceHandle& seq);
  void InsertSequences(const QList<QString>& names, seq::SequenceList& list);

  void RemoveGfxEntity(gfx::EntityP& entity);

  QModelIndexList GetModelIndexes(gfx::EntityP& entity, const mol::EntityView& view);
  int GetGlobalRow(ViewObject* obj, int row) const;


  const PainterList& GetPainters(const QModelIndex& index) const;

  // abstract item model interface
  int rowCount(const QModelIndex& parent=QModelIndex()) const;

  int columnCount(const QModelIndex& parent=QModelIndex()) const;

  QVariant data(const QModelIndex& index, int role=Qt::DisplayRole) const;

  QVariant headerData(int section, Qt::Orientation orientation,
                      int role=Qt::DisplayRole) const;

  virtual Qt::ItemFlags flags(const QModelIndex& index=QModelIndex()) const;

public slots:
  void ZoomIn();
  void ZoomOut();
  void DoubleClicked(const QModelIndex& index);
  void SelectionChanged(const QItemSelection& sel, const QItemSelection& desel);

private:
  ViewObject* GetItem(gfx::EntityP& entity);
  ViewObject* GetItem(const QModelIndex& index) const;
  QPair<int, ViewObject*> GetRowWithItem(int row) const;
  QPair<int, ViewObject*> GetRowWithItem(const QModelIndex& index) const;

  QList<ViewObject*> objects_;
  int max_columns;
};


}}

#endif
