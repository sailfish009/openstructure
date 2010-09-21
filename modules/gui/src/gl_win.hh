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
#ifndef OST_GUI_GL_WIN_HH
#define OST_GUI_GL_WIN_HH


#include <ost/gfx/scene_observer.hh>

#include <ost/gui/module_config.hh>
#include <ost/gui/widget.hh>
#include <ost/gui/gl_canvas.hh>

#include <ost/gui/tools/tool.hh>
#include <ost/gui/tools/tool_bar.hh>

#include <QMainWindow>
#include <QAction>

#include <QActionGroup>
#include <QStatusBar>

/*
  Authors: Ansgar Philippsen, Marco Biasini
 */
namespace ost { namespace gui {

class DLLEXPORT_OST_GUI GLWin: public Widget, public gfx::SceneObserver
{
  Q_OBJECT;
public:
  GLWin(QWidget* p);
  ~GLWin();
  void SetTestMode(bool f);

signals:
  void ReleaseFocus();

public:
  virtual bool Restore(const QString& prefix);
  virtual bool Save(const QString& prefix);

public slots:
  void StatusMessage(const String& m);
  void ActiveToolChanged(Tool* t);
private:
  static QGLFormat CreateFormat(int fid);
  GLCanvas* gl_canvas_;

  QAction* edit_rigid_action_;
  QAction* edit_torsion_action_;
  QAction* edit_angle_action_;
  QAction* edit_rotamer_action_;
  QAction* smode_atom_action_;
  QAction* smode_residue_action_;
  QAction* smode_chain_action_;

  ToolBar*     toolbar_;
  QStatusBar*  status_bar_;
  ToolBar* build_toolbar();
};

}} // ns

#endif

