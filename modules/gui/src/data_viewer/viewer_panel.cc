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
#include "widget_layer_item.hh"
#include "image_layer.hh"
#include "viewer_graphics_scene.hh"

namespace ost { namespace img { namespace gui {

ViewerPanel::ViewerPanel(QWidget* parent):
  QGraphicsView(new ViewerGraphicsScene(),parent),
  image_layer_(new ImageLayer()),
  widget_layer_(new WidgetLayerItem()),
  last_mouse_(),
  moving_image_(false)
{
  //doesn't work on properly OS X (clashes with 3D scene)
  //setViewport(new QGLWidget);
  setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
  setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
  scene()->addItem(image_layer_);
  scene()->addItem(widget_layer_);
  scene()->setBackgroundBrush(Qt::black);
  setAlignment(Qt::AlignLeft | Qt::AlignTop);
  setTransformationAnchor(NoAnchor);
  setResizeAnchor(NoAnchor);
}

void 	ViewerPanel::keyPressEvent(QKeyEvent* event)
{
  QGraphicsView::keyPressEvent(event);
  if(! event->isAccepted()){
    switch(event->key()) {
    case 'C':
      CenterOn(image_layer_->GetCenteringPosition());
      break;
    }
  }
}



void ViewerPanel::wheelEvent(QWheelEvent* event)
{
  // redirect wheel event to graphics items but don't call base class handler to avoid moving the scrollbars
  if (scene() && isInteractive()) {
    event->ignore();
    QGraphicsSceneWheelEvent wheelEvent(QEvent::GraphicsSceneWheel);
    wheelEvent.setWidget(viewport());
    wheelEvent.setScenePos(mapToScene(event->pos()));
    wheelEvent.setScreenPos(event->globalPos());
    wheelEvent.setButtons(event->buttons());
    wheelEvent.setModifiers(event->modifiers());
    wheelEvent.setDelta(event->delta());
    wheelEvent.setAccepted(false);
    QApplication::sendEvent(scene(), &wheelEvent);
    event->setAccepted(wheelEvent.isAccepted());
  }
  if(! event->isAccepted()){
    static const qreal scalemax=4096.0;
    if(event->delta()>0) {
      if(image_layer_->scale()>1.0/scalemax){
        QPointF p=image_layer_->pos()-geometry().center();
        image_layer_->setScale(0.5*image_layer_->scale());
        image_layer_->setPos(geometry().center()+p*0.5);
      }
    } else {
      if(image_layer_->scale()<scalemax){
        QPointF p=image_layer_->pos()-geometry().center();
        image_layer_->setScale(2.0*image_layer_->scale());
        image_layer_->setPos(geometry().center()+p*2.0);
      }
    }
    emit zoomed(image_layer_->scale());
  }
}

void ViewerPanel::mouseDoubleClickEvent(QMouseEvent* event)
{
  QGraphicsSceneMouseEvent mouseEvent(QEvent::GraphicsSceneMouseDoubleClick);
  mouseEvent.setWidget(viewport());
  mouseEvent.setScenePos(mapToScene(event->pos()));
  mouseEvent.setScreenPos(event->globalPos());
  mouseEvent.setButtons(event->buttons());
  mouseEvent.setModifiers(event->modifiers());
  mouseEvent.setAccepted(false);
  QApplication::sendEvent(scene(), &mouseEvent);
  event->setAccepted(mouseEvent.isAccepted());
  if(!event->isAccepted()){
    CenterOn(image_layer_->mapFromScene(event->pos()));
  }
}

GraphicsImageItem* ViewerPanel::AddImage(const Data& data)
{
  GraphicsImageItem* item = image_layer_->AddImage(data);
  CenterOn(image_layer_->GetCenteringPosition());
  return item;
}

void ViewerPanel::AddWidget(QWidget* widget)
{
  widget_layer_->AddWidget(widget);
}

void ViewerPanel::AddWidget(QGraphicsWidget* widget)
{
  widget_layer_->AddWidget(widget);
}

void ViewerPanel::CenterOn(const QPointF& p)
{
  image_layer_->setPos(width()/2.0-p.x()*image_layer_->scale(),height()/2.0-p.y()*image_layer_->scale());
}

void 	ViewerPanel::scrollContentsBy ( int dx, int dy )
{
}

void ViewerPanel::resizeEvent(QResizeEvent* event)
{
  QGraphicsView::resizeEvent(event);
  QPointF p=image_layer_->pos()-QPointF(event->oldSize().width()/2.0,event->oldSize().height()/2.0);
  image_layer_->setPos(QPointF(event->size().width()/2.0,event->size().height()/2.0)+p);
}

void ViewerPanel::mousePressEvent(QMouseEvent* event)
{
  QGraphicsView::mousePressEvent(event);
  last_mouse_=event->pos();
  if(!event->isAccepted()&& event->button() == Qt::RightButton){
    moving_image_=true;
  }
}

void ViewerPanel::mouseReleaseEvent(QMouseEvent* event)
{
  QGraphicsView::mouseReleaseEvent(event);
  last_mouse_=event->pos();
  if(event->button() == Qt::RightButton){
    moving_image_=false;
  }
}

void ViewerPanel::mouseMoveEvent(QMouseEvent* event)
{
  QGraphicsView::mouseMoveEvent(event);
  if(moving_image_){
    image_layer_->setPos(image_layer_->pos().x()+event->pos().x()-last_mouse_.x(),image_layer_->pos().y()+event->pos().y()-last_mouse_.y());
  }
  last_mouse_=event->pos();
}


}}} //ns
