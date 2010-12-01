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

#include <QWidget>
#include <QPainter>
#include "widget_layer_item.hh"
#include <QGraphicsProxyWidget>
#include <QGraphicsAnchorLayout>

namespace ost { namespace img { namespace gui {

WidgetLayerItem::WidgetLayerItem(QGraphicsItem* parent):
  QGraphicsWidget(parent)
{
  setLayout(new QGraphicsAnchorLayout);
  dynamic_cast<QGraphicsAnchorLayout*>(layout())->setSpacing(25);
}

void WidgetLayerItem::paint( QPainter * painter, const QStyleOptionGraphicsItem * option, QWidget * widget)
{
}

QRectF WidgetLayerItem::boundingRect() const
{
  return QRectF();
}

void WidgetLayerItem::AddWidget(QWidget* widget)
{
  QGraphicsProxyWidget* proxy=new QGraphicsProxyWidget(this,Qt::Window);
  proxy->setWidget(widget);
  proxy->setFlag(QGraphicsItem::ItemIsMovable);
  proxy->setOpacity(0.9);
  add_widget_to_layout(proxy);
}
void WidgetLayerItem::AddWidget(QGraphicsWidget* widget)
{
  widget->setFlag(QGraphicsItem::ItemIsMovable);
  widget->setParentItem(this);
  widget->setOpacity(0.9);
  add_widget_to_layout(widget);
}



void WidgetLayerItem::add_widget_to_layout(QGraphicsWidget* widget)
{
    const static int margin=30;
  /* if(layout()->count()>=1){
   QGraphicsLayoutItem* last=layout()->itemAt(layout()->count()-1);
   dynamic_cast<QGraphicsAnchorLayout*>(layout())->addCornerAnchors(last, Qt::BottomLeftCorner, widget, Qt::TopLeftCorner);
   dynamic_cast<QGraphicsAnchorLayout*>(layout())->addCornerAnchors(last, Qt::BottomRightCorner, widget, Qt::TopRightCorner);
   }else{
     dynamic_cast<QGraphicsAnchorLayout*>(layout())->addCornerAnchors(layout(), Qt::TopLeftCorner, widget, Qt::TopLeftCorner);
   }*/
    // current widget is already child of this, therefore count is at least 2 if other widget present
   if( childItems().size()>=2){
     QGraphicsProxyWidget* last_proxy=dynamic_cast<QGraphicsProxyWidget*>(childItems().at(childItems().count()-2));
     if(last_proxy){
       QWidget* last=last_proxy->widget();
       widget->setPos(last->pos().x(),last->pos().y()+last->frameGeometry().height()+margin );
     }else{
       QGraphicsWidget* last=dynamic_cast<QGraphicsWidget*>(childItems().at(childItems().count()-2));
       widget->setPos(last->pos().x(),last->pos().y()+last->geometry().height()+margin );
     }
   }else{
     widget->setPos(margin,margin);
   }
}

}}} //ns
