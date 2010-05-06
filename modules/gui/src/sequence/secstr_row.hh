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
#ifndef OST_SEQUENCE_VIEWER_SECSTR_ROW
#define OST_SEQUENCE_VIEWER_SECSTR_ROW

/*
  Author: Stefan Scheuber
 */

#include <QObject>
#include <QVarLengthArray>

#include <ost/mol/chain_view.hh>
#include <ost/mol/alg/sec_structure_segments.hh>

#include "sequence_row.hh"

namespace ost { namespace gui {

class SecStrRow : public SequenceRow
{
  Q_OBJECT

public:
  SecStrRow(const QString& name, mol::ChainView& chain, ViewObject* parent);

  virtual QVariant GetData(int column, int role) const;
  virtual void DoubleClicked(int column);

  void SetSequence(seq::SequenceHandle& sequence);
  void SetChain(mol::ChainView& chain);
  const mol::ChainView& GetChain() const;

private:
  mol::ChainView chain_;
  QVarLengthArray<mol::SecStructure> secstr_;
};
}}

#endif
