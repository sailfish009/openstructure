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

#include "plot_info_base.hh"

namespace ost { namespace gui {

PlotInfoBase::PlotInfoBase(PlotViewerPanel * viewer):
  QObject(),
  name(""),
  pen(QPen(QColor(0,0,0))),
  brush(QBrush(QColor(255,255,255))),
  mode(LINE),
  visible(true)
{
}

PlotInfoBase::PlotInfoBase(const PlotInfoBase& pib):
  QObject(),
  name(pib.name),
  pen(pib.pen),
  brush(pib.brush),
  mode(pib.mode),
  visible(pib.visible)
{
}

PlotInfoBase::~PlotInfoBase()
{
}


PlotInfoBase& PlotInfoBase::SetMode(unsigned int val)
{
  mode=val; 
  emit appearanceChanged();
  return *this;
}  

PlotInfoBase& PlotInfoBase::SetVisible(bool val)
{
  visible=val;
  emit visibilityChanged();
  return *this;
}  

PlotInfoBase& PlotInfoBase::SetColor(const QColor& color)
{
  pen.setColor(color); 
  brush.setColor(color);
  emit appearanceChanged();
  return *this;
}

PlotInfoBase& PlotInfoBase::SetColor(int r, int g, int b)
{
  return SetColor(QColor(r,g,b));
}

PlotInfoBase& PlotInfoBase::SetLineColor(int r, int g, int b)
{
  return SetLineColor(QColor(r,g,b));
}

PlotInfoBase& PlotInfoBase::SetLineColor(const QColor& color)
{
  pen.setColor(color); 
  emit appearanceChanged();
  return *this;
}

PlotInfoBase& PlotInfoBase::SetFillColor(int r, int g, int b)
{
  return SetFillColor(QColor(r,g,b));
}
PlotInfoBase& PlotInfoBase::SetFillColor(const QColor& color)
{
  brush.setColor(color);
  emit appearanceChanged();
  return *this;
}

PlotInfoBase& PlotInfoBase::SetName(const QString& n)
{
  name=n;
  emit nameChanged();
  return *this;
}

}}//ns
