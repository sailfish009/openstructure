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

#ifndef IMG_GUI_DATA_VIEWER_TOOL_WIDGET_H
#define IMG_GUI_DATA_VIEWER_TOOL_WIDGET_H

#include <QWidget>

//fw decl
class QToolButton;

namespace ost { namespace img { namespace gui {

class DataViewerToolWidget: public QWidget
{
Q_OBJECT;
public:
  DataViewerToolWidget(QWidget* child, const QString& title,QWidget* parent=NULL, bool visible=true);
  ~DataViewerToolWidget();
  int GetBodyHeight() const;
  bool  IsVisible() const;
  QWidget* GetChild() const;

public slots:
  void SetVisible(bool visible=true);

signals:
  void VisibilityChanged(QWidget* widget,bool visible);
  

protected:    
 QWidget* child_;
 QToolButton* button_;
 int body_height_;
 const static int margin_;
 const static int spacing_;
 const static int title_height_;

};

}}}  //ns

#endif
