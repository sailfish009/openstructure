//------------------------------------------------------------------------------
// This file is part of the OpenStructure project <www.openstructure.org>
//
// Copyright (C) 2008-2010 by the OpenStructure authors
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
  Author: Andreas Schenk
*/

#include <QKeyEvent>
#include <QMouseEvent>
#include <QWheelEvent>
#include <QGraphicsSceneWheelEvent>
#include <QApplication>
#include <QDebug>
#include <QGLWidget>
#include "viewer_panel.hh"

namespace ost { namespace img { namespace gui {

ViewerPanel::ViewerPanel(QGraphicsScene* scene, QWidget* parent):
  QGraphicsView(scene,parent),
  last_mouse_()
{
  //doesn't work on properly OS X (clashes with 3D scene)
  //setViewport(new QGLWidget);
  setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
  setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
  setDragMode(QGraphicsView::ScrollHandDrag);
}

void 	ViewerPanel::keyPressEvent(QKeyEvent* event)
{
  QGraphicsView::keyPressEvent(event);
  if(! event->isAccepted()){
    switch(event->key()) {
    case 'C':
      centerOn(scene()->sceneRect().center());
      break;
    }
  }
}



void ViewerPanel::wheelEvent(QWheelEvent* event)
{
  if (scene() && isInteractive()) {
    event->ignore();
    QGraphicsSceneWheelEvent wheelEvent(QEvent::GraphicsSceneWheel);
    wheelEvent.setWidget(viewport());
    wheelEvent.setScenePos(mapToScene(event->pos()));
    wheelEvent.setScreenPos(event->globalPos());
    wheelEvent.setButtons(event->buttons());
    wheelEvent.setModifiers(event->modifiers());
    wheelEvent.setDelta(event->delta());
    wheelEvent.setAccepted(event->isAccepted());
    wheelEvent.setAccepted(false);
    QApplication::sendEvent(scene(), &wheelEvent);
    event->setAccepted(wheelEvent.isAccepted());
  }
  if(! event->isAccepted()){
    if(event->delta()>0) {
      scale(0.5,0.5);
    } else {
      scale(2.0,2.0);
    }
  }
}

void ViewerPanel::mouseDoubleClickEvent(QMouseEvent* event)
{
  centerOn(mapToScene(event->pos()));
  update();
}

}}} //ns
