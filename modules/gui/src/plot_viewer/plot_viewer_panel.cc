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
/*
  Author: Andreas Schenk
*/

#include <QtGui>
#include "plot_viewer_panel.hh"
#include "plot_types.hh"
#include "plot_function.hh"

namespace ost{namespace gui{

PlotViewerPanel::PlotViewerPanel(QGraphicsScene * scene, QWidget* parent):
  QGraphicsView(scene,parent),
  axis_horizontal_(new PlotAxisHorizontal(this)),
  axis_vertical_(new PlotAxisVertical(this)),
  global_mouse_pos_(),
  autofit_(false)
{
  setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
  setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
  setDragMode(QGraphicsView::NoDrag);
  setRenderHint(QPainter::Antialiasing,true);
  setViewportMargins (PlotAxisBase::AXISWIDTH,0,0,PlotAxisBase::AXISWIDTH);
  setTransformationAnchor(QGraphicsView::NoAnchor);
  setResizeAnchor(QGraphicsView::NoAnchor);
  scale(1.0,-1.0);
  connect(axis_horizontal_,SIGNAL(zoom(int,QPoint)),this, SLOT(OnZoomX(int,QPoint)));
  connect(axis_vertical_,SIGNAL(zoom(int,QPoint)),this, SLOT(OnZoomY(int,QPoint)));
  connect(axis_horizontal_,SIGNAL(translate(Real,Real)),this, SLOT(OnTranslate(Real ,Real)));
  connect(axis_vertical_,SIGNAL(translate(Real,Real)),this, SLOT(OnTranslate(Real,Real)));
}

PlotViewerPanel::~PlotViewerPanel()
{
}

void PlotViewerPanel::SetMinMax(Real minx,Real maxx,Real miny, Real maxy)
{
    QRectF scenerect;
    scenerect.setCoords(minx,maxy,maxx,miny);
    QRect viewrect(0,0,viewport()->width(),viewport()->height());
    setTransform(GetTransform(scenerect,viewrect));
    UpdateAxes();
}

void PlotViewerPanel::SetMinimumX(Real x)
{
  if(x<GetMaximumX()){
    QRectF scenerect;
    scenerect.setCoords(x,GetMaximumY(),GetMaximumX(),GetMinimumY());
    QRect viewrect(0,0,viewport()->width(),viewport()->height());
    setTransform(GetTransform(scenerect,viewrect));
    UpdateAxes();
  }
}
void PlotViewerPanel::SetMaximumX(Real x)
{
  if(x>GetMinimumX()){
    QRectF scenerect;
    scenerect.setCoords(GetMaximumX(),GetMinimumY(),x,GetMinimumY());
    QRect viewrect(0,0,viewport()->width(),viewport()->height());
    setTransform(GetTransform(scenerect,viewrect));
    UpdateAxes();
  }
}
void PlotViewerPanel::SetMinimumY(Real y)
{
  if(y<GetMaximumY()){
    QRectF scenerect;
    scenerect.setCoords(GetMinimumX(),GetMaximumY(),GetMaximumX(),y);
    QRect viewrect(0,0,viewport()->width(),viewport()->height());
    setTransform(GetTransform(scenerect,viewrect));
    UpdateAxes();
  }
}
void PlotViewerPanel::SetMaximumY(Real y)
{
  if(y>GetMinimumY()){
    QRectF scenerect;
    scenerect.setCoords(GetMinimumX(),y,GetMaximumX(),GetMinimumY());
    QRect viewrect(0,0,viewport()->width(),viewport()->height());
    setTransform(GetTransform(scenerect,viewrect));
    UpdateAxes();
  }
}
Real PlotViewerPanel::GetMinimumX() const
{
  return mapToScene(QPoint(0,0)).x();
}
Real PlotViewerPanel::GetMaximumX() const
{
  return mapToScene(QPoint(viewport()->width(),0)).x();
}
Real PlotViewerPanel::GetMinimumY() const
{
  return mapToScene(QPoint(0,viewport()->height())).y();
}
Real PlotViewerPanel::GetMaximumY() const
{
  return mapToScene(QPoint(0,0)).y();
}

Real PlotViewerPanel::GetDelta() const
{
  return mapToScene(QPoint(1,0)).x()-mapToScene(QPoint(0,0)).x();
}

void PlotViewerPanel::wheelEvent ( QWheelEvent * e )
{
  QPointF cursorpos=mapToScene(e->pos());
  Real scalefactor=exp(e->delta()/1000.0);
  QRectF scenerect;
  scenerect.setCoords(cursorpos.x()-(cursorpos.x()-GetMinimumX())/scalefactor,
                      cursorpos.y()+(GetMaximumY()-cursorpos.y())/scalefactor,
                      cursorpos.x()+(GetMaximumX()-cursorpos.x())/scalefactor,
                      cursorpos.y()-(cursorpos.y()-GetMinimumY())/scalefactor);
  QRect viewrect(0,0,viewport()->width(),viewport()->height());
  setTransform(GetTransform(scenerect,viewrect));
  UpdateAxes();
}
void PlotViewerPanel::OnZoomX(int delta,QPoint pos)
{
  QPointF cursorpos=mapToScene(pos+QPoint(-int(PlotAxisBase::AXISWIDTH),0));
  Real scalefactor=exp(delta/1000.0);
  QRectF scenerect;
  scenerect.setCoords(cursorpos.x()-(cursorpos.x()-GetMinimumX())/scalefactor,
                  GetMaximumY(),
                  cursorpos.x()+(GetMaximumX()-cursorpos.x())/scalefactor,
                  GetMinimumY());
  QRect viewrect(0,0,viewport()->width(),viewport()->height());
  setTransform(GetTransform(scenerect,viewrect));
  UpdateAxes();
}

void PlotViewerPanel::OnZoomY(int delta,QPoint pos)
{
  QPointF cursorpos=mapToScene(pos);
  Real scalefactor=exp(delta/1000.0);
  QRectF scenerect;
  scenerect.setCoords(GetMinimumX(),
                      cursorpos.y()+(GetMaximumY()-cursorpos.y())/scalefactor,
                      GetMaximumX(),
                      cursorpos.y()-(cursorpos.y()-GetMinimumY())/scalefactor);
  QRect viewrect(0,0,viewport()->width(),viewport()->height());
  setTransform(GetTransform(scenerect,viewrect));
  UpdateAxes();
}

void PlotViewerPanel::resizeEvent ( QResizeEvent * event )
{
  QGraphicsView::resizeEvent(event);
  QRect cr=contentsRect();
  axis_horizontal_->setGeometry(QRect(cr.left(), cr.top()+cr.height()-PlotAxisBase::AXISWIDTH,cr.width(), PlotAxisBase::AXISWIDTH));
  axis_vertical_->setGeometry(QRect(cr.left(), cr.top(),PlotAxisBase::AXISWIDTH, cr.height()));
  if(autofit_){
    QRectF scenerect;
    scenerect.setCoords(GetMinimumX(),
                        GetMaximumY(),
                        mapToScene(QPoint(event->oldSize().width(),0)).x(),
                        mapToScene(QPoint(0,event->oldSize().height())).y());
    QRect viewrect(0,0,viewport()->width(),viewport()->height());  QGraphicsView::resizeEvent(event);
    setTransform(GetTransform(scenerect,viewrect));
  }else{
    setTransform(transform().translate(0.0,(event->size().height()-event->oldSize().height())/transform().m22()));
  }
  UpdateAxes();
}
void PlotViewerPanel::UpdateAxes()
{
  axis_horizontal_->SetMinimum(GetMinimumX());
  axis_horizontal_->SetMaximum(GetMaximumX());
  axis_vertical_->SetMinimum(GetMinimumY());
  axis_vertical_->SetMaximum(GetMaximumY());
  emit rangeChanged(GetMinimumX(),GetMaximumX(),GetDelta());
}

void PlotViewerPanel::scrollContentsBy ( int dx, int dy )
{
  QGraphicsView::scrollContentsBy(dx,dy);
  UpdateAxes();
}

void PlotViewerPanel::update()
{
  QGraphicsView::update();
  UpdateAxes();
}

void PlotViewerPanel::OnTranslate(Real dx, Real dy)
{
  translate(dx/transform ().m11(),dy/transform ().m22());
  update();
}

void PlotViewerPanel::mousePressEvent( QMouseEvent * event )
{
  if(event->button()== Qt::LeftButton){
    global_mouse_pos_=event->globalPos();
    return;
  }
  QGraphicsView::mousePressEvent(event);
}
void PlotViewerPanel::mouseMoveEvent( QMouseEvent * event )
{
  if(event->buttons() & Qt::LeftButton){
    OnTranslate(static_cast<Real>(event->globalPos().x()-global_mouse_pos_.x()),static_cast<Real>(event->globalPos().y()-global_mouse_pos_.y()));
    global_mouse_pos_=event->globalPos();
    return;
  }
  QGraphicsView::mouseMoveEvent(event);
}

QTransform PlotViewerPanel::GetTransform(QRectF scenerect, QRect viewrect)
{
  qreal vx1,vx2,vy1,vy2;
  qreal x1,x2,y1,y2;
  scenerect.getCoords(&x1,&y1,&x2,&y2);;
  QRectF(viewrect).getCoords(&vx1,&vy1,&vx2,&vy2);
  Real sx=(vx2-vx1)/(x2-x1);
  Real sy=(vy2-vy1)/(y2-y1);
  Real dx=vx1-sx*x1;
  Real dy=vy1-sy*y1;
  return QTransform(sx,0.0,0.0,
                    0.0,sy,0.0,
                    dx,dy,1.0);
}

void PlotViewerPanel::SetAutofit(bool fit)
{
  autofit_=fit;
}

}}//ns
