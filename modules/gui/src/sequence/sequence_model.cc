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

#include <QMap>
#include <QMapIterator>

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

void SequenceModel::InsertChain(QString& name, mol::ChainView& view){
  int cols = this->columnCount();
  int new_cols = view.GetResidueCount();
  this->beginInsertRows(QModelIndex(),this->rowCount(),this->rowCount());
  objects_.append(new ViewObject(view, name, this));
  if(new_cols > cols){
    this->beginInsertColumns(QModelIndex(), cols, new_cols);
    this->endInsertColumns();
  }
  this->endInsertRows();
}

void SequenceModel::InsertSequences(const QList<QString>& names, seq::SequenceList& list){
  this->beginInsertRows(this->index(this->rowCount(),0),this->rowCount(),this->rowCount()+list.GetCount());
  objects_.append(new ViewObject(list, names, this));
  this->endInsertRows();
}

void SequenceModel::InsertGfxEntity(gfx::EntityP& ent){
  mol::EntityView view=ent->GetView();
  int size = view.GetChainList().size();
  int cols = this->columnCount();
  this->beginInsertRows(QModelIndex(),this->rowCount(),this->rowCount()+size);
  ViewObject* obj = new ViewObject(ent, this);
  objects_.append(obj);
  int new_cols = obj->GetMaxColumnCount();
  if(new_cols > cols){
    this->beginInsertColumns(QModelIndex(), cols, new_cols);
    this->endInsertColumns();
  }
  this->endInsertRows();
}

void SequenceModel::RemoveGfxEntity(gfx::EntityP& entity){
  ViewObject* obj = this->GetObject(entity);
  if(obj){
    int index = objects_.indexOf(obj);
    this->beginRemoveRows(QModelIndex(),index,index+obj->GetRowCount()-1);
    int cols_before = this->columnCount();
    objects_.removeAt(index);
    this->endRemoveRows();
    int cols = this->columnCount();
    if(cols_before>cols){
      this->beginRemoveColumns(QModelIndex(), cols, cols_before-1);
      this->endRemoveColumns();
    }
  }
}

ViewObject* SequenceModel::GetObject(gfx::EntityP& entity){
  if(entity != NULL){
    for (int i = 0 ; i< objects_.size(); i++){
      if(entity == objects_[i]->GetGfxObject()){
        return objects_[i];
      }
    }
  }
  return NULL;
}

const PainterList& SequenceModel::GetPainters(const QModelIndex& index) const{
  QPair<int, ViewObject*> pair = this->GetRowWithItem(index);
  if(pair.second){
    pair.second->GetRow(pair.first);
    return pair.second->GetRow(pair.first)->GetPainters();
  }
  else{
    assert(false);
  }
}

QPair<int, ViewObject*> SequenceModel::GetRowWithItem(int row) const{
  if(!objects_.isEmpty()){
    int rows = 0;
    int i = -1;
    int last_row = 0;
    while (rows <= row && i < objects_.size()){
      i++;
      last_row =objects_[i]->GetRowCount();
      rows += last_row;
    }
    int sub_index = row - (rows-last_row);
    return QPair<int, ViewObject*>(sub_index, objects_[i]);
  }
  return QPair<int, ViewObject*>(-1, NULL);
}

QPair<int, ViewObject*> SequenceModel::GetRowWithItem(const QModelIndex& index) const{
  return this->GetRowWithItem(index.row());
}

ViewObject* SequenceModel::GetItem(const QModelIndex& index) const
{
  return this->GetRowWithItem(index).second;
}

void SequenceModel::SelectionChanged(const QItemSelection& sel, const QItemSelection& desel)
{
  QMap<int,QPair<QSet<int>,QSet<int> > > sel_map;
  const QModelIndexList& sel_indexes = sel.indexes();
  for(int i =0; i< sel_indexes.size(); i++){
     sel_map[sel_indexes[i].row()].first.insert(sel_indexes[i].column());
  }

  const QModelIndexList& desel_indexes = desel.indexes();
  for(int i =0; i< desel_indexes.size(); i++){
     sel_map[desel_indexes[i].row()].second.insert(desel_indexes[i].column());
  }

  QMapIterator< int,QPair<QSet<int>,QSet<int> > > i(sel_map);
  while (i.hasNext()) {
    i.next();
    QPair<int, ViewObject*> item = this->GetRowWithItem(i.key());
    item.second->SetSelection(item.first,i.value().first, i.value().second);
  }
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
      cols = max_col;
  }
  return cols;
}

QVariant SequenceModel::data(const QModelIndex& index, int role) const
{
  QPair<int, ViewObject*> item = this->GetRowWithItem(index);
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
  QPair<int, ViewObject*> item = GetRowWithItem(index);
  if(item.second){
    return item.second->Flags(item.first, index.column());
  }
  return QAbstractItemModel::flags(index);
}

}}
