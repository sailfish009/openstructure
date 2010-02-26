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
#ifndef PLOT_GRAPHICS_ELLIPSE_ITEM_HH_
#define PLOT_GRAPHICS_ELLIPSE_ITEM_HH_

/*
  Author: Andreas Schenk
*/

#include <QGraphicsItem>
#include <QGraphicsSceneHoverEvent>
#include "plot_data_entry.hh"
#include "plot_data_info_fw.hh"

namespace ost{namespace gui{

class DLLEXPORT_OST_GUI PlotGraphicsEllipseItem: public QGraphicsEllipseItem
{
public:
  PlotGraphicsEllipseItem(const PlotDataEntry& entry,const PlotDataInfoPtr& info , QGraphicsItem * parent);
  virtual ~PlotGraphicsEllipseItem();
protected:
  virtual void hoverEnterEvent ( QGraphicsSceneHoverEvent * event );
  virtual void hoverLeaveEvent ( QGraphicsSceneHoverEvent * event );
  virtual void mousePressEvent ( QGraphicsSceneMouseEvent * event );
  PlotDataEntry pde_;
};

}}//ns

#endif /*PLOT_GRAPHICS_ELLIPSE_ITEM_HH_*/
