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
#ifndef OST_GUI_SEQUENCE_ITEM_HH
#define OST_GUI_SEQUENCE_ITEM_HH

#include <ost/gui/module_config.hh>
#include <ost/mol/sec_structure.hh>
#include <ost/mol/alg/sec_structure_segments.hh>

#include <ost/seq/sequence_handle.hh>

#include <QGraphicsItem>
#include <QFont>

namespace ost { namespace gui {

class SequenceItem;
class SequenceHeader;

class DLLEXPORT_OST_GUI Knob : public QGraphicsItem {
public:
  Knob(const QString& text, const QColor& color, 
       SequenceItem* parent=NULL);
  
  virtual void paint(QPainter* painter, const QStyleOptionGraphicsItem* opts,
                     QWidget* widget=NULL);  
  virtual QRectF boundingRect() const;
private:
  QString text_;
  QColor  color_;
};

class DLLEXPORT_OST_GUI SequenceHeader : public QGraphicsItem {
public:
  SequenceHeader(SequenceItem* parent=NULL);
  
  virtual void paint(QPainter* painter, const QStyleOptionGraphicsItem* opts,
                     QWidget* widget=NULL);
  virtual QRectF boundingRect() const;                     
protected:
  virtual void mousePressEvent(QGraphicsSceneMouseEvent* event);
  virtual void mouseReleaseEvent(QGraphicsSceneMouseEvent* event);  
  virtual void contextMenuEvent(QGraphicsSceneContextMenuEvent* event);
private:
  QFont font_;
};

class DLLEXPORT_OST_GUI SequenceBody : public QGraphicsItem {
public:
  SequenceBody(SequenceItem* parent=NULL);
  
  virtual void paint(QPainter* painter, const QStyleOptionGraphicsItem* opts,
                     QWidget* widget=NULL);
  virtual QRectF boundingRect() const;         
protected:
  virtual void mousePressEvent(QGraphicsSceneMouseEvent* event);
  virtual void mouseReleaseEvent(QGraphicsSceneMouseEvent* event);  
  virtual void mouseMoveEvent(QGraphicsSceneMouseEvent* event);

  virtual void mouseDoubleClickEvent(QGraphicsSceneMouseEvent* event);    
private:
  QFont    font_;
  bool     merge_select_;
  bool     good_ole_click_;
  int      last_index_;  
};


class DLLEXPORT_OST_GUI SequenceItem : public QObject, // required for signals 
                                       public QGraphicsItem {
  Q_OBJECT
public:
  typedef std::vector<Range> Selection;  
  
  SequenceItem(const seq::SequenceHandle& seq, QGraphicsItem* parent=NULL);
  
  /// \brief get sequence
  const seq::SequenceHandle& GetSequence() const;
  
  virtual void paint(QPainter* painter, const QStyleOptionGraphicsItem* opts,
                     QWidget* widget=NULL);
  virtual QRectF boundingRect() const;                     
  
  QRectF GetCharBounds(int pos) const;
  
  float GetCharWidth() const;  
  float GetCharHeight() const;
  
  SequenceBody* GetBody();
  SequenceHeader* GetHeader();
  QPainterPath& GetSecStructPaths() { return sec_paths_; }
  QFont GetFont() { return font_; }
  
  const Selection& GetSelection() const;
  void Select(int i1, int i2, bool clear=true);
    
  void BeginSelectionChange();
  void EndSelectionChange();  
  
  bool IsEverythingSelected() const;
  void ClearSelection();
  void SelectAll();
  void SetShowSecStructure(bool show=true);
  bool IsSecStructureVisible() const;
  
  mol::alg::SecStructureSegments& GetSecStructSegments();
signals:

  /// \brief emitted whenever the selection changes
  /// 
  /// When using BeginSelectionChange(), no signal is emitted until 
  /// EndSelectionChange() is called.
  void SelectionChanged(SequenceItem* item);
private:
  void ExtractSecStructSegments();
  void SecStructSegmentsToPaths();
  void StackToPath(std::vector<QPointF>& stack);
  
  QPainterPath                     sec_paths_;
  mol::alg::SecStructureSegments   sec_;  
  SequenceHeader*                  header_;
  SequenceBody*                    body_;
  seq::SequenceHandle              seq_;
  QFont                            font_;
  Selection                        selection_;
  Selection                        ref_sel_;
  float                            advance_;
  float                            height_; 
  float                            ascent_;
  int                              selection_change_;
  bool                             show_sec_struct_;
};

typedef std::vector<SequenceItem*> SequenceItemList;
}}

#endif
