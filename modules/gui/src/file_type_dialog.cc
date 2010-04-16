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
#include "file_type_dialog.hh"

#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QTableWidgetItem>
#include <QHeaderView>

#include <iostream>

#include <ost/io/io_manager.hh>
#include <ost/io/io_exception.hh>

namespace ost { namespace gui {

FileTypeDialog::FileTypeDialog(const QString& file_name, QWidget* parent):
  QDialog(parent),entity_handler_(), surf_handler_()
#if OST_IMG_ENABLED
      ,map_handler_()
#endif
{
  this->setWindowTitle("File format not recognized");
  QVBoxLayout* vb=new QVBoxLayout(this);
  label_ = new QLabel("The file format could not be recognized, please select the type of the file from the list:");
  list_ = new QTableWidget(this);
  list_->setColumnCount(2);
  list_->setColumnWidth(0, 50);
  list_->setColumnWidth(1,525);
  list_->verticalHeader()->setVisible(false);
  list_->horizontalHeader()->setVisible(false);
  list_->setSelectionBehavior(QAbstractItemView::SelectRows);
  list_->setSelectionMode(QAbstractItemView::SingleSelection);
  QHBoxLayout* hb=new QHBoxLayout();  
  vb->addWidget(label_);
  vb->addWidget(list_);
  hb->setDirection(QBoxLayout::LeftToRight);
  QPushButton* cancel_btn = new QPushButton(tr("Cancel"), this);
  QPushButton* load_btn = new QPushButton(tr("Load"), this);
  hb->addStretch(1);
  hb->addWidget(cancel_btn);
  hb->addWidget(load_btn);
  vb->addLayout(hb);
  load_btn->setDefault(true);
  connect(load_btn, SIGNAL(clicked()), this, SLOT(accept()));
  connect(cancel_btn, SIGNAL(clicked()), this, SLOT(reject()));

  io::EntityIOHFList entity_handler = io::IOManager::Instance().GetAvailableEntityHandler();
  for(unsigned int i = 0 ; i < entity_handler.size() ; i++){
    QVariant handler = QVariant();
    handler.setValue(entity_handler[i]);
    this->AddRow(list_->rowCount(),entity_handler[i]->GetFormatName().c_str(),entity_handler[i]->GetFormatDescription().c_str(),handler);
  }

#if OST_IMG_ENABLED
  io::MapIOFList map_handler = io::IOManager::Instance().GetAvailableMapHandler();
  for(unsigned int i = 0 ; i < map_handler.size() ; i++){
    QVariant handler = QVariant();
    handler.setValue(map_handler[i]);
    this->AddRow(list_->rowCount(),map_handler[i]->GetFormatName().c_str(),map_handler[i]->GetFormatDescription().c_str(),handler);
  }
#endif
  io::SurfaceIOFList surf_handler = io::IOManager::Instance().GetAvailableSurfaceHandler();
  for(unsigned int i = 0 ; i < surf_handler.size() ; i++){
    QVariant handler = QVariant();
    handler.setValue(surf_handler[i]);
    this->AddRow(list_->rowCount(),surf_handler[i]->GetFormatName().c_str(),surf_handler[i]->GetFormatDescription().c_str(),handler);
  }
}

void FileTypeDialog::AddRow(int row, const QString& format_name, const QString& format_descr, QVariant& variant){
  list_->insertRow(row);
  QTableWidgetItem* new_item = new QTableWidgetItem(format_name);
  new_item->setFlags(Qt::ItemIsSelectable|Qt::ItemIsEnabled);
  new_item->setData(Qt::UserRole,variant);
  list_->setItem(row, 0, new_item);
  new_item = new QTableWidgetItem(format_descr);
  new_item->setFlags(Qt::ItemIsSelectable|Qt::ItemIsEnabled);
  list_->setItem(row, 1, new_item);
}

void FileTypeDialog::accept(){
  QList<QTableWidgetItem*> items = list_->selectedItems();
  for(int i=0; i<items.size();i++){
    if(items[i]->column()==0){
      QVariant variant = items[i]->data(Qt::UserRole);
      io::EntityIOHandlerFactoryBaseP ent_handler_fac = variant.value<io::EntityIOHandlerFactoryBaseP>();
      if(ent_handler_fac){
        entity_handler_ = ent_handler_fac->Create();
        break;
      }
      io::SurfaceIOHandlerFactoryBasePtr surf_handler_fac = variant.value<io::SurfaceIOHandlerFactoryBasePtr>();
      if(surf_handler_fac){
        surf_handler_ = surf_handler_fac->Create();
        break;
      }
#if OST_IMG_ENABLED
      io::MapIOHandlerFactoryBasePtr map_handler_fac = variant.value<io::MapIOHandlerFactoryBasePtr>();
      if(map_handler_fac){
        map_handler_ = map_handler_fac->Create();
        break;
      }
#endif
    }
  }
  this->QDialog::accept();
}

io::EntityIOHandlerP FileTypeDialog::GetEntityHandler(){
 return entity_handler_;
}


io::SurfaceIOHandlerPtr FileTypeDialog::GetSurfaceHandler(){
  return surf_handler_;
}

#if OST_IMG_ENABLED
io::MapIOHandlerPtr FileTypeDialog::GetMapHandler(){
  return map_handler_;
}
#endif

}} //ns
