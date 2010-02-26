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
#ifndef OST_GUI_SEQUENCE_VIEWER_BASE_HH
#define OST_GUI_SEQUENCE_VIEWER_BASE_HH
/*
  Author: Marco Biasini
 */

#include <ost/gui/sequence_viewer/sequence_item.hh>
#include <ost/gui/sequence_viewer/sequence_scene.hh>

#include <ost/gui/widget.hh>

#include <QGraphicsView>

namespace ost { namespace gui {

class SequenceSearchBar;

/// \brief base class for sequence viewers
/// 
/// subclassed by AlignmentViewer and SequenceViewer
class DLLEXPORT_OST_GUI SequenceViewerBase : public Widget {
  Q_OBJECT
public:
  typedef enum {
    DENSE,
    LOOSE,
  } Style;
  
  typedef enum {
    COLUMN,
    ROW
  } SelMode;
public:
  SequenceViewerBase(QWidget* parent=NULL);
  void SetDisplayStyle(Style style);
  
  void SetSelMode(SelMode mode);
  
  SelMode GetSelMode() const;
  Style GetDisplayStyle() const;
  
  /// \brief add sequence
  /// 
  /// issue: should we copy the sequence object to avoid weird side-effects?
  void AddSequence(SequenceItem* seq);
  
  /// \brief remove sequence
  void RemoveSequence(SequenceItem* seq);
  
  /// \brief select columns in all sequences
  void SelectColumns(std::vector<bool>& selected_cols);
  
  /// \brief select columns in sequence
  void SelectColumns(SequenceItem* seq, std::vector<bool>& selected_cols);
  /// \brief get length of longest sequence including gaps
  int GetLongestSequenceLength() const;
  /// \brief remove all sequences
  void Clear();

  virtual bool Restore(const QString& prefix);
  virtual bool Save(const QString& prefix);
public slots:
  /// \brief clear selection
  void ClearSelection();
  
  /// \internal
  void OnSearchBarUpdate(const QString&, bool, int);
  
  /// \brief show sequence search bar
  void FindInSequence();

private:
  Style               style_;  
  SelMode             sel_mode_;
  SequenceScene*      scene_;
  SequenceSearchBar*  search_bar_;  
  QGraphicsView*      view_;
};

}}
#endif
