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
#include "mdi_sub_window.hh"
/* 
  Author: Andreas Schenk
 */
namespace ost{namespace gui{

OstMdiSubWindow::OstMdiSubWindow(const QString& name,QPoint pos, QSize size, 
                                  bool visible, QWidget* parent,
                                 Qt::WindowFlags flags):
  OstMdiSubWindowBase(name,pos,size, visible, parent,flags)
{
}
OstMdiSubWindow::OstMdiSubWindow(const QString& name, QWidget* parent,
                                 Qt::WindowFlags flags):
  OstMdiSubWindowBase(name,parent,flags)
{
}

OstMdiSubWindow::~OstMdiSubWindow()
{
}

void OstMdiSubWindow::closeEvent( QCloseEvent * event )
{
  hide();
}
void OstMdiSubWindow::setVisible(bool visible)
{
  OstMdiSubWindowBase::setVisible(visible);
  emit OnSetVisible(visible);
}



OstTopWindow::OstTopWindow(const QString& name,QPoint pos, QSize size, bool vis,
                           QWidget* parent ,Qt::WindowFlags flags):
  OstTopWindowBase(name,pos,size,vis,parent,flags)
{
}
OstTopWindow::OstTopWindow(const QString& name, QWidget* parent,
                           Qt::WindowFlags flags):
  OstTopWindowBase(name,parent,flags)
{
}

OstTopWindow::~OstTopWindow()
{
}

void OstTopWindow::closeEvent( QCloseEvent * event )
{
  hide();
}

void OstTopWindow::setVisible(bool visible)
{
  OstTopWindowBase::setVisible(visible);
  emit OnSetVisible(visible);
}

}}//ns
