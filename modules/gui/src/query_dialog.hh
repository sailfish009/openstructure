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
#ifndef OST_GUI_QUERY_DIALOG_HH
#define OST_GUI_QUERY_DIALOG_HH

/*
  Author: Marco Biasini
 */

#include <ost/gui/module_config.hh>

#include <QLabel>
#include <QDialog>
#include <QTextEdit>

namespace ost { namespace gui {

/// \brief dialog to enter query statement
class DLLEXPORT_OST_GUI QueryDialog : public QDialog {
  Q_OBJECT  
public:
  QueryDialog(QWidget* parent=NULL);
  QString GetQueryString() const;
public slots:
  void OnTextChanged();
private:
  
  void Colorize();
  QTextEdit*       edit_;
  QLabel*          status_;
  QTextCharFormat  default_;
  QTextCharFormat  error_;
  bool             flag_set_;
};
 
}}

#endif
