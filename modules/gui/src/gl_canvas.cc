//------------------------------------------------------------------------------
// This file is part of the OpenStructure project <www.openstructure.org>
//
// Copyright (C) 2008-2011 by the OpenStructure authors
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

#include <chrono>

#include "gl_canvas.hh"

#include <ost/log.hh>
#include <ost/gfx/scene.hh>
#include <ost/gui/tools/tool_manager.hh>
#include <ost/gui/perspective.hh>
#include <ost/gui/gosty_app.hh>

#include <QResizeEvent>
#include <QMouseEvent>


namespace ost { namespace gui {

ost::gui::GLCanvas::GLCanvas(): QOpenGLWindow(),
                                last_pos_(),
                                show_beacon_(false),
                                bench_flag_(false) { 
  LOG_DEBUG("GLCanvas::registering with scene");
  gfx::Scene::Instance().Register(this);  
}

ost::gui::GLCanvas::~GLCanvas() {
  gfx::Scene::Instance().Unregister(this);
}

void GLCanvas::StatusMessage(const String& m) {
  // This Window can also be displayed without a full blown GostyApp.
  // We therefore only feed the message into Gosty if there's already
  // a valid instance.
  if(GostyApp::ValidInstance()) {
    GostyApp::Instance()->GetPerspective()->StatusMessage(m);
  }
}

void GLCanvas::SetDefaultFormat() {
  QSurfaceFormat f = QSurfaceFormat::defaultFormat();
  f.setRedBufferSize(8);
  f.setGreenBufferSize(8);
  f.setBlueBufferSize(8);
  f.setAlphaBufferSize(8);
  f.setDepthBufferSize(24);
  this->setFormat(f);
}

void GLCanvas::SetStereoFormat() {
  QSurfaceFormat f = QSurfaceFormat::defaultFormat();
  f.setRedBufferSize(8);
  f.setGreenBufferSize(8);
  f.setBlueBufferSize(8);
  // QOpenGLWindow seems to dislike alphabuffer in stereo rendering...
  //f.setAlphaBufferSize(8);
  f.setDepthBufferSize(24);
  f.setStereo(true);
  this->setFormat(f);
}

void GLCanvas::initializeGL() {
  LOG_DEBUG("GLCanvas::initializeGL()");
  gfx::Scene::Instance().InitGL();    
}

void GLCanvas::paintGL() {

  // static variables for benchmarking
  static int benchmark_count = 0;
  static std::chrono::steady_clock::time_point timer;

  if(bench_flag_) {
    ++benchmark_count;
    if(benchmark_count == 20) {
      benchmark_count = 0;
      int ms = std::chrono::duration_cast<std::chrono::milliseconds>(
        std::chrono::steady_clock::now() - timer).count();
      Real fps = Real(20) / (Real(ms) * 0.001);
      LOG_ERROR(fps << " fps");
      timer = std::chrono::steady_clock::now();
    }
    this->update();
  } 

  gfx::Scene::Instance().RenderGL(); 
  Tool* tool=ToolManager::Instance().GetActiveTool();
  if (tool) {
    tool->RenderGL();
  }
}

void GLCanvas::resizeGL(int w, int h) {
  gfx::Scene::Instance().Resize(w, h);
}

void GLCanvas::mouseMoveEvent(QMouseEvent* event) {
  
  if (!(show_beacon_ || event->buttons())) {
    return;
  }

  if (this->IsToolEvent(event)) {
    if (ToolManager::Instance().GetActiveTool()) {
      MouseEvent mouse_event(this->TranslateButtons(event->buttons()), 
                             last_pos_, QPoint(event->x(), event->y()),
                             event->modifiers());
      ToolManager::Instance().GetActiveTool()->MouseMove(mouse_event);
      event->accept();      
    }
  } else {
    this->HandleMouseMoveEvent(event);
  }
  last_pos_=QPoint(event->x(), event->y());
  this->DoRefresh();
}

void GLCanvas::mousePressEvent(QMouseEvent* event) {
  
  if (this->IsToolEvent(event)) {
    if (ToolManager::Instance().GetActiveTool()) {
      MouseEvent mouse_event(this->TranslateButtons(event->button()), 
                             QPoint(event->x(), event->y()),
                             QPoint(event->x(), event->y()),
                             event->modifiers());
      ToolManager::Instance().GetActiveTool()->MousePress(mouse_event);
    }
  } else {
    this->HandleMousePressEvent(event);
  }
  last_pos_=QPoint(event->x(), event->y());
  this->DoRefresh();
}

void GLCanvas::mouseReleaseEvent(QMouseEvent* event) {
  
  if (this->IsToolEvent(event)) {
    if (ToolManager::Instance().GetActiveTool()) {
      MouseEvent mouse_event(this->TranslateButtons(event->button()), 
                             QPoint(event->x(), event->y()),
                             QPoint(event->x(), event->y()),
                             event->modifiers());
      ToolManager::Instance().GetActiveTool()->Click(mouse_event);
      event->accept();      
    } else {
      this->HandleMouseReleaseEvent(event);
    }
  }
  last_pos_=QPoint(event->x(), event->y());
  this->DoRefresh();
}

void GLCanvas::mouseDoubleClickEvent(QMouseEvent* event) {
  
  if (this->IsToolEvent(event)) {
    if (ToolManager::Instance().GetActiveTool()) {
      MouseEvent mouse_event(this->TranslateButtons(event->button()), 
                             QPoint(event->x(), event->y()),
                             QPoint(event->x(), event->y()),
                             event->modifiers());
      ToolManager::Instance().GetActiveTool()->DoubleClick(mouse_event);
      event->accept();
    }
  } else {
    gfx::Scene& scene=gfx::Scene::Instance();
    mol::AtomHandle atom=scene.PickAtom(event->x(), 
                                        size().height()-event->y()).second;
    if (atom.IsValid()) {
      scene.SetCenter(atom.GetPos());
    }
  }
  if (show_beacon_) {
    this->DoRefresh();
  }
}

void GLCanvas::OnTransform(gfx::InputCommand com, int indx, 
                           gfx::TransformTarget trg, Real val) {
  // does not request a redraw on purpose, in order to chain OnTransform calls
  ost::gfx::Scene::Instance().Apply(gfx::InputEvent(gfx::INPUT_DEVICE_MOUSE, com, indx,
                                                    trg, val*0.5),false);
}

void GLCanvas::SetTestMode(bool f) {
  gfx::Scene::Instance().SetTestMode(f);
}

bool GLCanvas::IsToolEvent(QInputEvent* event) const {
  return event->modifiers() & Qt::ControlModifier;
}

MouseEvent::Buttons GLCanvas::TranslateButtons(Qt::MouseButtons buttons) const {
  return MouseEvent::Buttons(buttons);
}

void GLCanvas::HandleMousePressEvent(QMouseEvent* event) {
  gfx::Scene& scene=gfx::Scene::Instance();
  scene.Pick(event->x(), scene.GetViewport().height-event->y(), 0);
  event->accept();

  // if its a right click, we emit the ContextMenu signal,
  // maybe someone wants to implement something interactive on top...
  if(event->buttons() == Qt::RightButton) {
    emit CustomContextMenuRequested(event->pos());
  } 
}

void GLCanvas::HandleMouseReleaseEvent(QMouseEvent* event) {
  event->accept();  
}

void GLCanvas::HandleMouseMoveEvent(QMouseEvent* event) {
  int indx=0;
  gfx::TransformTarget trg=gfx::TRANSFORM_VIEW;  

  if(show_beacon_) {
    ost::gfx::Scene::Instance().SetBeacon(event->x(),size().height()-event->y());
  }

  QPoint delta=QPoint(event->x(), event->y())-last_pos_;
  if (event->buttons() & Qt::LeftButton) {
    if (event->buttons() & Qt::MidButton) {
      if (event->modifiers() & Qt::ShiftModifier) {
        OnTransform(gfx::INPUT_COMMAND_SLABN,indx,
                    trg,0.5*static_cast<Real>(-delta.x()));
        OnTransform(gfx::INPUT_COMMAND_SLABF,indx,
                    trg,0.5*static_cast<Real>(delta.x()));

      } else {
        OnTransform(gfx::INPUT_COMMAND_ROTZ, indx, trg, 
                    static_cast<Real>(delta.x()+delta.y()));
      }

    } else {
      if (event->modifiers() & Qt::ShiftModifier) {
        OnTransform(gfx::INPUT_COMMAND_TRANSX,indx,
                    trg,static_cast<Real>(delta.x())); // translate along x
        OnTransform(gfx::INPUT_COMMAND_TRANSY,indx,
                    trg,static_cast<Real>(-delta.y())); // translate along y
      } else {
        OnTransform(gfx::INPUT_COMMAND_ROTY,indx,trg,
                    static_cast<Real>(delta.x())); // rotation around y
        OnTransform(gfx::INPUT_COMMAND_ROTX,indx,trg,
                    static_cast<Real>(delta.y())); // rotation around x
      }

    }
  } else if (event->buttons() & Qt::MidButton) {
      if (event->modifiers() & Qt::ShiftModifier) {
        OnTransform(gfx::INPUT_COMMAND_SLABN,indx,trg,
                    0.5*static_cast<Real>(-delta.x()));
        OnTransform(gfx::INPUT_COMMAND_SLABF,indx,trg,
                    0.5*static_cast<Real>(-delta.x()));
      } else {
        OnTransform(gfx::INPUT_COMMAND_TRANSZ,indx,
                    trg,static_cast<Real>(delta.y())); // translate along z
      }
  }
  event->accept();
}

void GLCanvas::keyPressEvent(QKeyEvent* event) {
  if(event->key()==Qt::Key_Space) {
    show_beacon_=true;
    gfx::Scene::Instance().SetBeacon(last_pos_.x(),size().height()-last_pos_.y());
    DoRefresh();
    setCursor(Qt::BlankCursor);
  }
  if((event->modifiers() & Qt::ControlModifier)) {
    // Ctrl pressed
    if(event->key()==Qt::Key_A) {
      OnTransform(gfx::INPUT_COMMAND_AUTOSLAB,0,gfx::TRANSFORM_VIEW,0.0);
      return;
    } else if(event->key()==Qt::Key_F) {
      OnTransform(gfx::INPUT_COMMAND_TOGGLE_FOG,0,gfx::TRANSFORM_VIEW,0.0);
      return;
    } else if(event->key()==Qt::Key_B) { 
      bench_flag_=!bench_flag_;
      return;
    } else if(event->key()==Qt::Key_1) {
      gfx::Scene::Instance().SetShadingMode("fallback");
      DoRefresh();
      return;
    } else if(event->key()==Qt::Key_2) {
      gfx::Scene::Instance().SetShadingMode("basic");
      DoRefresh();
      return;
    } else if(event->key()==Qt::Key_3) {
      gfx::Scene::Instance().SetShadingMode("default");
      DoRefresh();
      return;
    } else if(event->key()==Qt::Key_4) {
      gfx::Scene::Instance().SetShadingMode("hf");
      DoRefresh();
      return;
    } else if(event->key()==Qt::Key_5) {
      gfx::Scene::Instance().SetShadingMode("toon1");
      DoRefresh();
      return;
    } else if(event->key()==Qt::Key_6) {
      gfx::Scene::Instance().SetShadingMode("toon2");
      DoRefresh();
      return;
    } else if(event->key()==Qt::Key_0) {
      gfx::Scene::Instance().SetShadow(!gfx::Scene::Instance().GetShadow());
      DoRefresh();
      return;
    } else if(event->key()==Qt::Key_9) {
      gfx::Scene::Instance().SetAmbientOcclusion(!gfx::Scene::Instance().GetAmbientOcclusion());
      DoRefresh();
      return;
    } else if(event->key()==Qt::Key_Equal) {
      if(gfx::Scene::Instance().GetStereoMode()>0) {
        gfx::Scene::Instance().SetStereoMode(0);
      } else {
        gfx::Scene::Instance().SetStereoMode(1);
      }
      DoRefresh();
      return;
    }    
  }
  event->ignore();
}

void GLCanvas::keyReleaseEvent(QKeyEvent* event) {
  if(event->key()==Qt::Key_Space) {
    show_beacon_=false;
    gfx::Scene::Instance().SetBeaconOff();
    DoRefresh();
    setCursor(Qt::ArrowCursor);
    return;
  }
}

void GLCanvas::wheelEvent(QWheelEvent* event) {
  OnTransform(gfx::INPUT_COMMAND_TRANSZ,0,gfx::TRANSFORM_VIEW,
              0.2*static_cast<Real>(-event->delta()));
}

bool GLCanvas::event(QEvent* event) {
  return QOpenGLWindow::event(event);
}

}} // ns
