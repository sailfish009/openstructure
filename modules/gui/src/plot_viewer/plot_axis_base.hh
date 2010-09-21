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
#ifndef PLOTAXIS_BASE_HH_
#define PLOTAXIS_BASE_HH_

/*
  Author: Andreas Schenk
*/

#include "plot_ticker.hh"
#include <QWidget>


namespace ost { namespace gui {

class DLLEXPORT_OST_GUI PlotAxisBase: public QWidget
{
  Q_OBJECT
public:
  PlotAxisBase(QWidget * parent = 0);
  virtual ~PlotAxisBase();
  const static unsigned int AXISWIDTH=60;
  void SetMinimum(Real min);
  void SetMaximum(Real max);
signals:
  void zoom(int delta,QPoint pos);
  void translate(Real deltax,Real deltay);
protected:
  int GetSeparation();
  void SetSeparation(int separation);
  void SetStart(int start);
  void SetEnd(int end);
  virtual void wheelEvent ( QWheelEvent * e );
  virtual void mousePressEvent ( QMouseEvent * event );
  TickList GetTicks();
  PlotTickerLinear ticker_;
  QPoint global_mouse_pos_;
};


}}//ns

#endif /*PLOTAXIS_BASE_HH_*/
