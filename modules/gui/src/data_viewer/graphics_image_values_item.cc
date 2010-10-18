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

#include <QPainter>
#include <QStyleOptionGraphicsItem>
#include "graphics_image_values_item.hh"
#include "graphics_image_item.hh"

namespace ost { namespace img { namespace gui {

GraphicsImageValuesItem::GraphicsImageValuesItem( const Data& data,QGraphicsItem* parent):
  QGraphicsItem(parent),
  DataObserver(data)
{
  setFlag(QGraphicsItem::ItemUsesExtendedStyleOption);
}

QRectF GraphicsImageValuesItem::boundingRect() const
{
  Extent ext=GetObservedData().GetExtent();
  return QRectF(ext.GetStart()[0],ext.GetStart()[1],ext.GetSize()[0],ext.GetSize()[1]);
}

void GraphicsImageValuesItem::paint( QPainter * painter, const QStyleOptionGraphicsItem * option, QWidget * widget)
{
  if(painter->transform().m11()<128){
    //only display if magnification is at least 128x
    return;
  }
  GraphicsImageItem* parent=dynamic_cast<GraphicsImageItem*>(parentItem());
  int slab=parent->GetSlab();
  ViewerNormalizerPtr normalizer=parent->GetNormalizer();
  painter->setWorldMatrixEnabled (false);
  if(GetObservedData().GetType()==REAL){
    for(int x=floor(option->exposedRect.left());x<=ceil(option->exposedRect.right());++x){
      for(int y=floor(option->exposedRect.top());y<=ceil(option->exposedRect.bottom());++y){
        Real pixel_value = GetObservedData().GetReal(Point(x,y,slab));
        QString value_string = QString("%1").arg(pixel_value,0,'g',5);
        Real rv = normalizer->Convert(pixel_value);
        painter->setPen((rv>130.0) ? Qt::black : Qt::white);
        QPoint p1=deviceTransform(painter->transform()).map(QPoint(x,y));
        QPoint p2=deviceTransform(painter->transform()).map(QPoint(x+1,y+1));
        painter->drawText(QRect(p1,p2),Qt::AlignCenter,value_string);
      }
    }
  }else{
    for(int x=floor(option->exposedRect.left());x<=ceil(option->exposedRect.right());++x){
      for(int y=floor(option->exposedRect.top());y<=ceil(option->exposedRect.bottom());++y){
        Complex pixel_value = GetObservedData().GetComplex(Point(x,y,slab));
        QString value_string = QString("%1+i%2").arg(pixel_value.real(),0,'g',5).arg(pixel_value.imag(),0,'g',5);
        Real rv = normalizer->Convert(abs(pixel_value));
        painter->setPen((rv>130.0) ? Qt::black : Qt::white);
        QPoint p1=deviceTransform(painter->transform()).map(QPoint(x,y));
        QPoint p2=deviceTransform(painter->transform()).map(QPoint(x+1,y+1));
        painter->drawText(QRect(p1,p2),Qt::AlignCenter,value_string);
      }
    }
  }
}

void GraphicsImageValuesItem::ObserverUpdate()
{
  update();
}

void GraphicsImageValuesItem::ObserverUpdate(const Extent& e)
{
  update();
}

void GraphicsImageValuesItem::ObserverUpdate(const Point& p)
{
  update();
}

void GraphicsImageValuesItem::ObserverRelease()
{
  //todo: handle observer release
}

}}} //ns
