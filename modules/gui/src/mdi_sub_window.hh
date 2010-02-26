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
#ifndef OST_MDI_SUB_WINDOW_HH_
#define OST_MDI_SUB_WINDOW_HH_

/*
  Author: Andreas Schenk
 */

#include <QMdiSubWindow>
#include <QMainWindow>

#include "widget_state_saver.hh"

namespace ost{namespace gui{

typedef WidgetStateSaver<QMdiSubWindow,Qt::WindowFlags> OstMdiSubWindowBase;

class OstMdiSubWindow: public OstMdiSubWindowBase
{
  Q_OBJECT;
public:
  OstMdiSubWindow(const QString& name,QPoint pos, QSize size, bool visible,
                  QWidget* parent, Qt::WindowFlags flags);
  OstMdiSubWindow(const QString& name, QWidget* parent, Qt::WindowFlags flags);
  virtual ~OstMdiSubWindow();
public slots:
  virtual void setVisible(bool visible=true);
signals:
  void OnSetVisible(bool visible);
protected:
  virtual void closeEvent( QCloseEvent * event );
};

typedef WidgetStateSaver<QMainWindow,Qt::WindowFlags> OstTopWindowBase;

class OstTopWindow: public OstTopWindowBase
{
  Q_OBJECT;
public:
  OstTopWindow(const QString& name,QPoint pos, QSize size, bool visible,
               QWidget* parent, Qt::WindowFlags flags);
  OstTopWindow(const QString& name, QWidget* parent ,Qt::WindowFlags flags);
  virtual ~OstTopWindow();
public slots:
  virtual void setVisible(bool visible=true);
signals:
  void OnSetVisible(bool visible);
protected:
  virtual void closeEvent( QCloseEvent * event );
};

}}//ns

#endif /*OST_MDI_SUB_WINDOW_HH_*/
