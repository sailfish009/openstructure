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

#ifndef GRAPHICS_IMAGE_ITEM_HH
#define GRAPHICS_IMAGE_ITEM_HH

#include <ost/img/data_observer.hh>
#include <ost/gui/data_viewer/viewer_normalizer.hh>
#include <QGraphicsObject>
#include <QCache>

//fw decl
namespace ost { namespace img { namespace gui {
class GraphicsImageItem;
class GraphicsImageValuesItem;
}}} //ns


class CacheKey
{
public:
  CacheKey(ost::img::gui::GraphicsImageItem* item,int x, int y, int z,int binning):
    item_(item),
    x_(x),
    y_(y),
    z_(z),
    binning_(binning)
  {
  }
  bool operator==(const CacheKey& rhs) const
  {
    return x_==rhs.x_ && y_==rhs.y_ && z_==rhs.z_ && item_==rhs.item_ && binning_==rhs.binning_;
  }

  ost::img::gui::GraphicsImageItem* item_;
  int x_;
  int y_;
  int z_;
  int binning_;
};

uint qHash(const CacheKey& key);

namespace ost { namespace img { namespace gui {



class GraphicsImageItem : public QGraphicsObject, public DataObserver
{
Q_OBJECT;
public:
  //! coloring mode
  enum Mode {GREY=1, PHASECOLOR, SIGNCOLOR};

  GraphicsImageItem( const Data& data,QGraphicsItem* parent=0);
  virtual void 	paint( QPainter * painter, const QStyleOptionGraphicsItem * option, QWidget * widget = 0 );
  QRectF boundingRect () const;

  //! do a renormalization with the current normalizer
  void Renormalize();
  //! return currently active selection
  /*!
    if no selection is present, return Extent()
  */
  ViewerNormalizerPtr GetNormalizer() const;
  int GetSlab() const;

  Extent GetSelection() const;

  bool HasSelection() const;

  void SetInvert(bool invert);
  bool GetInvert() const;
  void MoveSlab(int n);

  void ClearCache();
  void ClearCacheRegion(const Extent& e);

  virtual void 	mouseMoveEvent(QGraphicsSceneMouseEvent* event);
  virtual void 	mousePressEvent(QGraphicsSceneMouseEvent* event);
  virtual void 	mouseReleaseEvent(QGraphicsSceneMouseEvent* event);
  virtual void keyPressEvent (QKeyEvent* keyEvent);
  //////////////////////
  // observer interface
  virtual void ObserverUpdate();
  virtual void ObserverUpdate(const Extent& e);
  virtual void ObserverUpdate(const Point& p);
  virtual void ObserverRelease();

signals:
  void MousePositionComplex(const QPointF& pos, Complex value);
  void MousePositionReal(const QPointF& pos, Real value);
  void SlabChanged(int slab);
protected:
  virtual void hoverMoveEvent(QGraphicsSceneHoverEvent * event) ;
  static QCache<CacheKey,QImage> cache_;
  Mode cmode_;
  int slab_;
  ViewerNormalizerPtr normalizer_;
  QGraphicsRectItem* rubberband_;
  QList<CacheKey> cached_keys_;
  const static int blocksize_;
  GraphicsImageValuesItem* value_display_;

};


}}}  //ns



#endif // GRAPHICS_IMAGE_ITEM_HH

