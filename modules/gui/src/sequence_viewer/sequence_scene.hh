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
#ifndef OST_GUI_SEQUENCE_SCENE_HH
#define OST_GUI_SEQUENCE_SCENE_HH

#include <ost/gui/module_config.hh>
#include <ost/gui/sequence_viewer/sequence_item.hh>

#include <QGraphicsScene>

namespace ost { namespace gui {


class SequenceScene : public QGraphicsScene {
  Q_OBJECT
public:
  typedef std::vector<Range> Selection;

  SequenceScene(QObject* parent=NULL);

  void SetContextMenu(QMenu* menu);

  QMenu* GetContextMenu();
  std::vector<SequenceItem*>& GetSequences() { return sequences_; }
  const Selection& GetSelection() const;

  /// \brief get length of longest sequence (including gaps) in the alignment
  void GetMaxSequenceLength();

  
  void SetHeaderWidth(float width);
  float GetHeaderWidth() const;
  void AddSequence(SequenceItem* seq);
  void RemoveSequence(SequenceItem* seq);
  void RepackSequences();
public slots:
  void OnSceneRectChange(const QRectF& rect);  
protected: 
  virtual void drawBackground(QPainter* painter, const QRectF& rect);
private:
  void UpdateAlignment();
  Selection                  selection_;
  Selection                  ref_sel_;
  std::vector<SequenceItem*> sequences_;
  QMenu*                     context_menu_;
  float                      header_width_;
};

}}

#endif
