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

#include <ost/gui/data_viewer/data_viewer_tool_widget.hh>
#include <ost/gui/data_viewer/data_viewer_tool_widget_container.hh>
#include <QList>

namespace ost { namespace img { namespace gui {

DataViewerToolWidgetContainer::DataViewerToolWidgetContainer(QWidget* parent):
    QSplitter(Qt::Vertical,parent)
{
  setHandleWidth(2);
  setChildrenCollapsible(false);
  addWidget(new QWidget);
  setStretchFactor(0,1);
}

DataViewerToolWidgetContainer::~DataViewerToolWidgetContainer()
{
}

DataViewerToolWidget* DataViewerToolWidgetContainer::AddChildWidget(QWidget* child, const QString& title, bool visible)
{
  DataViewerToolWidget* wrapped_child=new DataViewerToolWidget(child,title,this,visible);
  AddChildWidget(wrapped_child);
  return wrapped_child;
}

void DataViewerToolWidgetContainer::AddChildWidget(DataViewerToolWidget* child)
{
  int pos=count()-1;
  insertWidget(pos,child);
  connect(child,SIGNAL(VisibilityChanged(QWidget*,bool)),this,SLOT(OnChildVisibilityChanged(QWidget*, bool)));
  setStretchFactor(pos,1);
}
void DataViewerToolWidgetContainer::RemoveWidget(QWidget* w)
{
  for(unsigned int i=0;i<count();++i){
    if(dynamic_cast<DataViewerToolWidget*>(widget(i))->GetChild()==w){
      widget(i)->hide();
    }
  }
}

void DataViewerToolWidgetContainer::OnChildVisibilityChanged(QWidget* child, bool visible)
{
  DataViewerToolWidget* casted_child=dynamic_cast<DataViewerToolWidget*> (child);
  QList<int> heights=sizes();
  int index=indexOf(casted_child);
  if(visible){
    int d=casted_child->height()+casted_child->GetBodyHeight()-heights[index];
    d=std::min(d,heights.last());
    heights.last()-=d;
    heights[index]+=d;
  }else{
    int d=heights[index]-casted_child->height();
    heights.last()+=d;
    heights[index]=casted_child->height();
  }
  setSizes(heights);
}

}}}  //ns
