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

#include <QHeaderView>
#include <QScrollBar>
#include <QTableWidgetItem>
#include <QMouseEvent>

#include <iostream>
#include "sequence_table_view.hh"
#include "sequence_model.hh"

namespace ost { namespace gui {


SequenceTableView::SequenceTableView(QAbstractItemModel * model)
{
  this->setModel(model);
  this->setStyleSheet("QTableView {"
        "show-decoration-selected: 1;"
    "}"
    "QTableView::item {"
        "border: 0px solid #d9d9d9;"
        "border-left: 0px;"
        "border-right: 0px;"
        "padding-bottom: 0px;"
        "padding-top: 0px;"
        "padding-left: 0px;"
        "padding-right: 0px;"
        "margin-top: 0px;"
        "margin-bottom: 0px;"
        "margin-left: 0px;"
        "margin-right: 0px;"
    "}");

  this->verticalHeader()->hide();
  this->horizontalHeader()->hide();

  this->setShowGrid(false);

  this->setHorizontalScrollMode(ScrollPerPixel);
  this->setVerticalScrollMode(ScrollPerPixel);

  connect(this->horizontalHeader(),SIGNAL(sectionResized(int,int,int)), this, SLOT(ResizeWidth(int,int,int)));
  connect(this->verticalHeader(),SIGNAL(sectionResized(int,int,int)), this, SLOT(ResizeHeight(int,int,int)));

  delegate_ = new SequenceDelegate(qobject_cast<SequenceModel*>(this->model()),this);

  this->InitStaticRow();
  this->InitStaticColumn();
//  this->viewport()->stackUnder(static_row_);
  this->InitStaticField();
//  this->viewport()->stackUnder(static_field_);
 }

void SequenceTableView::InitStaticColumn()
{
  static_column_ = new QTableView(this);

  static_column_->setModel(this->model());
  static_column_->setFocusPolicy(Qt::NoFocus);
  static_column_->verticalHeader()->hide();
  static_column_->horizontalHeader()->hide();

  //this->viewport()->stackUnder(static_column_);
  static_column_->setSelectionBehavior(SelectRows);
  static_column_->setSelectionModel(this->selectionModel());
  for(int col=1; col<this->model()->columnCount(); col++){
   static_column_->setColumnHidden(col, true);
  }

  static_column_->setColumnWidth(0, this->columnWidth(0) );

  static_column_->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
  static_column_->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
  static_column_->show();
  static_column_->setStyleSheet("QTableView { border: 0px;"
                                 "background-color: #FFFFFF}"
                                 "QTableView::item{ border: none;"
                                 "padding: 0px; border-width: 0px; margin: 0px;}");
  static_column_->setShowGrid(false);

  static_column_->setVerticalScrollMode(ScrollPerPixel);

  connect(static_column_->verticalScrollBar(), SIGNAL(valueChanged(int)), this->verticalScrollBar(), SLOT(setValue(int)));
  connect(verticalScrollBar(), SIGNAL(valueChanged(int)), static_column_->verticalScrollBar(), SLOT(setValue(int)));

  this->updateStaticColumn();
}

void SequenceTableView::InitStaticRow()
{
  static_row_ = new QTableView(this);

  static_row_->setModel(this->model());
  static_row_->setFocusPolicy(Qt::NoFocus);
  static_row_->horizontalHeader()->hide();
  static_row_->verticalHeader()->hide();

  static_row_->setSelectionBehavior(SelectColumns);
  static_row_->setSelectionModel(this->selectionModel());
  for(int row=1; row<this->model()->rowCount(); row++){
    static_row_->setRowHidden(row, true);
  }

  static_row_->setRowHeight(0, this->rowHeight(0));

  static_row_->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
  static_row_->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
  static_row_->show();
  static_row_->setStyleSheet("QTableView { border: 0px;"
                                 "background-color: #FFFFFF}"
                                 "QTableView::item{ border: none;"
                                 "padding: 0px; border-width: 0px; margin: 0px;}");
  static_row_->setShowGrid(false);

  static_row_->setHorizontalScrollMode(ScrollPerPixel);

  static_row_->setItemDelegate(delegate_);

  connect(static_row_->horizontalScrollBar(), SIGNAL(valueChanged(int)), this->horizontalScrollBar(), SLOT(setValue(int)));
  connect(horizontalScrollBar(), SIGNAL(valueChanged(int)), static_row_->horizontalScrollBar(), SLOT(setValue(int)));

  this->updateStaticRow();
}

void SequenceTableView::InitStaticField(){
  static_field_ = new QTableView(this);

  static_field_->setModel(this->model());
  static_field_->setFocusPolicy(Qt::NoFocus);
  static_field_->horizontalHeader()->hide();
  static_field_->verticalHeader()->hide();

  static_field_->setSelectionModel(this->selectionModel());
  for(int row=1; row<this->model()->rowCount(); row++){
    static_field_->setRowHidden(row, true);
  }
  for(int col=1; col<this->model()->columnCount(); col++){
    static_field_->setColumnHidden(col, true);
  }

  static_field_->setRowHeight(0, this->rowHeight(0));
  static_field_->setColumnWidth(0, this->columnWidth(0));

  static_field_->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
  static_field_->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
  static_field_->show();
  static_field_->setStyleSheet("QTableView { border: 0px;"
                                 "background-color: #FFFFFF}"
                                 "QTableView::item{ border: none;"
                                 "padding: 0px; border-width: 0px; margin: 0px;}");
  static_field_->setShowGrid(false);

  static_field_->setHorizontalScrollMode(ScrollPerPixel);
  static_field_->setVerticalScrollMode(ScrollPerPixel);

  connect(static_field_->horizontalScrollBar(), SIGNAL(valueChanged(int)), this->horizontalScrollBar(), SLOT(setValue(int)));
  connect(horizontalScrollBar(), SIGNAL(valueChanged(int)), static_field_->horizontalScrollBar(), SLOT(setValue(int)));
  connect(static_field_->verticalScrollBar(), SIGNAL(valueChanged(int)), this->verticalScrollBar(), SLOT(setValue(int)));
  connect(verticalScrollBar(), SIGNAL(valueChanged(int)), static_field_->verticalScrollBar(), SLOT(setValue(int)));

  this->updateStaticField();
}
void SequenceTableView::ResizeWidth(int index, int, int size)
{
  if(index == 0){
    static_column_->setColumnWidth(0,size);
    static_field_->setColumnWidth(0,size);
    this->updateStaticColumn();
    this->updateStaticField();
  }
  static_row_->setRowHeight(index,size);
}

void SequenceTableView::ResizeHeight(int index, int, int size)
{
  static_column_->setRowHeight(index, size);
  if(index == 0){
    static_row_->setRowHeight(0,size);
    static_field_->setRowHeight(0,size);
    this->updateStaticRow();
    this->updateStaticField();
  }
}

void SequenceTableView::resizeEvent(QResizeEvent * event)
{
  QTableView::resizeEvent(event);
  this->updateStaticColumn();
  this->updateStaticRow();
  this->updateStaticField();
}

QModelIndex SequenceTableView::moveCursor(CursorAction action, Qt::KeyboardModifiers modifiers)
{
  QModelIndex current = QTableView::moveCursor(action, modifiers);

  if(action == MoveLeft && current.column()>0
     && this->visualRect(current).topLeft().x() < static_column_->columnWidth(0) ){
   const int new_value = horizontalScrollBar()->value() + this->visualRect(current).topLeft().x() - static_column_->columnWidth(0);
   horizontalScrollBar()->setValue(new_value);
  }


  if(action == MoveUp && current.row()>0
     && this->visualRect(current).bottomLeft().y() < static_row_->rowHeight(0) ){
   const int new_value = verticalScrollBar()->value() + this->visualRect(current).bottomRight().y() - static_row_->rowHeight(0);
   verticalScrollBar()->setValue(new_value);
  }

  return current;
}

void SequenceTableView::scrollTo(const QModelIndex & index, ScrollHint hint){
  if(index.column()>0 && index.row()>0){
    QTableView::scrollTo(index, hint);
  }
}

void SequenceTableView::updateStaticColumn()
{
  int x = this->verticalHeader()->width()+this->frameWidth();
  int y = this->frameWidth();
  int w = this->columnWidth(0);
  int h = this->viewport()->height()+this->horizontalHeader()->height();
  static_column_->setGeometry(x,y,w,h);
}

void SequenceTableView::updateStaticRow(){
  int x = this->verticalHeader()->width()+this->frameWidth();
  int y = this->horizontalHeader()->height()+this->frameWidth();
  int w = this->viewport()->width()+this->verticalHeader()->width();
  int h = this->rowHeight(0);
  static_row_->setGeometry(x,y,w,h);
}

void SequenceTableView::updateStaticField(){
  int x = this->verticalHeader()->width()+this->frameWidth();
  int y = this->horizontalHeader()->height()+this->frameWidth();
  int w = this->columnWidth(0);
  int h = this->rowHeight(0);
  static_field_->setGeometry(x,y,w,h);
}

void SequenceTableView::columnCountChanged(const QModelIndex& index, int old_count, int new_count){
  if(old_count >= 0 && old_count <= new_count){
    if(old_count == 0)old_count = 1;
    for(int col=old_count; col<=new_count; col++){
      static_column_->setColumnHidden(col, true);
      static_field_->setColumnHidden(col,true);
      this->setItemDelegateForColumn(col, delegate_);
    }
  }
}

void SequenceTableView::rowCountChanged(const QModelIndex& index, int old_count, int new_count){
  if(old_count >= 0 && old_count <= new_count){
    if(old_count == 0){
      old_count = 1;
    }
    for(int row=old_count; row<=new_count; row++){
      static_row_->setRowHidden(row, true);
      static_field_->setRowHidden(row,true);
    }
  }
}


void SequenceTableView::resizeColumnsToContents(){
  QTableView::resizeColumnsToContents();
  static_column_->setColumnWidth(0,this->columnWidth(0));
  static_field_->setColumnWidth(0,this->columnWidth(0));
  for(int i = 0; i < this->model()->columnCount(); i++){
    static_row_->setColumnWidth(i,this->columnWidth(i));
  }
  this->updateStaticColumn();
  this->updateStaticField();
}

void SequenceTableView::resizeRowsToContents(){
  QTableView::resizeRowsToContents();
  static_row_->setRowHeight(0,this->rowHeight(0));
  static_field_->setRowHeight(0,this->rowHeight(0));
  for(int i = 0; i < this->model()->columnCount(); i++){
    static_column_->setRowHeight(i,this->rowHeight(i));
  }
  this->updateStaticRow();
  this->updateStaticField();
}

QTableView* SequenceTableView::GetStaticRow(){
  return static_row_;
}

QTableView* SequenceTableView::GetStaticColumn(){
  return static_column_;
}

QTableView* SequenceTableView::GetStaticField(){
  return static_field_;
}

void SequenceTableView::mouseDoubleClickEvent(QMouseEvent *event)
{
  QModelIndex index = indexAt(event->pos());
  this->last_double_click_ = index;
  QAbstractItemView::mouseDoubleClickEvent(event);
}


void SequenceTableView::mouseReleaseEvent(QMouseEvent* event)
{
  QModelIndex index = indexAt(event->pos());
  bool double_click = (index.isValid() && index == this->last_double_click_);
  if (double_click) {
    return;
  }
  QAbstractItemView::mouseReleaseEvent(event);
}

void SequenceTableView::wheelEvent(QWheelEvent* event)
{
  if(event->modifiers() & Qt::ControlModifier){
    emit MouseWheelEvent(event);
  }
  else{
    QTableView::wheelEvent(event);
  }
}

SequenceTableView::~SequenceTableView(){}

}}
