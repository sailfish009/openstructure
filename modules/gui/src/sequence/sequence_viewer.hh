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
#ifndef OST_SEQUENCE_VIEWER_SEQUENCE_VIEWER
#define OST_SEQUENCE_VIEWER_SEQUENCE_VIEWER

/*
  Author: Stefan Scheuber
 */

#include <QWidget>
#include <QTableView>

#include <ost/gui/widget.hh>

#include <ost/gui/module_config.hh>

namespace ost { namespace gui {

/// \brief QTableView with first column not moving
class DLLEXPORT_OST_GUI SequenceViewerV2 : public Widget {
  Q_OBJECT
public:
  SequenceViewerV2(QWidget* parent=NULL);
  ~SequenceViewerV2();

  virtual bool Restore(const QString&){return true;};
  virtual bool Save(const QString&){return true;};

private:
  QTableView* seq_table_view_;
};

}}

#endif
