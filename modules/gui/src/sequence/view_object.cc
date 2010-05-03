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

#include "painter.hh"
#include "seq_text_painter.hh"

#include "view_object.hh"

namespace ost { namespace gui {

ViewObject::ViewObject(seq::SequenceList& sequences, const QString& name, QObject *parent): QObject(parent), name_(name)
{
  for(int i=0; i<sequences.GetCount(); i++){
    seq::SequenceHandle seq = sequences[i];
    this->AddSequence(seq);
  }
}

ViewObject::ViewObject(seq::SequenceHandle& sequence, const QString& name, QObject *parent): QObject(parent), name_(name)
{
  this->AddSequence(sequence);
}

void ViewObject::InsertRow(int pos, Row* row)
{
  if(pos >= 0 && pos <= rows_.size()){
    seq::SequenceHandle sequence = seq::SequenceHandle();
    QPair<Row*, seq::SequenceHandle> pair(row,sequence);
    rows_.insert(pos,pair);
  }
}

void ViewObject::RemoveRow(Row* row)
{
  QList<int> rows_to_delete;
  for (int i = 0; i < rows_.size(); ++i){
    if(rows_[i].first == row){
      rows_to_delete.append(i);
    }
  }
  for (int i= 0; i < rows_to_delete.size(); ++i){
    rows_.removeAt(rows_to_delete[i]);
  }
}

Row* ViewObject::GetRow(int pos)
{
   if(pos >= 0 && pos < rows_.size()){
     return rows_[pos].first;
   }
   return NULL;
}

int ViewObject::GetRowCount()
{
  return rows_.size();
}

void ViewObject::AddSequence(seq::SequenceHandle& sequence)
{
  Row* new_row = new Row(this);
  Painter* p = new SeqTextPainter(this);
  new_row->InsertPainter(p,0);
  QPair<Row*, seq::SequenceHandle> pair(new_row,sequence);
  rows_.append(pair);
}

QVariant ViewObject::GetData(int row, int column, int role)
{
  if(row<0 || row >= rows_.size())return QVariant();

  if(column<0 || column >= this->GetMaxColumnCount())return QVariant();

  if(column == 0 && row == 0) {
    if (role == Qt::DisplayRole){
      return QVariant(this->GetName());
    }
  }
  else if(column > 0) {
    if (role==Qt::DisplayRole) {
      return QVariant(QString(rows_[row].second.GetOneLetterCode(column - 1)));
    }
  }
  return QVariant();
}

int ViewObject::GetMaxColumnCount() const
{
  int columns = 0;
  for(int i = 0; i < rows_.size(); i++){
    int col_length = rows_[i].second.GetLength();
    if(columns < col_length){
      columns = rows_[i].second.GetLength() + 1;
    }
  }
  return columns;
}

bool ViewObject::SetData(int column, const QVariant& value, int role)
{
  return false;
}

Qt::ItemFlags ViewObject::Flags(int row, int column) const
{
  if(row<0 || row >= rows_.size())return Qt::NoItemFlags;

  if(column<0 || column >= this->GetMaxColumnCount())return Qt::NoItemFlags;

  if(column==0){
    return Qt::ItemIsSelectable|Qt::ItemIsEnabled;
  }
  else if(column>0){
    return Qt::ItemIsSelectable|Qt::ItemIsEnabled;
  }
  return Qt::NoItemFlags;
}

const QString& ViewObject::GetName() const
{
 return name_;
}

void ViewObject::SetName(const QString& name)
{
  name_ = name;
}

}}
