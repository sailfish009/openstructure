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
#ifndef PLOT_DATA_POINT_HH_
#define PLOT_DATA_POINT_HH_


/*
  Author: Andreas Schenk
*/

#include <ost/gui/module_config.hh>
#include <QString>

namespace ost{namespace gui{

class DLLEXPORT_OST_GUI PlotDataEntry {
public:
    PlotDataEntry(Real x_=0, Real ex_=0, Real y_=0, Real ey_=0, Real q_=1,
                  const QString& info_ = ""):
      x(x_), y(y_), ex(ex_), ey(ey_), q(q_), info(info_) {}
    Real x;
    Real y;
    Real ex;
    Real ey;
    Real q;
    QString info;
};



}}//ns

#endif /*PLOT_DATA_POINT_HH_*/
