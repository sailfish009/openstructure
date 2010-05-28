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

/*
  Author: Stefan Scheuber
 */


#include <QtGui>

#include <ost/mol/mol.hh>
#include <ost/mol/view_op.hh>

#include "sequence_row.hh"
#include "secstr_row.hh"

#include "painter.hh"
#include "background_painter.hh"
#include "seq_secstr_painter.hh"
#include "seq_selection_painter.hh"
#include "seq_text_painter.hh"

#include "alignment_view_object.hh"

namespace ost { namespace gui {

AlignmentViewObject::AlignmentViewObject(const seq::AlignmentHandle& alignment, QObject* parent): SequenceViewObject(parent)
{
  for(int i=0; i<alignment.GetCount();i++){
    seq::SequenceHandle seq_handle = alignment.GetSequence(i).Copy();
    this->AddSequence(seq_handle, seq_handle.GetName().c_str());
  }
}

}}
