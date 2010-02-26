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

#include <QSettings>

#include <ost/gui/widget_registry.hh>

#include "tabbed_panel_bar.hh"

namespace ost { namespace gui {

TabbedPanelBar::TabbedPanelBar(PanelBar* parent):
  PanelWidgetContainer(parent)
{
  layout_ = new QHBoxLayout(this);
  layout_->setMargin(0);
  layout_->setSpacing(0);
  tabWidget_ = new QTabWidget(this);
  #if QT_VERSION>=0x40503
    tabWidget_->setDocumentMode(true);
  #endif
  layout_->addWidget(tabWidget_);
}

bool TabbedPanelBar::Save(const QString& prefix)
{
  return true;
}

bool TabbedPanelBar::Restore(const QString& prefix)
{
  return true;
}

void TabbedPanelBar::WidgetMoved(Widget* widget, int index){
  QString class_name = widget->metaObject()->className();
  if(tabWidget_->indexOf(widget)>=0){
    tabWidget_->removeTab(tabWidget_->indexOf(widget));
  }
  tabWidget_->insertTab(index,widget,WidgetRegistry::Instance()->GetFullName(class_name));
}

void TabbedPanelBar::ShowWidget(Widget* widget, int pos, bool show){
  if(tabWidget_->indexOf(widget)>=0){
    if(!show){
      tabWidget_->removeTab(tabWidget_->indexOf(widget));
    }
  }
  else{
    if(show){
      WidgetMoved(widget,pos);
    }
  }
}

QString TabbedPanelBar::GetName(){
  return "Tabbed Mode";
}

TabbedPanelBar::~TabbedPanelBar(){
  //Do not destroy widgets
  for(int i=0; i< tabWidget_->count();i++){
    tabWidget_->widget(i)->setParent(NULL);
  }
}

}}

