//------------------------------------------------------------------------------
// This file is part of the OpenStructure project <www.openstructure.org>
//
// Copyright (C) 2008-2011 by the OpenStructure authors
// Copyright (C) 2003-2010 by the IPLT authors
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
  Author: Ansgar Philippsen
*/

#include "main_window.hh"
#include "dock_widget.hh"

Q_DECLARE_METATYPE(QDockWidget*);

namespace ost { namespace gui {

MainWindow::MainWindow(QWidget* parent, Qt::WindowFlags flags):
  QMainWindow(parent,flags),
  window_menu_(new QMenu("Windows")),
  dock_widget_map_()
{
  connect(window_menu_,SIGNAL(triggered(QAction*)),this,SLOT(OnDockMenu(QAction*)));
}

void MainWindow::AddDockWidget(QWidget* w, const QString& name, bool shown, int area)
{
  DockWidget* dock = new DockWidget(name, this);
  dock->adjustSize();
  dock->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);
  dock->setWidget(w);
  addDockWidget(area==0 ? Qt::RightDockWidgetArea : Qt::LeftDockWidgetArea, dock);
  dock_widget_map_[w]=dock;
  connect(dock,SIGNAL(OnClose(QDockWidget*)),this,SLOT(OnDockClose(QDockWidget*)));
  if(!shown) dock->hide();

  QAction* act = new QAction(name,this);
  act->setCheckable(true);
  act->setChecked(shown);
  act->setData(qVariantFromValue((QDockWidget*)dock));
  WindowMenu()->addAction(act);
  
}

void MainWindow::RemoveDockWidget(QWidget* w)
{
  if(dock_widget_map_.find(w)!=dock_widget_map_.end()) {
    QDockWidget* dock = dock_widget_map_[w]; 
    QList<QAction*> alist = WindowMenu()->actions();
    for(QList<QAction*>::iterator it=alist.begin();it!=alist.end();++it) {
      QDockWidget* adock = (*it)->data().value<QDockWidget*>();
      if(dock==adock) {
        WindowMenu()->removeAction(*it);
        break;
      }
    }
    removeDockWidget(dock);
    dock_widget_map_.erase(w);
  }
}

void MainWindow::OnDockMenu(QAction* a)
{
  QDockWidget* dock = a->data().value<QDockWidget*>();
  if(dock) {
    dock->setVisible(a->isChecked());
  }  
}

void MainWindow::OnDockClose(QDockWidget* dock)
{
  QList<QAction*> alist = WindowMenu()->actions();
  for(QList<QAction*>::iterator it=alist.begin();it!=alist.end();++it) {
    QDockWidget* adock = (*it)->data().value<QDockWidget*>();
    if(dock==adock) {
      (*it)->setChecked(false);
    }
  }
}


QMenu* MainWindow::WindowMenu()
{
  return window_menu_;
}

const QMenu* MainWindow::WindowMenu() const
{
  return window_menu_;
}

}} //ns
