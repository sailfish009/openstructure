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
  Authors: Ansgar Philippsen, Andreas Schenk
*/

#include <cassert>
#include <iostream>
//#include <cstring>
//#include <sstream>


//#include <ost/img/io/io_manager.hh>
#include <ost/message.hh>
//#include <ost/img/alg/fft.hh>   
#include <ost/img/alg/norm.hh>   
#include <ost/gui/dock_widget.hh>

#include "data_viewer.hh"
#include "data_viewer_panel.hh"
#include <ost/gui/data_viewer/data_viewer_tool_widget_container.hh>
#include "overlay_base.hh"
#include "overlay_manager.hh"

#include <QLabel>
#include <QStatusBar>
#include <QMouseEvent>
#include <QVariant>
#include <QMenu>
#include <QMenuBar>
#include <QApplication>
#include <QDesktopWidget>
#include <ost/gui/data_viewer/graphics_image_item.hh>
namespace ost { namespace img { namespace gui {

namespace {
int ipow(int base, unsigned int exponent){
  int result=1;
  for(unsigned int i=0;i<exponent;++i){
    result*=base;
  }
  return result;
}
}

DataViewer::DataViewer(QWidget* p, const Data& data, const QString& name):
  QMainWindow(p,0),
  container_(new DataViewerToolWidgetContainer(this)),
  scene_(new QGraphicsScene(this)),
  panel_(new ViewerPanel(scene_,this)),
  ov_manager_(new OverlayManager(this)),
  ov_manager_gui_(new OverlayManagerGUI(this, ov_manager_)),
  info_(new InfoPanel(this)),
  argand_(new Argand(data,this)),
  fft_(new FFTPanel(data,this)),
  zoomlabel_(new QLabel("1:1")),
  slablabel_(new QLabel("0")),
  lastmouse_()
{
  setWindowTitle(name);
  GraphicsImageItem* image = new GraphicsImageItem(data);
  scene_->addItem(image);
  scene_->setBackgroundBrush(Qt::black);
  scene_->setSceneRect(image->boundingRect());
  connect(ov_manager_gui_,SIGNAL(SettingsChanged()),this,SLOT(UpdateView()));
  setAnimated(false);
  QSplitter* splitter=new QSplitter(this);
  setCentralWidget(splitter);
  splitter->addWidget(panel_);
  splitter->addWidget(container_);
  splitter->setCollapsible(0,false);
  splitter->setStretchFactor(0,1);
  splitter->setCollapsible(1,true);
  splitter->setStretchFactor(1,0);
  statusBar()->addWidget(zoomlabel_);
  statusBar()->addWidget(slablabel_);

  container_->AddChildWidget(ov_manager_gui_,"Overlays",true);
  container_->AddChildWidget(info_,"Info",true);
  container_->AddChildWidget(argand_,"Argand",false);
  container_->AddChildWidget(fft_,"Live FFT",false);
  info_->SetImageInfo(data);

 /* connect(panel_,SIGNAL(zoomed(int)),SLOT(OnZoomChange(int)));
  connect(panel_,SIGNAL(slabChanged(int)),SLOT(OnSlabChange(int)));
  connect(panel_,SIGNAL(selected(const Extent&)),argand_,SLOT(SetExtent(const Extent&)));
  connect(panel_,SIGNAL(deselected()),argand_,SLOT(ClearExtent()));
  connect(panel_,SIGNAL(selected(const Extent&)),info_,SLOT(SetSelection(const Extent&)));
  connect(panel_,SIGNAL(deselected()),info_,SLOT(ClearSelection()));
  connect(panel_,SIGNAL(released()),this,SLOT(close()));*/
  show();
}

DataViewer::~DataViewer()
{
}

void DataViewer::SetData(const Data& d)
{
//  panel_->SetData(d);
 // info_->SetImageInfo(d);
}

NormalizerPtr DataViewer::GetNormalizer() const 
{
//  assert(panel_);
//  return (panel_->GetNormalizer());
}

//! re-apply normalization using current normalizer
void DataViewer::Renormalize() 
{
 // assert(panel_);
//  panel_->Renormalize();
}

void DataViewer::Recenter() 
{
//  assert(panel_);
//  panel_->Recenter();
}

//! update view
void DataViewer::UpdateView() 
{
 // assert(panel_);
//  panel_->UpdateView(false);
}

Extent DataViewer::GetSelection() const
{
//  assert(panel_);
//  return panel_->GetSelection();
}

void DataViewer::SetName(const String& name)
{
  setWindowTitle(QString::fromStdString(name));
}

int DataViewer::GetSlab() const 
{
  //return panel_->GetSlab();
}

int DataViewer::AddOverlay(const OverlayPtr& ov, bool make_active)
{
 /* int retval= ov_manager_->AddOverlay(ov,make_active);
  if(panel_) panel_->UpdateView(false);
  return retval;*/
}

void DataViewer::ClearOverlays()
{
  ov_manager_->DeleteAllOverlays();
}

OverlayManagerPtr DataViewer::GetOverlayManager() const
{
  return ov_manager_;
}

void DataViewer::SetSlab(int slab)
{
 // panel_->SetSlab(slab);
}

void DataViewer::OnSlabChange(int slab)
{
  slablabel_->setText(QString::number(slab));
}

void DataViewer::OnZoomChange(int zoomlevel)
{
  if(zoomlevel>=0){
    zoomlabel_->setText(QString::number(ipow(2,zoomlevel))+":1");	
  }else{
    zoomlabel_->setText("1:"+QString::number(ipow(2,-zoomlevel)));	
  }
}


void DataViewer::OnPanelMouseEvent(QMouseEvent* event)
{
/*  geom::Vec2 mpos = panel_->WinToFracPoint(event->pos());
  Point mposi(mpos);
  bool lmb=event->buttons() == Qt::LeftButton;
  bool cmd=event->modifiers()&Qt::ControlModifier;

  if(panel_->GetObservedData().GetType()==REAL) {
    Real val=panel_->GetObservedData().GetReal(mposi);
    info_->SetMousePoint(mpos,val);
    if(lmb && !cmd) {
      info_->SetClickPoint(mpos,val);
    }
  } else {
    Complex val=panel_->GetObservedData().GetComplex(mposi);
    info_->SetMousePoint(mpos,val);
    if(lmb && !cmd) {
      info_->SetClickPoint(mpos,val);
    }
  }*/
}


void DataViewer::SetAntialiasing(bool f)
{
 // panel_->SetAntialiasing(f);
}

void DataViewer::AddDockWidget(QWidget* w, const QString& name, bool show)
{
  container_->AddChildWidget(w,name,show);
}

void DataViewer::RemoveDockWidget(QWidget* w)
{
  container_->RemoveWidget(w);
}


//////////////////////////
// private methods



bool DataViewer::eventFilter(QObject * object, QEvent *event)
{
  /*if (object == panel_) {
    switch (event->type()){
    case QEvent::KeyPress:
      {
        QKeyEvent *key_event = static_cast<QKeyEvent*>(event);
        if(key_event->modifiers() & Qt::ControlModifier) {
          ov_manager_->OnKeyEvent(key_event,panel_);
          return true;
        }
      }
      break;
    case QEvent::MouseButtonDblClick:
    case QEvent::MouseButtonPress:
    case QEvent::MouseButtonRelease:
    case QEvent::MouseMove:
      {
        QMouseEvent *mouse_event = static_cast<QMouseEvent*>(event);
        if((mouse_event->modifiers() & Qt::ShiftModifier) && fft_->isVisible() )
	{
          fft_->SetPosition(panel_->WinToPoint(mouse_event->pos()));
        }
        OnPanelMouseEvent(mouse_event);
        if(mouse_event->modifiers() & Qt::ControlModifier) {
          ov_manager_->OnMouseEvent(mouse_event,panel_,lastmouse_);
          lastmouse_=mouse_event->pos();
          return true;
        }else{
          lastmouse_=mouse_event->pos();
        }
      }
      break;
    case QEvent::Paint:
      {
        QPaintEvent *paint_event = static_cast<QPaintEvent*>(event);
        panel_->paintEvent(paint_event);
        QPainter painter(panel_);
        ov_manager_->OnDraw(painter,panel_);
        return true;
      }
      break;
    default:
      break;
    }
    return false;
  }else{
    return QMainWindow::eventFilter(object, event);
  }*/
}

}}}  //ns
