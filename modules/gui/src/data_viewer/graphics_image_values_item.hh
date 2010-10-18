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

#ifndef GRAPHICS_IMAGE_VALUES_ITEM_HH
#define GRAPHICS_IMAGE_VALUES_ITEM_HH

#include <QGraphicsItem>
#include <ost/img/data_observer.hh>

namespace ost { namespace img { namespace gui {

class GraphicsImageValuesItem : public QGraphicsItem, public DataObserver
{
public:
    GraphicsImageValuesItem( const Data& data,QGraphicsItem* parent=0);
    virtual void 	paint( QPainter * painter, const QStyleOptionGraphicsItem * option, QWidget * widget = 0 );
    QRectF boundingRect () const;

    //////////////////////
    // observer interface
    virtual void ObserverUpdate();
    virtual void ObserverUpdate(const Extent& e);
    virtual void ObserverUpdate(const Point& p);
    virtual void ObserverRelease();
};

}}} //ns

#endif // GRAPHICS_IMAGE_VALUES_ITEM_HH
