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
#ifndef PLOT_INFO_BASE_HH_
#define PLOT_INFO_BASE_HH_

/*
  Author: Andreas Schenk
*/

#include <boost/shared_ptr.hpp>

#include <ost/gui/module_config.hh>

#include <QObject>
#include <QColor>
#include <QPen>
#include <QBrush>
namespace ost{namespace gui{

//fw decl
class PlotViewerPanel;

class DLLEXPORT_OST_GUI PlotInfoBase: public QObject
{
  Q_OBJECT  
public:
  enum modes{
    POINTS,
    LINES,
    POINTSANDLINES,
    AREA,
    POINTSANDAREA
  };
  enum shapes{
    CIRCLE,
    RECTANGLE,
    CROSS,
    DIAGONALCROSS
  };
  enum errormodes{
    NONE,
    LINE,
    BOX,
    VECTOR
  };
  PlotInfoBase(PlotViewerPanel * viewer=0);
  PlotInfoBase(const PlotInfoBase&);
  virtual ~PlotInfoBase();

  virtual PlotInfoBase& SetName(const QString& n);
  virtual PlotInfoBase& SetMode(unsigned int val);
  virtual PlotInfoBase& SetVisible(bool val);
  virtual PlotInfoBase& SetColor(const QColor& color);
  virtual PlotInfoBase& SetColor(int r, int g, int b);
  virtual PlotInfoBase& SetLineColor(int r, int g, int b);
  virtual PlotInfoBase& SetFillColor(int r, int g, int b);
  virtual PlotInfoBase& SetLineColor(const QColor& color);
  virtual PlotInfoBase& SetFillColor(const QColor& color);
  QString name;    
  QPen pen;
  QBrush brush;
  unsigned int mode;
  bool visible;

signals:
  void appearanceChanged();
  void visibilityChanged();
  void nameChanged();
};
typedef boost::shared_ptr<PlotInfoBase> PlotInfoBasePtr;

}}//ns

#endif /*PLOT_INFO_BASE_HH_*/
