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
#ifndef OST_SEQUENCE_SEARCH_BAR_HH
#define OST_SEQUENCE_SEARCH_BAR_HH

/*
  Author: Marco Biasini
 */
#include <ost/gui/module_config.hh>
#include <ost/seq/alignment_handle.hh>
#include <QWidget>
#include <QComboBox>
#include <QCheckBox>

namespace ost { namespace gui {

class SequenceItem;  

/// \brief search bar to search in multiple sequence alignment
class DLLEXPORT_OST_GUI SequenceSearchBar : public QWidget {
  Q_OBJECT
public:
  SequenceSearchBar(QWidget* parent=NULL);
  
  void Show(const std::vector<SequenceItem*>& sequences);
signals:
  void Changed(const QString&, bool, int);
public slots:
   void OnSubjectChanged(const QString&);
   void OnSearchInChanged(int);
   void OnSearchAllChanged(int);
protected:
  virtual void paintEvent(QPaintEvent* paint_event);
  virtual void keyPressEvent(QKeyEvent* key_event);
private:
  seq::AlignmentHandle ali_;
  QLineEdit*           subject_;
  QCheckBox*           search_all_;
  QComboBox*           search_in_;
}; 
  
}}

#endif
