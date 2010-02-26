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
#ifndef PLOT_DATA_GRAPHICS_ITEM_HH_
#define PLOT_DATA_GRAPHICS_ITEM_HH_

#include "plot_data_graphics_item_base.hh"
#include "plot_data.hh"

namespace ost{namespace gui{

class PlotDataGraphicsItem: public PlotDataGraphicsItemBase
{
public:
  PlotDataGraphicsItem (const PlotDataInfoPtr& info,const PlotDataPtr& dataptr, QGraphicsItem * parent = 0 );
  virtual ~PlotDataGraphicsItem();
  void Callback(const PlotDataEntry& pde);
  virtual void Redraw();

protected:
  PlotDataPtr dataptr_;
};

}}//ns

#endif /*PLOT_DATA_GRAPHICS_ITEM_HH_*/
