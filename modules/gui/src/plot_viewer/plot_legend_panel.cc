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
  Author: Andreas Schenk
*/

#include <ost/dyn_cast.hh>
#include "plot_legend_panel.hh"

#include <QMetaType>
Q_DECLARE_METATYPE(ost::gui::PlotInfoBasePtr)


namespace ost{namespace gui{


PlotLegendPanel::PlotLegendPanel(QWidget* parent):
  QTreeWidget(parent)
{
  setColumnCount(3);
  setHeaderLabels(QStringList() << "Dataset" << "Visible" << "Apprearance");
  connect(this,SIGNAL(itemChanged(QTreeWidgetItem*, int)),this,SLOT(OnItemChanged(QTreeWidgetItem*, int)));
}

PlotLegendPanel::~PlotLegendPanel()
{
}

QTreeWidgetItem* PlotLegendPanel::create_item_(const PlotInfoBasePtr& info)
{
  QTreeWidgetItem* item=new QTreeWidgetItem(QStringList(info->name));
  item->setCheckState(1,Qt::Checked);
  item->setData(0,Qt::UserRole,QVariant::fromValue(info));
  return item;
}

void PlotLegendPanel::AddData(const PlotDataInfoPtr& info)
{
  addTopLevelItem(create_item_(info));
}

void PlotLegendPanel::AddFunction(const PlotFunctionInfoPtr& info)
{
  addTopLevelItem(create_item_(info));
}

void PlotLegendPanel::AddGroup(const PlotGroupInfoPtr& info)
{
  QTreeWidgetItem* item=create_item_(info);
  generate_group_(info,item);
  addTopLevelItem(item);
}

void PlotLegendPanel::generate_group_(const PlotGroupInfoPtr& info, QTreeWidgetItem* parent)
{
  for(unsigned int i=0;i<info->GetSize();++i){
    PlotDataInfoPtr dataptr=dyn_cast<PlotDataInfo>(info->GetChild(i));
    if(dataptr!=PlotDataInfoPtr()){
      parent->addChild(create_item_(dataptr));
    }else{
      PlotFunctionInfoPtr functionptr=dyn_cast<PlotFunctionInfo>(info->GetChild(i));
      if(functionptr!=PlotFunctionInfoPtr()){
        parent->addChild(create_item_(functionptr));
      }else{
        PlotGroupInfoPtr groupptr=dyn_cast<PlotGroupInfo>(info->GetChild(i));
        if(groupptr!=PlotGroupInfoPtr()){
          QTreeWidgetItem* child=create_item_(groupptr);
          generate_group_(groupptr,child);
          parent->addChild(child);
        }else{
          //should never be reached
        }
      }
    }
  }
}
void PlotLegendPanel::OnItemChanged(QTreeWidgetItem * item, int column )
{
  if(column==1){
    item->data(0,Qt::UserRole).value<PlotInfoBasePtr>()->SetVisible(item->checkState(1));
  }
}

}}//ns
