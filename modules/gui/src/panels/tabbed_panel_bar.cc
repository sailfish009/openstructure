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
#include <QDir>

#include <ost/gui/widget_registry.hh>

#include "tabbed_panel_bar.hh"

namespace ost { namespace gui {

TabbedPanelBar::TabbedPanelBar(PanelBar* parent):
  PanelWidgetContainer(parent)
{
  layout_ = new QHBoxLayout(this);
  layout_->setMargin(0);
  layout_->setSpacing(0);
  tab_widget_ = new QTabWidget(this);
  #if QT_VERSION>=0x40503
    tab_widget_->setDocumentMode(true);
  #endif
  layout_->addWidget(tab_widget_);

  toolbar_ = new QToolBar(this);
  toolbar_->setFloatable(false);
  toolbar_->setContentsMargins(0, 0, 0, 0);
  toolbar_->setToolButtonStyle(Qt::ToolButtonIconOnly);
  toolbar_->setIconSize(QSize(16,16));
  tab_widget_->setCornerWidget(toolbar_, Qt::TopRightCorner);
  connect(tab_widget_,SIGNAL(currentChanged(int)),this,SLOT(CurrentChanged(int)));
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
  if(tab_widget_->indexOf(widget)>=0){
    tab_widget_->removeTab(tab_widget_->indexOf(widget));
  }
  tab_widget_->insertTab(index,widget,WidgetRegistry::Instance()->GetFullName(class_name));
}

void TabbedPanelBar::ShowWidget(Widget* widget, int pos, bool show){
  if(tab_widget_->indexOf(widget)>=0){
    if(!show){
      tab_widget_->removeTab(tab_widget_->indexOf(widget));
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

void TabbedPanelBar::CurrentChanged(int index){
  if(Widget* widget = qobject_cast<Widget*>(tab_widget_->currentWidget())){
    ActionList actions = widget->GetActions();
    toolbar_->clear();
    if(!actions.isEmpty()){
      for(int i = 0;i<actions.size();i++){
        toolbar_->addAction(actions[i]);
      }
    }
  }
}


TabbedPanelBar::~TabbedPanelBar(){
  toolbar_->clear();
}

}}

