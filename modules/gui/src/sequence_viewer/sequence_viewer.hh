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
#ifndef OST_SEQUENCE_VIEWER_HH
#define OST_SEQUENCE_VIEWER_HH

/*
  Author: Marco Biasini
 */

#include <map>

#include <ost/gfx/scene_observer.hh>
#include <ost/gui/sequence_viewer/sequence_viewer_base.hh>
#include <ost/gfx/entity.hh>

namespace ost { namespace gui {

class SequenceItem;

/// \brief sequence view
class DLLEXPORT_OST_GUI SequenceViewer : public SequenceViewerBase, 
                                         public gfx::SceneObserver {
  Q_OBJECT
public:
  SequenceViewer(QWidget* parent=NULL);
  ~SequenceViewer();
  
  virtual void NodeAdded(const gfx::GfxNodeP& node);
  
  virtual void NodeRemoved(const gfx::GfxNodeP& node);
  
  virtual void SelectionChanged(const gfx::GfxObjP& obj, const mol::EntityView& sel);
public slots:
  void ItemSelectionChanged(SequenceItem* item);
private:
  SequenceItemList SeqItemsForGfxObj(const gfx::GfxObjP& obj);
  gfx::GfxObjP GfxObjForSeqItem(SequenceItem* seq_item);  
  std::map<SequenceItem*, gfx::EntityP>  obj_map_;
  bool we_are_changing_the_selection_;
};
 
}}

#endif
