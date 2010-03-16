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
#include "query_dialog.hh"

#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QKeyEvent>

#include <iostream>

#include <ost/mol/query.hh>

namespace ost { namespace gui {

QueryDialog::QueryDialog(QWidget* parent):
  QDialog(parent), flag_set_(false)
{
  error_.setForeground(QBrush(QColor(150,0,0)));
  default_.setForeground(QBrush(QColor(0,0,0)));  
  delete layout();
  QVBoxLayout* vb=new QVBoxLayout(this);  
  edit_=new QTextEdit(this);
  edit_->setAcceptRichText(false);
  edit_->setTabChangesFocus(true);
  status_=new QLabel(QString(""), this);
  status_->setWordWrap(true);
  status_->setMargin(0);
  QHBoxLayout* hb=new QHBoxLayout();  
  vb->addWidget(status_);
  vb->addWidget(edit_);
  hb->setDirection(QBoxLayout::LeftToRight);
  QPushButton* select_btn = new QPushButton(tr("Select"), this);
  QPushButton* cancel_btn = new QPushButton(tr("Cancel"), this);  
  hb->addWidget(cancel_btn);
  hb->addWidget(select_btn);
  vb->addLayout(hb);
  select_btn->setDefault(true);
  connect(select_btn, SIGNAL(clicked()), this, SLOT(accept()));
  connect(cancel_btn, SIGNAL(clicked()), this, SLOT(reject()));  
  connect(edit_, SIGNAL(textChanged()), this, 
         SLOT(OnTextChanged()));    
} 

void QueryDialog::OnTextChanged()
{
  if (flag_set_)
    return;
  int l=edit_->document()->size().height()/edit_->fontMetrics().lineSpacing();
  if (l>1) {
    edit_->document()->undo();
  }
  flag_set_=true;
  this->Colorize();
  flag_set_=false;
}

void QueryDialog::Colorize()
{
  mol::Query q(edit_->toPlainText().toStdString());
  QTextCursor c=edit_->textCursor();
  c.select(QTextCursor::Document);
  c.setCharFormat(default_);  
  if (!q.IsValid()) {
    mol::QueryErrorDesc d=q.GetErrorDescription();
    c.movePosition(QTextCursor::Start);
    c.movePosition(QTextCursor::NextCharacter, QTextCursor::MoveAnchor,
                   d.range.Loc);
    c.movePosition(QTextCursor::NextCharacter, QTextCursor::KeepAnchor,
                   d.range.Length);
    c.setCharFormat(error_);             
    status_->setText(QString(d.msg.c_str()));    
  } else {
    status_->setText(QString(""));
  }
}

QString QueryDialog::GetQueryString() const
{
  return edit_->toPlainText();
}

void QueryDialog::keyPressEvent(QKeyEvent* event){
  if (event->key() == Qt::Key_Return || event->key() == Qt::Key_Enter) {
    this->accept();
  }
}

}}
