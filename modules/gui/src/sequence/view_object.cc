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

#include "painter.hh"
#include "seq_secstr_painter.hh"
#include "seq_selection_painter.hh"
#include "seq_text_painter.hh"

#include "view_object.hh"

namespace ost { namespace gui {

ViewObject::ViewObject(seq::SequenceList& sequences, const QList<QString>& names, QObject *parent): QObject(parent)
{
  if(names.size() == sequences.GetCount()){
    for(int i=0; i<sequences.GetCount(); i++){
      seq::SequenceHandle seq = sequences[i];
      this->AddSequence(seq, names[i]);
    }
    this->Init();
  }
}

ViewObject::ViewObject(seq::SequenceHandle& sequence, const QString& name, QObject *parent): QObject(parent), entity_(gfx::EntityP())
{
  this->AddSequence(sequence, name);
  this->Init();
}

ViewObject::ViewObject(mol::ChainView& chain, const QString& name, QObject *parent): QObject(parent), entity_(gfx::EntityP())
{
  this->AddChain(chain, name);
  this->Init();
}

ViewObject::ViewObject(gfx::EntityP& entity, QObject* parent): QObject(parent), entity_(entity){
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
  this->Init();
}

void ViewObject::Init()
{
  font_ = QFont("Courier",30);
  QFontMetrics metrics = QFontMetrics(font_);
  default_size_=QSize(metrics.boundingRect('W').width(),metrics.boundingRect('|').height());
  default_cell_size_ = QSize(metrics.boundingRect('W').width()+2,metrics.boundingRect('|').height()*2);
}

void ViewObject::InsertRow(int pos, Row* row)
{
  if(pos >= 0 && pos <= rows_.size()){
    ListEntry entry(row);
    rows_.insert(pos,entry);
  }
}

void ViewObject::RemoveRow(Row* row)
{
  QList<int> rows_to_delete;
  for (int i = 0; i < rows_.size(); ++i){
    if(rows_[i].row == row){
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
     return rows_[pos].row;
   }
   return NULL;
}

int ViewObject::GetRowCount()
{
  return rows_.size();
}

void ViewObject::AddSequence(seq::SequenceHandle& sequence, const QString& name)
{
  Row* new_row = new Row(this);
  Painter* p = new SeqSelectionPainter(this);
  new_row->InsertPainter(p);
  p = new SeqTextPainter(this);
  new_row->InsertPainter(p);
  ListEntry entry(new_row, name, sequence);
  rows_.append(entry);
}

void ViewObject::AddChain(mol::ChainView& chain, const QString& name)
{
  String seq_str;
  seq_str.reserve(chain.GetResidueCount());
  for (mol::ResidueViewList::const_iterator r=chain.GetResidueList().begin(),
       e2=chain.GetResidueList().end(); r!=e2; ++r) {
    mol::ResidueView res=*r;
    seq_str.append(1, res.GetOneLetterCode());
  }
  if (!seq_str.empty()) {
    seq::SequenceHandle sequence=seq::CreateSequence(name.toStdString(), seq_str);
    mol::EntityView v_one_chain=chain.GetEntity().GetHandle().CreateEmptyView();
    v_one_chain.AddChain(chain, mol::ViewAddFlag::INCLUDE_ALL);
    sequence.AttachView(v_one_chain);

    Row* new_row = new Row(this);
    Painter* p = new SeqSelectionPainter(this);
    new_row->InsertPainter(p);
    p = new SeqSecStrPainter(this);
    new_row->InsertPainter(p);
    p = new SeqTextPainter(this);
    new_row->InsertPainter(p);
    mol::alg::SecStructureSegments sec = mol::alg::ExtractSecStructureSegments(chain);
    QVarLengthArray<mol::SecStructure> sec_str(chain.GetResidueCount());
    for (mol::alg::SecStructureSegments::iterator i=sec.begin(),
         e=sec.end(); i!=e; ++i) {
      mol::alg::SecStructureSegment s=*i;
      for(int i = s.first; i <= s.last ;i++){
        sec_str[i] = s.ss_type;
      }
    }
    ListEntry entry(new_row, name, sequence, chain, sec_str);
    rows_.append(entry);
  }
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
  if(gfx::EntityP entity = this->GetGfxObject()){
    ListEntry& entry = rows_[row];
    mol::EntityView sel = entity->GetSelection();
    seq::SequenceHandle& seq = entry.seq;
    mol::EntityView view = seq.GetAttachedView().GetHandle().CreateEmptyView();

    QSetIterator<int> i(removed);
    while (i.hasNext()){
      int column = i.next()-1;
      if(column >= 0 && column < seq.GetLength()){
        if (mol::ResidueView rv=seq.GetResidue(seq.GetResidueIndex(column))) {
          view.AddResidue(rv, mol::ViewAddFlag::INCLUDE_ATOMS);
        }
      }
    }
    sel = mol::Difference(sel,view);
    view = seq.GetAttachedView().GetHandle().CreateEmptyView();
    i = QSetIterator<int>(added);
    while (i.hasNext()){
      int column = i.next()-1;
      if(column >= 0 && column < seq.GetLength()){
        if (mol::ResidueView rv=seq.GetResidue(seq.GetResidueIndex(column))) {
          view.AddResidue(rv, mol::ViewAddFlag::INCLUDE_ATOMS);
        }
      }
    }
    sel = mol::Union(sel,view);
    sel.AddAllInclusiveBonds();
    entity->SetSelection(sel);
  }
}

QVariant ViewObject::GetData(int row, int column, int role)
{
  if(row<0 || row >= rows_.size())return QVariant();

  if(column<0 || column >= rows_[row].seq.GetLength()+1)return QVariant();

  if(column == 0) {
    if (role == Qt::DisplayRole){
      return QVariant(rows_[row].name);
    }
  }
  else if(column > 0) {
    if (role==Qt::DisplayRole) {
      return QVariant(QString(rows_[row].seq.GetOneLetterCode(column - 1)));
    }
    if (role==Qt::FontRole){
      return QVariant(font_);
    }
    if (role==Qt::SizeHintRole){
      return QVariant(default_cell_size_);
    }
    if (role==Qt::UserRole){
      QVariant variant;
      variant.setValue(rows_[row]);
      return variant;
    }
    if (role==Qt::UserRole+1){
      return QVariant(default_size_);
    }
  }
  return QVariant();
}

int ViewObject::GetMaxColumnCount() const
{
  int columns = 0;
  for(int i = 0; i < rows_.size(); i++){
    int col_length = rows_[i].seq.GetLength();
    if(columns <= col_length){
      columns = col_length+1;
    }
  }
  return columns;
}

bool ViewObject::SetData(int column, const QVariant& value, int role)
{
  return false;
}

void ViewObject::DoubleClicked(int row, int column)
{
  if(column==0){
    QSet<int> all;
    int seq_length = rows_[row].seq.GetLength();
    for(int i = 0; i < seq_length; i++){
      all.insert(i+1);
    }
    this->SetSelection(row,all,QSet<int>());
  }
  else if(column>0){
    column-=1;
    QVarLengthArray<mol::SecStructure> sec = rows_[row].secstr;
    if(sec.size()>0 && column < sec.size()){
      mol::SecStructure& src_str = sec[column];
      QVarLengthArray<bool> src_type(3);
      src_type[0] = src_str.IsHelical();
      src_type[1] = src_str.IsExtended();
      src_type[2] = src_str.IsCoil();
      int i = column;
      QSet<int> cols_to_add;
      mol::SecStructure& col_str = sec[i];
      while(i >= 0 && (col_str = sec[i])){
        if(src_type[0] == col_str.IsHelical()
            && src_type[1] == col_str.IsExtended()
            && src_type[2] == col_str.IsCoil()){
        cols_to_add.insert(i+1);
        --i;
        }
        else{break;}
      }
      i = column + 1;
      if(i < sec.size()){
        while(i < sec.size() && (col_str = sec[i])){
          if(src_type[0] == col_str.IsHelical()
              && src_type[1] == col_str.IsExtended()
              && src_type[2] == col_str.IsCoil()){
          cols_to_add.insert(i+1);
          ++i;
          }
          else{
            break;
          }
        }
      }
      this->SetSelection(row,cols_to_add, QSet<int>());
    }
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
      mol::ChainView dst_chain=(rows_[i].chain);
      seq::SequenceHandle seq = rows_[i].seq;
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
    return selected_indexes;
  }
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

}}
