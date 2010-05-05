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
#include "title_row.hh"

#include "painter.hh"
#include "seq_secstr_painter.hh"
#include "seq_selection_painter.hh"
#include "seq_text_painter.hh"
#include "tick_painter.hh"

#include "view_object.hh"

namespace ost { namespace gui {

ViewObject::ViewObject(seq::SequenceList& sequences, const QList<QString>& names, QObject *parent): QObject(parent)
{
  if(names.size() == sequences.GetCount()){
    for(int i=0; i<sequences.GetCount(); i++){
      seq::SequenceHandle seq = sequences[i];
      this->AddSequence(seq, names[i]);
    }
  }
}

ViewObject::ViewObject(seq::SequenceHandle& sequence, const QString& name, QObject *parent): QObject(parent), entity_(gfx::EntityP())
{
  this->AddSequence(sequence, name);
}

ViewObject::ViewObject(mol::ChainView& chain, const QString& name, QObject *parent): QObject(parent), entity_(gfx::EntityP())
{
  this->AddChain(chain, name);
}

ViewObject::ViewObject(gfx::EntityP& entity, QObject* parent): QObject(parent), entity_(entity)
{
  mol::EntityView view =entity->GetView();
  for (mol::ChainViewList::const_iterator c=view.GetChainList().begin(),
       e1=view.GetChainList().end(); c!=e1; ++c) {
    mol::ChainView chain=*c;
    QString name = QString(entity->GetName().c_str());
    if (chain.GetName()!="" && chain.GetName()!=" ") {
      name= name + " ("+chain.GetName().c_str()+")";
    }
    this->AddChain(chain, name);
  }
}

ViewObject::ViewObject(QObject* parent): QObject(parent)
{
  TitleRow* new_row = new TitleRow(this);
  Painter* p = new TickPainter(this);
  new_row->InsertPainter(p);
  rows_.append(new_row);
}

void ViewObject::InsertRow(int pos, BaseRow* row)
{
  if(pos >= 0 && pos <= rows_.size()){
    rows_.insert(pos,row);
  }
}

void ViewObject::RemoveRow(BaseRow* row)
{
  rows_.removeAll(row);
}

BaseRow* ViewObject::GetRow(int pos)
{
   if(pos >= 0 && pos < rows_.size()){
     return rows_[pos];
   }
   return NULL;
}

int ViewObject::GetRowCount()
{
  return rows_.size();
}

void ViewObject::AddSequence(seq::SequenceHandle& sequence, const QString& name)
{
  SequenceRow* new_row = new SequenceRow(name, sequence, this);
  Painter* p = new SeqSelectionPainter(this);
  new_row->InsertPainter(p);
  p = new SeqTextPainter(this);
  new_row->InsertPainter(p);
  rows_.append(new_row);
}

void ViewObject::AddChain(mol::ChainView& chain, const QString& name)
{
  SecStrRow* new_row = new SecStrRow(name, chain, this);
  Painter* p = new SeqSelectionPainter(this);
  new_row->InsertPainter(p);
  p = new SeqSecStrPainter(this);
  new_row->InsertPainter(p);
  p = new SeqTextPainter(this);
  new_row->InsertPainter(p);
  rows_.append(new_row);
}

void ViewObject::AttachGfxObject(gfx::EntityP& ent)
{
  entity_ = ent;
}

gfx::EntityP& ViewObject::GetGfxObject()
{
  return entity_;
}

void ViewObject::SetSelection(int row, const QSet<int>& added, const QSet<int>& removed)
{
  if(SequenceRow* sequence_row = qobject_cast<SequenceRow*>(rows_[row])){
    sequence_row->SetSelection(added,removed);
  }
}

QVariant ViewObject::GetData(int row, int column, int role)
{
  if(row<0 || row >= rows_.size())return QVariant();

  return rows_[row]->GetData(column,role);
}

int ViewObject::GetMaxColumnCount() const
{
  int columns = 0;
  for(int i = 0; i < rows_.size(); i++){
    int col_length = rows_[i]->GetColumnCount();
    if(columns < col_length){
      columns = col_length;
    }
  }
  return columns;
}

bool ViewObject::SetData(int row, int column, const QVariant& value, int role)
{
  if(row<0 || row >= rows_.size())return false;

  return rows_[row]->SetData(column, value, role);
}

void ViewObject::DoubleClicked(int row, int column)
{
  if(row>=0 || row < rows_.size()){
    rows_[row]->DoubleClicked(column);
  }
}

QMap<int, QList<int> > ViewObject::GetIndexesForView(const mol::EntityView& view)
{
  if(view.GetChainCount()==0){
    return QMap<int, QList<int> >();
  }
  else{
    QMap<int, QList<int> > selected_indexes;
    for(int i=0; i< rows_.size(); i++){
      if(SecStrRow* secstr_row = qobject_cast<SecStrRow*>(rows_[i])){
        mol::ChainView dst_chain=(secstr_row->GetChain());
        seq::SequenceHandle seq = secstr_row->GetSequence();
        if (mol::ChainView src_chain=view.FindChain(dst_chain.GetName())) {
          // for each residue in the selection deduce index in sequence
          for (mol::ResidueViewList::const_iterator j=src_chain.GetResidueList().begin(),
             e2=src_chain.GetResidueList().end(); j!=e2; ++j) {
            mol::ResidueView dst_res=dst_chain.FindResidue(j->GetHandle());
            assert(dst_res.IsValid());
            int p=dst_res.GetIndex()+1;
            assert(p>=0 && p<=seq.GetLength());
            selected_indexes[i].append(p);
          }
        }
      }
    }
    return selected_indexes;
  }
}

Qt::ItemFlags ViewObject::Flags(int row, int column) const
{
  if(row<0 || row >= rows_.size())return Qt::NoItemFlags;

  return rows_[row]->Flags(column);
}

}}
