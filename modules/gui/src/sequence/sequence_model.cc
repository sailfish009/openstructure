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

#include "sequence_model.hh"

namespace ost { namespace gui {

SequenceModel::SequenceModel(QObject *parent)
    : QAbstractTableModel(parent)
{
}

void SequenceModel::InsertSequence(QString& name, seq::SequenceHandle& seq){
  int cols = this->columnCount();
  int new_cols = seq.GetLength();
  this->beginInsertRows(QModelIndex(),this->rowCount(),this->rowCount());
  objects_.append(new ViewObject(seq, name, this));
  if(new_cols > cols){
    this->beginInsertColumns(QModelIndex(), cols, new_cols);
    this->endInsertColumns();
  }
  this->endInsertRows();
}

void SequenceModel::InsertSequences(QString& name, seq::SequenceList& list){
  this->beginInsertRows(this->index(this->rowCount(),0),this->rowCount(),this->rowCount()+list.GetCount());
  objects_.append(new ViewObject(list, name, this));
  this->endInsertRows();
}

void SequenceModel::RemoveSequence(QString& name){
  ViewObject* obj = this->GetObject(name);
  if(obj){
    int index = objects_.indexOf(obj);
    this->beginRemoveRows(QModelIndex(),index,index);
    objects_.removeAt(index);
    this->endRemoveRows();
  }
}

ViewObject* SequenceModel::GetObject(QString& name){
  for (int i = 0 ; i< objects_.size(); i++){
    if(name == objects_[i]->GetName()){
      return objects_[i];
    }
  }
  return NULL;
}

const PainterList& SequenceModel::GetPainters(const QModelIndex& index) const{
  QPair<int, ViewObject*> pair = this->GetItem(index);
  return pair.second->GetRow(pair.first)->GetPainters();
}

QPair<int, ViewObject*> SequenceModel::GetItem(const QModelIndex& index) const{
  if(!objects_.isEmpty()){
    int ind_row = index.row();
    int rows = 0;
    int i = 0;
    while (i < objects_.size() && rows < ind_row){
      i++;
      rows += objects_[i]->GetRowCount();
    }
    int sub_index = ind_row - rows;
    return QPair<int, ViewObject*>(sub_index, objects_[i]);
  }
  return QPair<int, ViewObject*>(-1, NULL);
}

int SequenceModel::rowCount(const QModelIndex& parent) const
{
  int rows = 0;
  for (int i = 0; i<objects_.size(); i++){
    rows += objects_[i]->GetRowCount();
  }
  return rows;
}

int SequenceModel::columnCount(const QModelIndex& parent) const
{
  int cols = 0;
  for (int i = 0; i<objects_.size(); i++){
    int max_col = objects_[i]->GetMaxColumnCount();
    if(max_col >= cols)
      cols = max_col+1;
  }
  return cols;
}

QVariant SequenceModel::data(const QModelIndex& index, int role) const
{
  QPair<int, ViewObject*> item = this->GetItem(index);
  if(!item.second) return QVariant();
  QVariant data = item.second->GetData(item.first,index.column(),role);
  return data;
}

QVariant SequenceModel::headerData(int section, Qt::Orientation orientation,
                                    int role) const
{
  if (orientation == Qt::Horizontal && role == Qt::DisplayRole) {
    return QVariant("");
  }
  return QVariant();
}

Qt::ItemFlags SequenceModel::flags(const QModelIndex& index) const
{
  QPair<int, ViewObject*> item = GetItem(index);
  if(item.second){
    return item.second->Flags(item.first, index.column());
  }

  return QAbstractItemModel::flags(index);
}

}}
