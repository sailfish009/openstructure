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
#ifndef OST_GUI_GL_CANVAS_HH
#define OST_GUI_GL_CANVAS_HH


// must come before QGLWidget
#include <ost/mol/mol.hh>


// Qt includes must come last
#include <QGLWidget>
#include <QTime>
#include <QBasicTimer>
#include <QMouseEvent>
#include <ost/gui/tools/tool.hh>
#include <ost/gui/scene_menu.hh>
#include <ost/gfx/glwin_base.hh>
#include <ost/gfx/input.hh>
#include <ost/gfx/gfx_object_fw.hh>

class QMenu;

namespace ost { namespace gui {

class GLCanvas;

class GLWin;

class DLLEXPORT_OST_GUI GLCanvas : public QGLWidget, public gfx::GLWinBase
{  
  Q_OBJECT;
public:
  GLCanvas(GLWin *gl_win, QWidget* parent,
           const QGLFormat& f = QGLFormat::defaultFormat());

  // gfx::GLWinBase interface
  virtual void MakeActive();
  virtual void DoRefresh();
  virtual void StatusMessage(const String& m);
  virtual void SetStereo(bool s);
  virtual bool HasMultisample() const {return format().sampleBuffers();}

  // central point for sending input to the gfx layer
  void OnTransform(gfx::InputCommand, int indx, 
                   gfx::TransformTarget, Real val);

  void SetTestMode(bool f);
  
signals:
  void ReleaseFocus();
  
protected:
  void Select(const String& query);  
  void CopySelectionToClipboard();  
  // QGLWidget interface
  virtual void initializeGL();
  virtual void resizeGL(int w, int h);
  virtual void paintGL();
  
  virtual void mouseMoveEvent(QMouseEvent* event);
  virtual void mousePressEvent(QMouseEvent* event);
  virtual void mouseReleaseEvent(QMouseEvent* event);
  virtual void mouseDoubleClickEvent(QMouseEvent* event);
  
  virtual void keyPressEvent(QKeyEvent* event);
  virtual void keyReleaseEvent(QKeyEvent* event);
  virtual void timerEvent(QTimerEvent * event);
  virtual void wheelEvent(QWheelEvent* event);

private slots:
  virtual void RequestContextMenu(const QPoint& pos);

private:
  bool IsToolEvent(QInputEvent* event) const;
  MouseEvent::Buttons TranslateButtons(Qt::MouseButtons buttons) const;
  void HandleMousePressEvent(QMouseEvent* event);
  void HandleMouseReleaseEvent(QMouseEvent* event);
  void HandleMouseMoveEvent(QMouseEvent* event);
  void build_context_menu();
  void prepare_context_menu(const QPoint& pos);
  GLWin* glwin_;
  int mouse_key_mask_;
  bool refresh_;
  QBasicTimer master_timer_;
  bool bench_flag_;
  QPoint last_pos_;
  SceneMenu* scene_menu_;
};

}} // ns

#endif
