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
#ifndef OST_SEQUENCE_VIEWER_SEQUENCE_ROW
#define OST_SEQUENCE_VIEWER_SEQUENCE_ROW

/*
  Author: Stefan Scheuber
 */

#include <QObject>

#include <ost/seq/sequence_handle.hh>

#include "base_row.hh"

namespace ost { namespace gui {

class ViewObject;

class SequenceRow : public BaseRow
{
  Q_OBJECT

public:
  SequenceRow(const QString& name, seq::SequenceHandle& sequence, ViewObject* parent);
  SequenceRow(const QString& name, ViewObject* parent);

  virtual int GetColumnCount() const;

  virtual QVariant GetData(int column, int role) const;
  virtual Qt::ItemFlags Flags(int column) const;
  virtual void DoubleClicked(int column);

  void SetName(const QString& name);
  const QString& GetName() const;

  virtual void SetSequence(seq::SequenceHandle& sequence);
  const seq::SequenceHandle& GetSequence() const;

  void SetSelection(const QSet<int>& added, const QSet<int>& removed);

private:
  QString name_;
  seq::SequenceHandle sequence_;
};

}}

#endif
