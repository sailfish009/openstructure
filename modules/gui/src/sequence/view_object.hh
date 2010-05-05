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
#ifndef OST_SEQUENCE_VIEWER_VIEW_OBJECT
#define OST_SEQUENCE_VIEWER_VIEW_OBJECT

/*
  Author: Stefan Scheuber
 */

#include <QObject>
#include <QPair>
#include <QList>
#include <QVarLengthArray>
#include <QFont>
#include <QSize>

#include <ost/mol/alg/sec_structure_segments.hh>
#include <ost/mol/entity_handle.hh>

#include <ost/gfx/entity.hh>

#include <ost/seq/sequence_list.hh>

#include "base_row.hh"


namespace ost { namespace gui {

class ViewObject : public QObject
{
  Q_OBJECT


public:
  ViewObject(seq::SequenceList& sequences, const QList<QString>& names, QObject* parent = 0);
  ViewObject(seq::SequenceHandle& sequence, const QString& name, QObject* parent = 0);
  ViewObject(mol::ChainView& chain, const QString& name, QObject* parent = 0);
  ViewObject(gfx::EntityP& entity, QObject* parent = 0);
  ViewObject(QObject* parent = 0);

  void InsertRow(int pos, BaseRow* row);
  void RemoveRow(BaseRow* row);

  BaseRow* GetRow(int pos);
  int GetRowCount();
  int GetMaxColumnCount() const;

  void AddSequence(seq::SequenceHandle& sequence, const QString& name=QString());
  void AddChain(mol::ChainView& chain, const QString& name=QString());

  void AttachGfxObject(gfx::EntityP& ent);
  gfx::EntityP& GetGfxObject();

  void SetSelection(int row, const QSet<int>& added, const QSet<int>& removed);

  QVariant GetData(int row, int column, int role);
  bool SetData(int row, int column, const QVariant& value, int role);
  Qt::ItemFlags Flags(int row, int column) const;

  void DoubleClicked(int row, int column);

  QMap<int, QList<int> > GetIndexesForView(const mol::EntityView& view);

private:
  QList<BaseRow*> rows_;
  gfx::EntityP entity_;
};


}}

#endif
