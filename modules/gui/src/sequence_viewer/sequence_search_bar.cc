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
  Author: Marco Biasini
 */

#include "sequence_search_bar.hh"
#include "sequence_item.hh"

#include <QHBoxLayout>
#include <QLineEdit>
#include <QPainter>
#include <QPaintEvent>
#include <QLabel>
#include <QKeyEvent>

namespace ost { namespace gui { 

SequenceSearchBar::SequenceSearchBar(QWidget* parent):
  QWidget(parent)
{
  subject_=new QLineEdit(this);
  search_all_=new QCheckBox("search in all sequences", this);
  search_in_=new QComboBox(this);
  QHBoxLayout* l= new QHBoxLayout(this);
  l->addSpacing(2);
  l->addWidget(subject_, 0);
  l->addWidget(search_all_, 0);
  QLabel* label=new QLabel("search in:", this);
  l->addSpacing(10);
  l->addWidget(label, 0);
  l->addWidget(search_in_, 0);
  l->addStretch(1);
  subject_->setMaximumWidth(200);
  setLayout(l);
  l->setSizeConstraint(QLayout::SetMaximumSize);
  l->setMargin(1);  
  search_all_->setCheckState(Qt::Checked);  
  search_in_->setEnabled(false);  
#if defined(__APPLE__)
  subject_->setAttribute(Qt::WA_MacSmallSize, true);
  search_all_->setAttribute(Qt::WA_MacSmallSize, true);
  search_in_->setAttribute(Qt::WA_MacSmallSize, true);    
  label->setAttribute(Qt::WA_MacSmallSize, true);
#endif  
  connect(subject_, SIGNAL(textChanged(const QString&)), this, 
          SLOT(OnSubjectChanged(const QString&)));
  connect(search_all_, SIGNAL(stateChanged(int)), this, 
          SLOT(OnSearchAllChanged(int)));
  connect(search_in_, SIGNAL(currentIndexChanged(int)), this,
          SLOT(OnSearchInChanged(int)));
}
  
void SequenceSearchBar::Show(const std::vector<SequenceItem*>& sequences)
{
  search_in_->clear();
  
  for (size_t i=0;i<sequences.size(); ++i) {
    QString name(sequences[i]->GetSequence().GetName().c_str());
    search_in_->addItem(name);
  }
  if (sequences.empty()) {
    search_all_->setCheckState(Qt::Checked);  
    search_in_->setEnabled(false);
  }
  this->show();
  subject_->setFocus(Qt::ActiveWindowFocusReason);  
  subject_->selectAll();  
}
void SequenceSearchBar::OnSearchAllChanged(int state)
{
  if (state==Qt::Unchecked) {
    search_in_->setEnabled(true);
  } else {
    search_in_->setEnabled(false);
  }
  emit Changed(subject_->text(), search_all_->checkState()==Qt::Checked, 
               search_in_->currentIndex());
}

void SequenceSearchBar::OnSearchInChanged(int index)
{
  emit Changed(subject_->text(), search_all_->checkState()==Qt::Checked, 
               search_in_->currentIndex());
}

void SequenceSearchBar::OnSubjectChanged(const QString& str)
{
  emit Changed(str, search_all_->checkState()==Qt::Checked, 
               search_in_->currentIndex());
}

void SequenceSearchBar::paintEvent(QPaintEvent* paint_event)
{
  QPainter p(this);
  p.setBrush(QBrush(QColor(Qt::blue).lighter(300)));
  p.setPen(QPen(QColor(Qt::blue).lighter(200)));  
  p.drawRect(rect());
  paint_event->accept();
}

void SequenceSearchBar::keyPressEvent(QKeyEvent* key_event)
{
  if (key_event->key()==Qt::Key_Escape) {
    this->hide();
    key_event->accept(); 
  }
  QWidget::keyPressEvent(key_event);
}

}}
