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
#include <ost/log.hh>
#include <ost/config.hh>
#include <ost/gfx/scene.hh>

#include <ost/gui/gl_win.hh>
#include <ost/gui/tools/tool_manager.hh>
#include <ost/gui/tools/selection_tool.hh>
#include <ost/gui/tools/manipulator.hh>
#include <ost/gui/tools/measure_tool.hh>
#include <ost/gui/gosty_app.hh>
#include <ost/gui/perspective.hh>

#if OST_IMG_ENABLED
#  include <ost/gui/tools/map_tool.hh>
#endif

#include <QApplication>
#include <QDesktopWidget>
#include <QIcon>
#include <QActionGroup>
#include <QLabel>
#include <QStatusBar>
#include <QVBoxLayout>

/*
  Authors: Ansgar Philippsen, Marco Biasini
 */

namespace ost { namespace gui {

GLWin::GLWin(QWidget* p, bool try_stereo):
Widget(NULL, p), 
gl_canvas_(NULL)
{
  QMainWindow* main=new QMainWindow;
  
  if(try_stereo) {
    LOGN_VERBOSE("GLCanvas: trying stereo visuals first");
    for(int format_id=3;format_id>=0;--format_id) {
      QGLFormat format=GLWin::CreateFormat(format_id);
      format.setStereo(true);
      gl_canvas_=new GLCanvas(this, main, format);
      if(gl_canvas_->isValid() && gl_canvas_->format().stereo()) {
        break; // format is fine
      } else {
        delete gl_canvas_; // delete this canvas and try a less sophisticated format
        gl_canvas_=0;
      }
    }
  }
  if(!gl_canvas_) {
    if(try_stereo) {
      LOGN_VERBOSE("GLCanvas: no stereo visual found, trying normal ones");
    }
    for(int format_id=3;format_id>=0;--format_id) {
      QGLFormat format=GLWin::CreateFormat(format_id);
      gl_canvas_=new GLCanvas(this, main, format);
      if(gl_canvas_->isValid()) {
        break; // format is fine
      } else {
        delete gl_canvas_; // delete this canvas and try a less sophisticated format
        gl_canvas_=0;
      }
    }
  }

  if(!gl_canvas_ || !gl_canvas_->isValid()) {
    LOGN_ERROR("GLCanvas: no valid GL context found, this is pretty fatal");
    return;
  }

  this->SetInternalWidget(main);
  gfx::Scene::Instance().AttachObserver(this);
  QGLFormat format = gl_canvas_->format();
  LOGN_VERBOSE("GLCanvas: rbits=" << format.redBufferSize() 
               << " gbits=" << format.greenBufferSize() 
               << " bbits=" << format.blueBufferSize() 
               << " abits=" << format.alphaBufferSize() 
               << " dbits=" << format.depthBufferSize()
               << " accumbits=" << format.accumBufferSize()
               << " multisample=" << format.sampleBuffers()
               << " with samples=" << format.samples());
  if(gl_canvas_->format().stereo()) {
    LOGN_VERBOSE("GLCanvas: using stereo visual");
  }
  main->setCentralWidget(gl_canvas_);
  connect(gl_canvas_, SIGNAL(ReleaseFocus()), this, SIGNAL(ReleaseFocus()));
  connect(&ToolManager::Instance(), SIGNAL(ActiveToolChanged(Tool*)), this, SLOT(ActiveToolChanged(Tool*)));
  toolbar_=build_toolbar();     
  main->addToolBar(Qt::LeftToolBarArea, toolbar_);
  ToolManager::Instance().AddTool(new SelectionTool);
  ToolManager::Instance().AddTool(new Manipulator);
  ToolManager::Instance().AddTool(new MeasureTool);
#if OST_IMG_ENABLED
  ToolManager::Instance().AddTool(new MapTool);
#endif
  QBoxLayout* l=new QVBoxLayout(this);
  l->setMargin(0);
  l->setSpacing(0);
  l->addWidget(main);
}

void GLWin::ActiveToolChanged(Tool* t)
{
  gfx::Scene::Instance().RequestRedraw();
}

QGLFormat GLWin::CreateFormat(int fid)
{
  QGLFormat format = QGLFormat::defaultFormat();
  if(fid==3) {
    format.setDepthBufferSize(24);
    format.setRedBufferSize(8);
    format.setGreenBufferSize(8);
    format.setBlueBufferSize(8);
    format.setAlpha(true);
    format.setAlphaBufferSize(8);
    format.setAccum(true);
    format.setSampleBuffers(true);
  } else if(fid==2) {
    format.setDepthBufferSize(12);
    format.setRedBufferSize(8);
    format.setGreenBufferSize(8);
    format.setBlueBufferSize(8);
    format.setAlpha(true);
    format.setAlphaBufferSize(8);
    format.setAccum(true);
    format.setSampleBuffers(true);
  } else if(fid==1) {
    format.setDepthBufferSize(12);
    format.setRedBufferSize(8);
    format.setGreenBufferSize(8);
    format.setBlueBufferSize(8);
    format.setAlpha(true);
    format.setAlphaBufferSize(8);
  } else {
    format.setDepthBufferSize(6);
    format.setRedBufferSize(4);
    format.setGreenBufferSize(4);
    format.setBlueBufferSize(4);
  }
  return format;  
}

GLWin::~GLWin()
{
  gfx::Scene::Instance().DetachObserver(this);
}

void GLWin::SetTestMode(bool f)
{
  gl_canvas_->SetTestMode(f);
}

ToolBar* GLWin::build_toolbar()
{
  ToolBar* tb=new ToolBar(this);
  return tb;
}

void GLWin::StatusMessage(const String& m)
{
  GostyApp::Instance()->GetPerspective()->StatusMessage(m);
}

bool GLWin::Restore(const QString& prefix)
{
  return true;
}

bool GLWin::Save(const QString& prefix)
{
  return true;
}

}} // ns
