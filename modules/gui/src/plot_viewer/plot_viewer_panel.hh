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
#ifndef PLOT_VIEWER_PANEL_HH_
#define PLOT_VIEWER_PANEL_HH_


/*
  Author: Andreas Schenk
*/

#include "plot_axis_horizontal.hh"
#include "plot_axis_vertical.hh"
#include <QWidget>
#include <QHash>
#include <QPair>
#include <QGraphicsView>

namespace ost{namespace gui{

class DLLEXPORT_OST_GUI PlotViewerPanel:public QGraphicsView
{
  Q_OBJECT
public:
  PlotViewerPanel(QGraphicsScene * scene, QWidget* parent=0);
  virtual ~PlotViewerPanel();
  void SetMinimumX(Real x);
  void SetMaximumX(Real x);
  void SetMinimumY(Real x);
  void SetMaximumY(Real x);
  void SetMinMax(Real xmin,Real maxx,Real miny, Real maxy);
  void SetAutofit(bool fit=true);
  Real GetMinimumX() const;
  Real GetMaximumX() const;
  Real GetMinimumY() const;
  Real GetMaximumY() const;
  Real GetDelta() const;
public slots:
  void OnZoomX(int delta,QPoint pos);
  void OnZoomY(int delta,QPoint pos);
  void OnTranslate(Real dx, Real dy);
  void update();
signals:
  void rangeChanged(Real start, Real end, Real delta);
protected:
  virtual void wheelEvent ( QWheelEvent * e );
  virtual void resizeEvent ( QResizeEvent * event );
  virtual void scrollContentsBy ( int dx, int dy );
  virtual void mousePressEvent ( QMouseEvent * event );
  virtual void mouseMoveEvent ( QMouseEvent * event );
  void UpdateAxes();
  QTransform GetTransform(QRectF scenerect, QRect viewrect);
  PlotAxisHorizontal* axis_horizontal_;
  PlotAxisVertical* axis_vertical_;
  QPointF global_mouse_pos_;
  bool autofit_;
};

}}//ns

#endif /*PLOT_VIEWER_PANEL_HH_*/
