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
#ifndef PLOT_LEGEND_PANEL_HH_
#define PLOT_LEGEND_PANEL_HH_

/*
  Author: Andreas Schenk
*/

#include "plot_data_info.hh"
#include "plot_group_info.hh"
#include "plot_function_info.hh"

#include <QTreeWidget>
#include <QTreeWidgetItem>
namespace ost{namespace gui{

class DLLEXPORT_OST_GUI PlotLegendPanel:public  QTreeWidget
{
  Q_OBJECT
public:
  PlotLegendPanel(QWidget* parent=0);
  virtual ~PlotLegendPanel();
  void AddData(const PlotDataInfoPtr& info);
  void AddFunction(const PlotFunctionInfoPtr& info);
  void AddGroup(const PlotGroupInfoPtr& info);
public slots:
  void OnItemChanged(QTreeWidgetItem * item, int column );  
protected:
  void generate_group_(const PlotGroupInfoPtr& info, QTreeWidgetItem* parent);
  QTreeWidgetItem* create_item_(const PlotInfoBasePtr& info);
};

}}//ns

#endif /*PLOT_LEGEND_PANEL_HH_*/
