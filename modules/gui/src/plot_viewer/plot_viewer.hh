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
#ifndef PLOT_VIEWER_HH_
#define PLOT_VIEWER_HH_

/*
  Author: Andreas Schenk
*/

#include <QStandardItemModel>
#include <QGraphicsScene>
#include <QWidget>
#include <QString>

#include "plot_data.hh"
#include "plot_function.hh"
#include "plot_group.hh"
#include "plot_function_info.hh"
#include "plot_data_info_fw.hh"
#include "plot_group_info.hh"

namespace ost{namespace gui{


//fw decl
class PlotViewerPanel;
class PlotLegendPanel;


class DLLEXPORT_OST_GUI PlotViewer: public QWidget
{
  typedef boost::shared_ptr<QStandardItem> QStandardItemPtr;
public:
  PlotViewer(QWidget* parent,const QString& name="");
  virtual ~PlotViewer();
  PlotDataInfoPtr Add(PlotDataPtr dataptr, bool fit=false);
  PlotFunctionInfoPtr Add(PlotFunctionPtr functionptr);
  PlotGroupInfoPtr Add(PlotGroupPtr groupptr, bool fit=false);
  void SetMinimumX(Real x);
  void SetMaximumX(Real x);
  void SetMinimumY(Real y);
  void SetMaximumY(Real y);
  Real GetMinimumX() const;
  Real GetMaximumX() const;
  Real GetMinimumY() const;
  Real GetMaximumY() const;
  void SetAutofit(bool fit=true);
private:
  QGraphicsScene scene_;
  PlotViewerPanel* viewer_;
  PlotLegendPanel* legend_;
  QStandardItemModel model_;
  std::vector<QStandardItemPtr> ptr_list_;
};

}}//ns

#endif /*PLOT_VIEWER_HH_*/
