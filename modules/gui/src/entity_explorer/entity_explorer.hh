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
#ifndef OST_GUI_ENTITY_EXPLORER_HH
#define OST_GUI_ENTITY_EXPLORER_HH

//#include <ost/signals.hh>
#include <ost/gui/module_config.hh>

// Qt includes must come last
#include <QTreeView>
#include <QMainWindow>

namespace ost { namespace gui {

class GLWin;
  
/// \brief Interactive scene menu
class DLLEXPORT_OST_GUI EntityExplorer: public QTreeView 
{
  Q_OBJECT;
public:
  EntityExplorer(QWidget* p);

public slots:
  void OnAction(QAction* action);
  void DoubleClicked(const QModelIndex& index);
};  

}} // ns

#endif
