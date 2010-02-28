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
  Authors: Ansgar Philippsen, Johan Hebert, Andreas Schenk
*/

#include <ost/img/alg/norm.hh>
#include <ost/img/alg/stat_min_max.hh>

//#include "bitmaps/move.xpm"
//#include "bitmaps/crosshair.xpm"
//#include "bitmaps/rotate.xpm"
//#include "bitmaps/arrowmove.xpm"

#include <QResizeEvent>
#include <QPainter>
#include <QColor>
#include <QFont>
#include <QPixmapCache>
#include <ost/message.hh>

#include "img2qt.hh"
#include "data_viewer_panel_base.hh"

#define USE_PIXMAP_CACHE

namespace ost { namespace img { namespace gui {

DataViewerPanelBase::DataViewerPanelBase(const Data& data,QWidget* parent):
  QWidget(parent),
  DataObserver(data),
  popupmenu_(new QMenu),
  data_min_(0.0),
  data_max_(0.0),
  data_min_pos_(),
  data_max_pos_(),
  normalizer_(ViewerNormalizerPtr(new ViewerNormalizer)),
  image_(new QImage(1,1,QImage::Format_RGB32)),
  pixmap_(new QPixmap(QPixmap::fromImage(*image_))),
  zoom_level_(0),
  update_raster_image_(false),
  center_x_(0.0),
  center_y_(0.0),
  offset_x_(0.0),
  offset_y_(0.0),
  clicked_position_(),
  show_clicked_position_(false),
  zoom_scale_(1.0),
  i_zoom_scale_(1.0),
  slab_(0),
  cursor_(Qt::CrossCursor),
  display_pixel_values_(true),
  mouseposition_(),
  last_x_(0),last_y_(0),
  right_press_x_(0),right_press_y_(0),
  selection_dragging_(false),
  selection_rect_(),
  selection_(),
  selection_pen_(QPen(QColor(255,255,0),1,Qt::DashLine)),
  selection_brush_(QBrush(Qt::NoBrush)),
  selection_state_(false),
  selection_mode_(0),
  cmode_(RasterImage::GREY),
  cursors_(),
  use_update_extent_(false),
  update_extent_(),
  fast_low_mag_(true),
  fast_high_mag_(true),
  antialiasing_(true)
{
  update_min_max();
  UpdateNormalizer(data_min_,data_max_,1.0,false);
  // placeholder for image
  Recenter();
  slab_=GetObservedData().GetExtent().GetCenter()[2];

  emit slabChanged(slab_);

  setMinimumSize(200,200);
  on_resize(size().width(),size().height()); // needed for proper initialization

  setMouseTracking(true);

  //TODO cursors
  setCursor(cursor_);
  /*
  QMenu* m_zoom = new QMenu();
  m_zoom->Append(ID_ZOOM_PLUS,QT("Zoom In"));
  m_zoom->Append(ID_ZOOM_MINUS,QT("Zoom Out"));
  m_zoom->AppendSeparator();
  m_zoom->Append(ID_ZOOM_1,QT("Original Size"));

  QMenu* m_slab = new QMenu();
  m_slab->Append(ID_SLAB_PLUS,QT("Next Slab"));
  m_slab->Append(ID_SLAB_MINUS,QT("Previous Slab"));
  m_slab->AppendSeparator();
  m_slab->Append(ID_SLAB_CENTER,QT("Center Slab"));
  m_slab->Append(ID_SLAB_START,QT("First Slab"));
  m_slab->Append(ID_SLAB_END,QT("Last Slab"));
  
  popupmenu_->Append(ID_NORMALIZE,QT("Normalize"));
  popupmenu_->Append(ID_CENTER,QT("Center image"));
  popupmenu_->AppendSeparator();
  popupmenu_->Append(ID_ZOOM,QT("Zoom"),m_zoom);
  popupmenu_->Append(ID_SLAB,QT("Slab"),m_slab);
  popupmenu_->AppendSeparator();
  popupmenu_->AppendCheckItem(ID_PHASECOLOR,QT("Phase Color Display"));
  popupmenu_->AppendCheckItem(ID_INVERT,QT("Invert"));
  popupmenu_->AppendCheckItem(ID_DISP_PIXEL_VAL,QT("Display pixel values"));
  popupmenu_->AppendCheckItem(ID_SHOW_CLICK_POS,QT("Show last clicked point"));
  */
}

DataViewerPanelBase::~DataViewerPanelBase()
{
  delete image_;
  delete pixmap_;
}

void DataViewerPanelBase::SetData(const Data& d)
{
  SetObservedData(d);
  update_min_max();
  UpdateView(true);
}

void DataViewerPanelBase::UpdateView(bool update_raster_image)
{
  // schedule update for next paint event
  this->update();

  if(update_raster_image_) return;
  update_raster_image_=update_raster_image;
}

ViewerNormalizerPtr DataViewerPanelBase::GetNormalizer() const 
{
  return normalizer_;
}

#if 0
void DataViewerPanelBase::OnMenu(QCommandEvent& e)
{
  switch(e.GetId()){
  case ID_PHASECOLOR:
    if(e.IsChecked()){
      SetColorMode(RasterImage::PHASECOLOR);
    }else{
      SetColorMode(RasterImage::GREY);
    }
    break;
  case ID_NORMALIZE:
    Renormalize();
    break;
  case ID_CENTER:
    Recenter();
    break;
  case ID_INVERT:
    SetInvert(!GetInvert());
    break;
  case ID_DISP_PIXEL_VAL:
    SetDisplayPixelValues(!display_pixel_values_);
    break;
  case ID_SHOW_CLICK_POS:
    ShowClickedPosition(!show_clicked_position_);
    break;
  case ID_ZOOM_PLUS:
    SetZoomScale(2.0*GetZoomScale());
    break;
  case ID_ZOOM_MINUS:
    SetZoomScale(0.5*GetZoomScale());
    break;
  case ID_ZOOM_1:
    SetZoomScale(1.0);
    break;
  case ID_SLAB_PLUS:
    SetSlab(GetSlab()+1);
    break;
  case ID_SLAB_MINUS:
    SetSlab(GetSlab()-1);
    break;
  case ID_SLAB_START:
    SetSlab(GetObservedData().GetExtent().GetStart()[2]);
    break;
  case ID_SLAB_END:
    SetSlab(GetObservedData().GetExtent().GetEnd()[2]);
    break;
  case ID_SLAB_CENTER:
    SetSlab(static_cast<int>((GetObservedData().GetExtent().GetEnd()[2]
                             -GetObservedData().GetExtent().GetStart()[2])/2.0));
    break;
  default:
    e.Skip();
    break;
  }
}
void DataViewerPanelBase::OnUpdateMenu(QUpdateUIEvent& e)
{
  switch(e.GetId()){
  case ID_PHASECOLOR:
    e.Check(GetColorMode()==RasterImage::PHASECOLOR);
    break;
  case ID_INVERT:
    e.Check(GetInvert());
    break;
  case ID_DISP_PIXEL_VAL:
    e.Check(display_pixel_values_);
    break;
  case ID_SHOW_CLICK_POS:
    e.Check(show_clicked_position_);
    break;
  default:
    e.Skip();
    break;
  }
}
#endif

void DataViewerPanelBase::Renormalize() 
{
  if(!IsDataValid()) return;
  if(selection_state_) {    
    alg::StatMinMax s;
    ImageHandle tmp=CreateImage(GetSelection());
    tmp.Paste(GetObservedData());
    tmp.Apply(s);
    UpdateNormalizer(s.GetMinimum(),s.GetMaximum(),normalizer_->GetGamma(),normalizer_->GetInvert());
  } else {
    UpdateNormalizer(data_min_,data_max_,normalizer_->GetGamma(),normalizer_->GetInvert());
  }
  UpdateView(true);
}




// event handling
void DataViewerPanelBase::ObserverUpdate()
{
  update_min_max();
  UpdateNormalizer(data_min_,data_max_,normalizer_->GetGamma(),normalizer_->GetInvert());
  UpdateView(true);

  /*
  QCommandEvent ev=QCommandEvent(QEVT_VIEWER_DATA_CHANGE,GetId());
  ev.SetEventObject(this);
  AddPendingEvent(ev);
  */
}

void DataViewerPanelBase::ObserverUpdate(const Extent& e)
{
  if(!IsDataValid()) return;
  bool update_norm=false;
  if((e.Contains(data_min_pos_) || e.Contains(data_max_pos_))){
    ObserverUpdate();
  }else{
    ImageHandle subimage=CreateImage(e);
    subimage.Paste(GetObservedData());

    alg::StatMinMax s;
    subimage.Apply(s);
    if(s.GetMinimum()<data_min_){
      data_min_=s.GetMinimum();
      data_min_pos_=s.GetMinimumPosition();
      update_norm=true;
    }
    if(s.GetMaximum()>data_max_){
      data_max_=s.GetMaximum();
      data_max_pos_=s.GetMaximumPosition();
      update_norm=true;
    }
    if(update_norm){
      UpdateNormalizer(data_min_,data_max_,normalizer_->GetGamma(),
                       normalizer_->GetInvert());
      use_update_extent_=false;
    }else{
      if(use_update_extent_){
        update_extent_=Extent(Point(std::min(update_extent_.GetStart()[0],e.GetStart()[0]),
                                    std::min(update_extent_.GetStart()[1],e.GetStart()[1])),
                                    Point(std::max(update_extent_.GetEnd()[0],e.GetEnd()[0]),
                                    std::max(update_extent_.GetEnd()[1],e.GetEnd()[1])));
        
      }else{
        if(!update_raster_image_){
          use_update_extent_=true;
          update_extent_=e;
        }
      }
    }
    UpdateView(true);
  }
}

void DataViewerPanelBase::ObserverUpdate(const Point& p)
{
  if(!IsDataValid()) return;
  bool update_norm=false;
  if(p==data_min_pos_ || p==data_max_pos_){
    ObserverUpdate();
  }else{
    Real val=GetObservedData().GetReal(p);
   if(val<data_min_){
      data_min_=val;
      data_min_pos_=p;
      update_norm=true;
    }
    if(val>data_max_){
      data_max_=val;
      data_max_pos_=p;
      update_norm=true;
    }

    if(update_norm){
      UpdateNormalizer(data_min_,data_max_,normalizer_->GetGamma(),normalizer_->GetInvert());
      use_update_extent_=false;
    }else{
      if(use_update_extent_){
        update_extent_=Extent(Point(std::min(update_extent_.GetStart()[0],p[0]),
                                          std::min(update_extent_.GetStart()[1],p[1])),
                              Point(std::max(update_extent_.GetEnd()[0],p[0]),
                                          std::max(update_extent_.GetEnd()[1],p[1])));
        
      }else{
        if(!update_raster_image_){
          use_update_extent_=true;
          update_extent_=Extent(p,p);
        }
      }
    }
    UpdateView(true);
  }
}

void DataViewerPanelBase::ObserverRelease()
{
  emit released();
  /*
  QCommandEvent ev=QCommandEvent(QEVT_VIEWER_RELEASE,GetId());
  ev.SetEventObject(this);
  AddPendingEvent(ev);
  */
}

void DataViewerPanelBase::resizeEvent(QResizeEvent* event)
{
  on_resize(event->size().width(),event->size().height());
}

void DataViewerPanelBase::paintEvent(QPaintEvent* event)
{
  static const int blocksize=256;

  if(!IsDataValid()) return;
  QPainter painter(this);
  painter.setRenderHints(QPainter::Antialiasing | QPainter::TextAntialiasing,
                         antialiasing_);
#ifdef USE_PIXMAP_CACHE
  if(update_raster_image_) {
    QPixmapCache::clear();
  }

  int uc=1+static_cast<int>(floor(static_cast<Real>(size().width())/static_cast<Real>(blocksize)));
  int vc=1+static_cast<int>(floor(static_cast<Real>(size().height())/static_cast<Real>(blocksize)));
  
  geom::Vec2 cen=zoom_scale_*geom::Vec2(center_x_+offset_x_,
                                        center_y_+offset_y_)/static_cast<Real>(blocksize);
  Point i(static_cast<int>(floor(cen[0])),
          static_cast<int>(floor(cen[1])));
  Point i0 = i-Point((uc-1)/2-1,(vc-1)/2-1);
  Point o(static_cast<int>(static_cast<Real>(blocksize)*(cen[0]-floor(cen[0]))),
          static_cast<int>(static_cast<Real>(blocksize)*(cen[1]-floor(cen[1]))));
  
  painter.setPen(QColor(255,255,255));
  for(int v=0;v<=vc;++v) {
    for(int u=0;u<=uc;++u) {
      QString cache_key = QString("%1,%2,%3,%4")
        .arg(reinterpret_cast<unsigned long>(this))
        .arg(i0[0]+u)
        .arg(i0[1]+v)
        .arg(zoom_level_);
      QPixmap pm;
      if (update_raster_image_ || !QPixmapCache::find(cache_key, pm)) {
        RasterImage ri(blocksize,blocksize);
        ri.Fill(GetObservedData(),zoom_level_,
                geom::Vec3(WinToFracPoint(u*blocksize-o[0],v*blocksize-o[1])),
                slab_,normalizer_,cmode_,fast_low_mag_,fast_high_mag_);
        QImage im(blocksize,blocksize,QImage::Format_RGB32);
        for(int b=0;b<blocksize;++b) {
          for(int a=0;a<blocksize;++a) {
            RasterImage::Pixel p=ri.GetPixel(a,b);
            im.setPixel(a,b,qRgb(p.r,p.g,p.b));
          }
        }
        pm=QPixmap::fromImage(im);
        QPixmapCache::insert(cache_key,pm);
      }
      painter.drawPixmap(u*blocksize-o[0],v*blocksize-o[1],pm);
      //painter.drawRect(u*blocksize-o[0],v*blocksize-o[1],blocksize,blocksize);
    }
  }
  update_raster_image_=false;
#else
  if(update_raster_image_) {
    image_->fill(127);
    extract_ri();
    update_raster_image_=false;
    delete pixmap_;
    pixmap_ = new QPixmap(QPixmap::fromImage(*image_));
  }
  // this is an optimization for overlays
  //painter.drawImage(QPoint(0,0),*image_);
  painter.drawPixmap(QPoint(0,0),*pixmap_);
#endif

  if(zoom_level_ >= 6 && display_pixel_values_) {
    draw_pixel_values(painter);
  }

  if(zoom_level_<6) {
    draw_extent(painter);
  }

  if(selection_dragging_ || selection_state_) {
    painter.setPen(selection_pen_);
    painter.setBrush(selection_brush_);
    painter.drawRect(selection_rect_);
  }
}

void DataViewerPanelBase::keyPressEvent(QKeyEvent * event)
{
  if(!IsDataValid()) return;
  int step=static_cast<int>(zoom_scale_);
  switch(event->key()) {
  case Qt::Key_Left: move(-step,0); break;
  case Qt::Key_Right: move(step,0); break;
  case Qt::Key_Up: move(0,-step); break;
  case Qt::Key_Down: move(0,step); break;
  case '-':
  case 'L':
    zoom(-1); break;
  case '=':
  case 'K': 
    zoom(+1); break;
  case 'C': Recenter(); break;
  case 'I': 
    SetInvert(!GetInvert());
    break;
  case 'N': Renormalize(); break;
  case 'Z':
  case 'Y': slab(+1); break;
  case 'X': slab(-1); break;
  case 'S': selection_mode_=(selection_mode_+1)%3; break;
  case 'V': SetDisplayPixelValues(!display_pixel_values_);break;
  case 'P': 
    if(GetColorMode()==RasterImage::PHASECOLOR){
      SetColorMode(RasterImage::GREY);
    }else{
      SetColorMode(RasterImage::PHASECOLOR);
    }
    break;
  }
}

void DataViewerPanelBase::mousePressEvent(QMouseEvent* event)
{
  if(!IsDataValid()) return;
  if(event->button() == Qt::LeftButton && event->modifiers()==Qt::NoModifier) {
    selection_state_=false;
    selection_dragging_=false;
    selection_=Extent();
    selection_rect_.setX(event->x());
    selection_rect_.setY(event->y());
    selection_rect_.setWidth(0);
    selection_rect_.setHeight(0);
    last_x_=event->x();
    last_y_=event->y();
    QPoint cp=FracPointToWin(geom::Vec2(clicked_position_));
    clicked_position_ = geom::Vec3(WinToFracPoint(QPoint(event->x(),event->y())));
    clicked_position_[2]=slab_;
    UpdateView(false);
    
    emit clicked(clicked_position_);

  }
  if(event->button() == Qt::RightButton && event->modifiers()==Qt::NoModifier) {
    setCursor(Qt::ClosedHandCursor);
    right_press_x_=event->x();
    right_press_y_=event->y();
  }
  last_x_=event->x();
  last_y_=event->y();
}

void DataViewerPanelBase::mouseReleaseEvent(QMouseEvent* event)
{
  if(!IsDataValid()) return;
  if(event->button() == Qt::LeftButton && event->modifiers()==Qt::NoModifier) {
    if(selection_dragging_) {
      selection_dragging_=false;
      selection_state_=true;
      UpdateView(false);
      emit selected(selection_);
    } else {
      UpdateView(false);
      emit deselected();
    }
  } else if(event->button() & Qt::RightButton) {
    setCursor(cursor_);
    if(right_press_x_==event->x() && right_press_y_==event->y()) {
      popupmenu_->popup(mapToGlobal(event->pos()));
    }
  }
  last_x_=event->x();
  last_y_=event->y();
}

void DataViewerPanelBase::mouseMoveEvent(QMouseEvent* event)
{
  if(!IsDataValid()) return;
  static Point drag_start;
  if(event->buttons() == Qt::RightButton && event->modifiers()==Qt::NoModifier) {
    // right mouse drag translates pic
    int dx = event->x()-last_x_;
    int dy = event->y()-last_y_;
    move(dx,dy);
  } else if(event->buttons() == Qt::LeftButton  && event->modifiers()==Qt::NoModifier) {
    // left mouse drag does selection box
    if(!selection_dragging_){
      drag_start=WinToPoint(event->x(),event->y());
    }
    selection_dragging_=true;
    QSize vsize=size();
    Point mouse_pos=WinToPoint(event->x(),event->y());
    Point max_pos=WinToPoint(vsize.width(),vsize.height());
    Point min_pos=WinToPoint(0,0);
    selection_=Extent(Point(std::max(min_pos[0],std::min(drag_start[0],mouse_pos[0])),
                            std::max(min_pos[1],std::min(drag_start[1],mouse_pos[1]))),
                      Point(std::max(drag_start[0],std::min(max_pos[0],mouse_pos[0])),
                            std::max(drag_start[1],std::min(max_pos[1],mouse_pos[1]))));
    if(selection_mode_>0){
      Size s=selection_.GetSize();
      int minsize=std::min<int>(s[0],s[1]);
      if(selection_mode_==2){
        minsize=1<<static_cast<int>(floor(log(Real(minsize))/log(Real(2.0))));
      }
      if(drag_start[0]>mouse_pos[0] && drag_start[1]>mouse_pos[1]){
        selection_.SetStart(drag_start-Point(minsize,minsize)+Point(1,1));
        selection_.SetEnd(drag_start);        
      }else if(drag_start[0]>mouse_pos[0] && drag_start[1]<=mouse_pos[1]){
        selection_.SetStart(drag_start-Point(minsize,0)+Point(1,0));
        selection_.SetEnd(drag_start+Point(0,minsize)-Point(0,1));        
      }else if(drag_start[0]<=mouse_pos[0] && drag_start[1]>mouse_pos[1]){
        selection_.SetStart(drag_start-Point(0,minsize)+Point(0,1));        
        selection_.SetEnd(drag_start+Point(minsize,0)-Point(1,0));
      }else{
        selection_.SetEnd(selection_.GetStart()+Point(minsize,minsize)-Point(1,1));
      }
    }
    QRect new_selection_rect_(PointToWin(selection_.GetStart()),PointToWin(selection_.GetEnd()+Point(1,1)));
    selection_rect_=new_selection_rect_;
    UpdateView(false);
  } else if((event->buttons() == Qt::MidButton) && HasSelection()) {
    int dx=static_cast<int>(i_zoom_scale_*(event->x()-last_x_));
    int dy=static_cast<int>(i_zoom_scale_*(event->y()-last_y_));
    selection_.SetStart(selection_.GetStart()+Point(dx,dy));
    QRect new_selection_rect_(PointToWin(selection_.GetStart()),PointToWin(selection_.GetEnd()+Point(1,1)));
    selection_rect_=new_selection_rect_;
    UpdateView(false);
    emit selected(Extent(Point(selection_rect_.left(),
                                           selection_rect_.top()),
                                Point(selection_rect_.bottom(),
                                            selection_rect_.right())));
  }
  last_x_=event->x();
  last_y_=event->y();
  mouseposition_=WinToFracPoint(QPoint(last_x_,last_y_));
}

void DataViewerPanelBase::wheelEvent(QWheelEvent* event)
{
  if(!IsDataValid()) return;
  if(event->delta()>0) {
    zoom(-1);
  } else {
    zoom(1);
  }
}

void DataViewerPanelBase::enterEvent(QEvent* event)
{
  if(!IsDataValid()) return;
  setFocus();
}

void DataViewerPanelBase::SetSelectionMode(int mode)
{
  selection_mode_=mode%3;
}
int DataViewerPanelBase::GetSelectionMode()
{
  return selection_mode_;
}

geom::Vec2 DataViewerPanelBase::WinToFracPoint(const QPoint& p) const
{
  return WinToFracPoint(p.x(),p.y());
}

geom::Vec2 DataViewerPanelBase::WinToFracPointCenter(const QPoint& p) const
{
  return WinToFracPointCenter(p.x(),p.y());
}

geom::Vec2 DataViewerPanelBase::WinToFracPoint(int mx, int my) const
{
  Real px = i_zoom_scale_*static_cast<Real>(mx)+(center_x_+offset_x_);
  Real py = i_zoom_scale_*static_cast<Real>(my)+(center_y_+offset_y_);

  return geom::Vec2(px,py);
}

geom::Vec2 DataViewerPanelBase::WinToFracPointCenter(int mx, int my) const
{
  Real px = i_zoom_scale_*static_cast<Real>(mx)+(center_x_+offset_x_)-0.5;
  Real py = i_zoom_scale_*static_cast<Real>(my)+(center_y_+offset_y_)-0.5;

  return geom::Vec2(px,py);
}

Point DataViewerPanelBase::WinToPoint(const QPoint& p) const
{
  return WinToPoint(p.x(),p.y());
}

Point DataViewerPanelBase::WinToPoint(int mx, int my) const
{
  geom::Vec2 v=WinToFracPoint(mx,my);
  return Point(static_cast<int>(floor(v[0])),static_cast<int>(floor(v[1])),slab_);
}

QPoint DataViewerPanelBase::PointToWin(const Point& p) const
{
  return FracPointToWin(geom::Vec2(static_cast<Real>(p[0]),static_cast<Real>(p[1])));
}

QPoint DataViewerPanelBase::FracPointToWin(const geom::Vec2& v) const
{
  return QPoint(static_cast<int>(zoom_scale_*(v[0]-(center_x_+offset_x_))),
		static_cast<int>(zoom_scale_*(v[1]-(center_y_+offset_y_))));
}

QPoint DataViewerPanelBase::FracPointToWinCenter(const geom::Vec2& v) const
{
  return QPoint(static_cast<int>(zoom_scale_*(v[0]+0.5-(center_x_+offset_x_))),
		static_cast<int>(zoom_scale_*(v[1]+0.5-(center_y_+offset_y_))));
}

Extent DataViewerPanelBase::GetSelection() const 
{
  return selection_;
}

Real DataViewerPanelBase::GetZoomScale() const 
{
  return zoom_scale_;
}

void DataViewerPanelBase::SetZoomScale(Real zoomscale) 
{
  zoom(static_cast<int>(log2(zoomscale)-log2(zoom_scale_)));
}

Extent DataViewerPanelBase::GetExtent() 
{
  if(!IsDataValid()) return Extent();
  return GetObservedData().GetExtent();
}

geom::Vec3 DataViewerPanelBase::GetPixelSampling() 
{
  if(!IsDataValid()) return geom::Vec3();
  return GetObservedData().GetPixelSampling();
}


void DataViewerPanelBase::SetDefaultCursor(int id)
{
}

void DataViewerPanelBase::SetDefaultCursor(const QCursor& cursor)
{
}

void DataViewerPanelBase::SetCursor(int id)
{
}

void DataViewerPanelBase::SetCursor(const QCursor& cursor)
{
}

void DataViewerPanelBase::SetDisplayPixelValues(bool show)
{
  display_pixel_values_ = show;
  UpdateView(false);
}

void DataViewerPanelBase::Recenter()
{
  if(!IsDataValid()) return;
  if (!HasSelection())  {
    if(GetObservedData().IsSpatial()) {
    
      Point cen = GetObservedData().GetSpatialOrigin();
      Size siz = GetObservedData().GetSize();

      center_x_=static_cast<Real>(siz[0])/2.0+static_cast<Real>(cen[0]);
      center_y_=static_cast<Real>(siz[1])/2.0+static_cast<Real>(cen[1]);
    } else {
      center_x_=0.0;
      center_y_=0.0;
    }

    offset_x_ = -center_x_;
    offset_y_ = -center_y_;
  
  } else {
    Extent selection = GetSelection();
    center_x_=static_cast<Real>(selection.GetCenter()[0]);
    center_y_=static_cast<Real>(selection.GetCenter()[1]);
  }
  zoom(0);// force refresh without changing zoom level
}

bool DataViewerPanelBase::IsWithin(const QPoint& qp) const
{
  return qp.x()>=0 && qp.x()<size().width() && qp.y()>=0 && qp.y()<size().height();
}

// private methods


void DataViewerPanelBase::draw_extent(QPainter& painter)
{
  if(!IsDataValid()) return;
  painter.setPen(QPen(QColor(255,255,255),1));
  painter.setBrush(QBrush(Qt::NoBrush));

  painter.drawRect(QRect(PointToWin(GetObservedData().GetExtent().GetStart()),
                         PointToWin(GetObservedData().GetExtent().GetEnd()+Point(1,1))));
}

void DataViewerPanelBase::set_clippingregion(QPainter& painter)
{
  if(!IsDataValid()) return;
  QPoint p1=PointToWin(GetObservedData().GetExtent().GetStart());
  QPoint p2=PointToWin(GetObservedData().GetExtent().GetEnd()+Point(1,1));
  //painter.SetClippingRegion(p1.x,p1.y,p2.x-p1.x,p2.y-p1.y);
}


void DataViewerPanelBase::extract_ri()
{
  if(!IsDataValid()) return;
  
  use_update_extent_=false; // hack

  if(use_update_extent_){
    use_update_extent_=false;
    int w=size().width();
    int h=size().height();
    QPoint tl=PointToWin(update_extent_.GetStart());
    QPoint br=PointToWin(update_extent_.GetEnd()+Point(1,1));
    RasterImage ri(w,h,image_->bits());
    ri.Fill(GetObservedData(),
            zoom_level_, 
            geom::Vec3(center_x_+offset_x_, center_y_+offset_y_, 0.0),
            slab_,
            normalizer_,cmode_,fast_low_mag_,fast_high_mag_,
            std::min(w,std::max(0,tl.x())),std::min(h,std::max(0,tl.y())),
            std::min(w,std::max(0,br.x()-1)),std::min(h,std::max(0,br.y()-1)));
  }else{
    if(zoom_level_==0) {
      img2qt(GetObservedData(),image_,
              zoom_level_,
              geom::Vec3(center_x_+offset_x_, center_y_+offset_y_, 0.0),
              slab_,
              normalizer_);
    } else {
      RasterImage ri(size().width(),size().height());
      ri.Fill(GetObservedData(),
              zoom_level_, 
              geom::Vec3(center_x_+offset_x_, center_y_+offset_y_, 0.0),
              slab_,
              normalizer_,cmode_,fast_low_mag_,fast_high_mag_);
      
      for(int h=0;h<size().height();++h) {
        for(int w=0;w<size().width();++w) {
          RasterImage::Pixel p=ri.GetPixel(w,h);
          image_->setPixel(w,h,qRgb(p.r,p.g,p.b));
        }
      }
    }
  }
}

void DataViewerPanelBase::zoom(int d)
{
  if((d>0 && INT_MAX-d >zoom_level_)|| (d<0 && INT_MIN-d <zoom_level_)){
    zoom_level_+=d;
    // maximal zoom = 2^8
    zoom_level_=std::min(zoom_level_,8);
    zoom_level_=std::max(zoom_level_,-8);
    
  }

  zoom_scale_ = ldexp(1.0,zoom_level_);
  i_zoom_scale_ = 1.0/zoom_scale_;

  offset_x_ = -0.5*i_zoom_scale_*static_cast<Real>(size().width());
  offset_y_ = -0.5*i_zoom_scale_*static_cast<Real>(size().height());

  selection_rect_=QRect(PointToWin(selection_.GetStart()),
                        PointToWin(selection_.GetEnd()+Point(1,1)));
#ifdef USE_PIXMAP_CACHE
  UpdateView(false);
#else
  UpdateView(true);
#endif
  emit zoomed(zoom_level_);
}


void DataViewerPanelBase::move(int dx, int dy)
{
  center_x_-=i_zoom_scale_*(Real)dx;
  center_y_-=i_zoom_scale_*(Real)dy;
  selection_rect_=QRect(PointToWin(selection_.GetStart()),
                        PointToWin(selection_.GetEnd()+Point(1,1)));
#ifdef USE_PIXMAP_CACHE
  UpdateView(false);
#else
  UpdateView(true);
#endif
}

void DataViewerPanelBase::MoveTo(const geom::Vec2& p)
{
  center_x_=p[0];
  center_y_=p[1];

#ifdef USE_PIXMAP_CACHE
  UpdateView(false);
#else
  UpdateView(true);
#endif
}

int DataViewerPanelBase::GetSlab()
{
  return slab_;
}

void DataViewerPanelBase::SetSlab(int s)
{
  slab(s-slab_);
}

void DataViewerPanelBase::slab(int dz)
{
  if(!IsDataValid()) return;
  slab_+=dz;
  slab_=std::max(slab_,GetObservedData().GetExtent().GetStart()[2]);
  slab_=std::min(slab_,GetObservedData().GetExtent().GetEnd()[2]);
  emit slabChanged(slab_);
  UpdateView(true);
}

void DataViewerPanelBase::SetColorMode(RasterImage::Mode m) 
{
  cmode_=m;
  UpdateView(true);
}

void DataViewerPanelBase::on_resize(int width, int height)
{
#ifdef USE_PIXMAP_CACHE
  UpdateView(true);
#else
  delete image_;
  image_ = new QImage(width,height,QImage::Format_RGB32);
#endif
  zoom(0);
}

ImageHandle DataViewerPanelBase::Extract(const Extent& e)
{
  ImageHandle im=CreateImage(e);
  if(IsDataValid()) {
    im.Paste(GetObservedData());
  }
  return im;
}


void DataViewerPanelBase::ShowClickedPosition(bool show = true)
{
  show_clicked_position_ = show;
  UpdateView(false);
}

void DataViewerPanelBase::HideClickedPosition()
{
  ShowClickedPosition(false);
}

bool DataViewerPanelBase::ClickedPositionIsVisible()
{
  return show_clicked_position_;
}

geom::Vec3 DataViewerPanelBase::GetClickedPosition()
{
  return clicked_position_;
}

bool DataViewerPanelBase::HasSelection()
{
  return selection_state_;
}

Real DataViewerPanelBase::GetDataMin() const
{
  return data_min_;
}

Real DataViewerPanelBase::GetDataMax() const
{
  return data_max_;
}


void DataViewerPanelBase::UpdateNormalizer(Real min, Real max, Real gamma,
                                           bool invert)
{
  normalizer_=ViewerNormalizerPtr(new ViewerNormalizer(min,max,0.0,255.0,gamma,invert));
  UpdateView(true);
}

void DataViewerPanelBase::update_min_max()
{
  if(!IsDataValid()) return;
  alg::StatMinMax s;

  GetObservedData().Apply(s);
  data_min_=s.GetMinimum();
  data_max_=s.GetMaximum();
  data_min_pos_=s.GetMinimumPosition();
  data_max_pos_=s.GetMaximumPosition();
}

bool DataViewerPanelBase::GetInvert() const
{
  return normalizer_->GetInvert();
}

void DataViewerPanelBase::SetInvert(bool invert)
{
  UpdateNormalizer(normalizer_->GetMinimum(),
                   normalizer_->GetMaximum(),
                   normalizer_->GetGamma(),invert);
  UpdateView(true);
}

void DataViewerPanelBase::SetAntialiasing(bool f)
{
  antialiasing_=f;
  update();
}

bool DataViewerPanelBase::GetAntialiasing() const
{
  return antialiasing_;
}

void DataViewerPanelBase::draw_pixel_values(QPainter& painter)
{
  if(!IsDataValid()) return;
  Extent visible = Extent(WinToPoint(0, 0), 
                                      WinToPoint(this->size().width(),
                                                 this->size().height()));
  Point a = visible.GetStart();
  Point b = visible.GetEnd();

  QFont fnt("Courier",(zoom_level_-6)*2+7);
  painter.setFont(fnt);
  
  for(int i = 0; i <= (b[0] - a[0]); ++i) {
    for(int j = 0; j <= (b[1] - a[1]); ++j) {
      Real pixel_value = GetObservedData().GetReal(Point(a[0]+i, 
                                                               a[1]+j,slab_));
      QString value_string = QString("%1").arg(pixel_value,0,'g',5);
      Real rv = GetNormalizer()->Convert(pixel_value);
      QPoint p = FracPointToWinCenter(geom::Vec2(static_cast<Real>(a[0]+i),
                                                 static_cast<Real>(a[1]+j)));
      unsigned char rgb = (rv>130.0) ? 0 : 255;
      painter.setPen(QColor(rgb,rgb,rgb));
      int string_h=painter.fontMetrics().height();
      int string_w=painter.fontMetrics().width(value_string);
      painter.drawText(p.x() - string_w/2, p.y() + string_h/2, value_string);
    }
  }
}

}}}  //ns
