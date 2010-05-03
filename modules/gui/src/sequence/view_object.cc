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

#include "painter.hh"
#include "seq_secstr_painter.hh"
#include "seq_selection_painter.hh"
#include "seq_text_painter.hh"

#include "view_object.hh"

namespace ost { namespace gui {

ViewObject::ViewObject(seq::SequenceList& sequences, const QString& name, QObject *parent): QObject(parent), name_(name)
{
  for(int i=0; i<sequences.GetCount(); i++){
    seq::SequenceHandle seq = sequences[i];
    this->AddSequence(seq);
  }
  this->Init();
}

ViewObject::ViewObject(seq::SequenceHandle& sequence, const QString& name, QObject *parent): QObject(parent), name_(name)
{
  this->AddSequence(sequence);
  this->Init();
}

ViewObject::ViewObject(mol::ChainView& chain, const QString& name, QObject *parent): QObject(parent), name_(name)
{
  this->AddChain(chain);
  this->Init();
}

void ViewObject::Init()
{
  font_ = QFont("Courier",10);
  QFontMetrics metrics = QFontMetrics(font_);
  default_size_=QSize(metrics.boundingRect('W').width(),metrics.boundingRect('|').height());
  default_cell_size_ = QSize(metrics.boundingRect('W').width()+2,metrics.boundingRect('|').height()+2);
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

void ViewObject::AddSequence(seq::SequenceHandle& sequence)
{
  Row* new_row = new Row(this);
  Painter* p = new SeqSelectionPainter(this);
  new_row->InsertPainter(p);
  p = new SeqTextPainter(this);
  new_row->InsertPainter(p);
  QPair<Row*, seq::SequenceHandle> pair(new_row,sequence);
  //rows_.append(pair);
}

void ViewObject::AddChain(mol::ChainView& chain)
{
  String seq_str;
  seq_str.reserve(chain.GetResidueCount());
  for (mol::ResidueViewList::const_iterator r=chain.GetResidueList().begin(),
       e2=chain.GetResidueList().end(); r!=e2; ++r) {
    mol::ResidueView res=*r;
    seq_str.append(1, res.GetOneLetterCode());
  }
  if (!seq_str.empty()) {
    seq::SequenceHandle sequence=seq::CreateSequence(this->GetName().toStdString(), seq_str);
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
    QPair<Row*, seq::SequenceHandle> pair(new_row,sequence);
    mol::alg::SecStructureSegments sec = mol::alg::ExtractSecStructureSegments(chain);
    QVarLengthArray<mol::SecStructure> sec_str(chain.GetResidueCount());
    for (mol::alg::SecStructureSegments::iterator i=sec.begin(),
         e=sec.end(); i!=e; ++i) {
      mol::alg::SecStructureSegment s=*i;
      for(int i = s.first; i <= s.last ;i++){
        sec_str[i] = s.ss_type;
      }
    }
    ListEntry entry(new_row, sequence, sec_str);
    rows_.append(entry);
  }
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
    if(columns < col_length){
      columns = rows_[i].seq.GetLength() + 1;
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
