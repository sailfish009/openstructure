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

#include <ost/seq/sequence_list.hh>

#include "sequence_model.hh"
#include "view_object.hh"

namespace ost { namespace gui {

class SequenceModel : public QAbstractTableModel
{
  Q_OBJECT

public:
  SequenceModel(QObject *parent = 0);

  void InsertSequence(QString& name, seq::SequenceHandle& seq);
  void InsertSequences(QString& name, seq::SequenceList& list);

  void RemoveSequence(QString& name);

  ViewObject* GetObject(QString& name);

  QPair<int, ViewObject*> GetItem(const QModelIndex& index) const;

  // abstract item model interface
  int rowCount(const QModelIndex& parent=QModelIndex()) const;

  int columnCount(const QModelIndex& parent=QModelIndex()) const;

  QVariant data(const QModelIndex& index, int role=Qt::DisplayRole) const;

  QVariant headerData(int section, Qt::Orientation orientation,
                      int role=Qt::DisplayRole) const;

  virtual Qt::ItemFlags flags(const QModelIndex& index=QModelIndex()) const;

private:
  QList<ViewObject*> objects_;
};


}}

#endif
