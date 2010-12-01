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

#ifndef VIEWER_PANEL_HH
#define VIEWER_PANEL_HH

#include <QGraphicsView>
#include <ost/geom/vec3.hh>

namespace ost { namespace img {

//fw decl
class Data;

namespace gui {
//fw decl
class ImageLayer;
class WidgetLayerItem;
class GraphicsImageItem;

class ViewerPanel : public QGraphicsView
{
Q_OBJECT;
public:
  ViewerPanel(QWidget* parent=0);
  virtual GraphicsImageItem* AddImage(const Data& data);
  virtual void AddWidget(QWidget* widget);
  virtual void AddWidget(QGraphicsWidget* widget);
  virtual void CenterOn(const QPointF& p);
protected:
  virtual void 	keyPressEvent (QKeyEvent * event);
  virtual void 	wheelEvent (QWheelEvent* event);
  virtual void 	mouseDoubleClickEvent(QMouseEvent* event);
  virtual void 	mousePressEvent(QMouseEvent* event);
  virtual void 	mouseReleaseEvent(QMouseEvent* event);
  virtual void 	mouseMoveEvent(QMouseEvent* event);
  virtual void 	scrollContentsBy ( int dx, int dy );
  virtual void   resizeEvent(QResizeEvent* event);
  ImageLayer* image_layer_;
  WidgetLayerItem* widget_layer_;
  QPoint last_mouse_;
  bool moving_image_;
signals:
  void clicked(const geom::Vec3& mousepos);
  void zoomed(Real scale);
  void released(void);

};

}}} //ns

#endif // VIEWER_PANEL_HH
