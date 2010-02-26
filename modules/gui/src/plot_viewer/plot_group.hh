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
#ifndef PLOT_GROUP_HH_
#define PLOT_GROUP_HH_


/*
  Author: Andreas Schenk
*/

#include <vector>
#include <boost/shared_ptr.hpp>
#include "plot_types.hh"
#include "plot_data.hh"
#include "plot_function.hh"

namespace ost{namespace gui{

class PlotGroup;
typedef boost::shared_ptr<PlotGroup> PlotGroupPtr;

class DLLEXPORT_OST_GUI PlotGroup : public PlotDataBase
{
public:
  PlotGroup();
  virtual ~PlotGroup();
  void Add(PlotDataPtr dataptr);
  void Add(PlotFunctionPtr functionptr);
  void Add(PlotGroupPtr groupptr);
  unsigned int GetSize();
  PlotDataBasePtr GetChild(unsigned int index);
  virtual Real GetMinimumX();
  virtual Real GetMaximumX();
  virtual Real GetMinimumY();
  virtual Real GetMaximumY();
private:
  std::vector<PlotDataBasePtr> ptr_list_;
};


}}//ns

#endif /*PLOT_GROUP_HH_*/
