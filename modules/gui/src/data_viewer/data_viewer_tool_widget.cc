//------------------------------------------------------------------------------
// This file is part of the OpenStructure project <www.openstructure.org>
//
// Copyright (C) 2008-2010 by the OpenStructure authors
// Copyright (C) 2003-2010 by the IPLT authors
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
  Author: Andreas Schenk
*/

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QToolButton>
#include <QStyle>
#include <ost/gui/data_viewer/data_viewer_tool_widget.hh>

namespace ost { namespace img { namespace gui {

DataViewerToolWidget::DataViewerToolWidget(QWidget* child, const QString& title,QWidget* parent, bool visible):
  child_(child),
  button_(new QToolButton),
  body_height_(child->height())
{
  child_->setParent(this);
  QVBoxLayout* vb=new QVBoxLayout;
  vb->setMargin(margin_);
  vb->setSpacing(spacing_);
  QHBoxLayout* hb=new QHBoxLayout;
  button_->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
  button_->setText(title);
  button_->setCheckable(true);
  button_->setChecked(visible);
  button_->setFixedHeight(title_height_);
  QIcon icon=style()->standardIcon(QStyle::SP_TitleBarShadeButton);
  icon.addPixmap(style()->standardPixmap(QStyle::SP_TitleBarUnshadeButton),QIcon::Normal,QIcon::On);
  button_->setIcon(icon);
  button_->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Fixed);
  vb->addWidget(button_);
  child_->setVisible(visible);
  vb->addWidget(child_);
  connect(button_,SIGNAL(toggled(bool)),this,SLOT(SetVisible(bool)));
  setLayout(vb);

}

DataViewerToolWidget::~DataViewerToolWidget()
{
}

int DataViewerToolWidget::GetBodyHeight() const
{
  return body_height_;
}

QWidget* DataViewerToolWidget::GetChild() const
{
  return child_;
}

void DataViewerToolWidget::SetVisible(bool visible)
{
  if(visible){
    setSizePolicy(QSizePolicy::Minimum,QSizePolicy::Preferred);
    setMaximumHeight(16777215);
  }else{
    body_height_=child_->height();
    setSizePolicy(QSizePolicy::Minimum,QSizePolicy::Fixed);
    setMaximumHeight(title_height_+2*margin_);
  }
  child_->setVisible(visible);
  emit VisibilityChanged(this,visible);
}

bool  DataViewerToolWidget::IsVisible() const
{
  child_->isVisible();
}

const int DataViewerToolWidget::margin_=2;
const int DataViewerToolWidget::spacing_=3;
const int DataViewerToolWidget::title_height_=20;


}}}  //ns
