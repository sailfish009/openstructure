//------------------------------------------------------------------------------
// This file is part of the OpenStructure project <www.openstructure.org>
//
// Copyright (C) 2008-2011 by the OpenStructure authors
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

#include "img2qt.hh"
#include "data_viewer_panel_base.hh"

#include <QResizeEvent>
#include <QPainter>
#include <QColor>
#include <QFont>
#include <QPixmapCache>
#include <ost/message.hh>

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
  rubberband_(new QRubberBand(QRubberBand::Rectangle,this)),
  zoom_level_(0),
  update_raster_image_(false),
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
  selection_(),
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
  slab_=GetObservedData().GetExtent().GetCenter()[2];

  emit slabChanged(slab_);

  Recenter();
  setMouseTracking(true);
  setFocusPolicy(Qt::WheelFocus);
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
  QPixmapCache::setCacheLimit(51200);

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
  Recenter();
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
  if(HasSelection()) {    
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
  if(event->oldSize().width()<0 || event->oldSize().height()<0){
   Recenter();
   }else{
    offset_x_+=static_cast<Real>((event->oldSize().width()-event->size().width()))*0.5*i_zoom_scale_;
    offset_y_+=static_cast<Real>((event->oldSize().height()-event->size().height()))*0.5*i_zoom_scale_;
    UpdateView(false);
  }
}

void DataViewerPanelBase::paintEvent(QPaintEvent* event)
{
  static const int blocksize=128;

  if(!IsDataValid() ) return;
  QPainter painter(this);
  painter.setRenderHints(QPainter::Antialiasing | QPainter::TextAntialiasing, antialiasing_);
  if(update_raster_image_) {
    QPixmapCache::clear();
  }
  painter.fillRect(0,0,width(),height(),Qt::black);
  Size imsize=GetObservedData().GetSize();
  Point start=GetObservedData().GetExtent().GetStart();
  geom::Vec2 cen=zoom_scale_*(geom::Vec2(offset_x_,offset_y_)-start.ToVec2())/static_cast<Real>(blocksize);
  // o = offet of the block grid compared to (0,0)
  Point o(static_cast<int>(static_cast<Real>(blocksize)*(cen[0]-floor(cen[0]))),
          static_cast<int>(static_cast<Real>(blocksize)*(cen[1]-floor(cen[1]))));

  Point i(static_cast<int>(floor(cen[0])),static_cast<int>(floor(cen[1])));
  int ustart=std::max<int>(0,std::max<int>((event->rect().left()+o[0])/blocksize,-i[0]));
  int vstart=std::max<int>(0,std::max<int>((event->rect().top()+o[1])/blocksize,-i[1]));
  int uend=std::min<int>((event->rect().right()+o[0])/blocksize,
                  static_cast<int>(zoom_scale_*imsize[0]+1)/blocksize-i[0]);
  int vend=std::min<int>((event->rect().bottom()+o[1])/blocksize,
                  static_cast<int>(zoom_scale_*imsize[1]+1)/blocksize-i[1]);
  painter.setPen(QColor(255,255,255));
  for(int v=vstart;v<=vend;++v) {
    for(int u=ustart;u<=uend;++u) {
      QString cache_key = QString("%1,%2,%3,%4")
        .arg(reinterpret_cast<unsigned long>(this))
        .arg(i[0]+u)
        .arg(i[1]+v)
        .arg(zoom_level_);
      QPixmap pm;
      #ifdef __APPLE__
      // Partial fix for buggy MDI on OSX. QMdiSubwindows hidden behind the opaque active window
      // still receive QPaintEvents. Redrawing the hidden windows may deplete the pixmap cache.
      // hasFocus() avoids the depletion but doesn't avoid the redraw.
      if (update_raster_image_ || ( !QPixmapCache::find(cache_key, pm) && hasFocus())) {
      #else
      if (update_raster_image_ || ( !QPixmapCache::find(cache_key, pm))) {
      #endif
        RasterImage ri(blocksize,blocksize);
        ri.Fill(GetObservedData(),zoom_level_,
                geom::Vec3(WinToFracPoint(u*blocksize-o[0],v*blocksize-o[1])),
                slab_,normalizer_,cmode_,fast_low_mag_,fast_high_mag_);
        QImage im(ri.GetDataPtr(),blocksize,blocksize,blocksize*3,QImage::Format_RGB888);
        pm=QPixmap::fromImage(im);
        QPixmapCache::insert(cache_key,pm);
        painter.drawPixmap(u*blocksize-o[0],v*blocksize-o[1],pm);
        // next line only used for cache debugging
        //painter.drawLine(u*blocksize-o[0],v*blocksize-o[1],(u+1)*blocksize-o[0],(v+1)*blocksize-o[1]);
      }else{
        painter.drawPixmap(u*blocksize-o[0],v*blocksize-o[1],pm);
      }
      // next line only used for cache debugging
      //painter.drawRect(u*blocksize-o[0],v*blocksize-o[1],blocksize,blocksize);
    }
  }
  update_raster_image_=false;

  if(zoom_level_ >= 7 && display_pixel_values_) {
    draw_pixel_values(painter);
  }

  if(zoom_level_<7) {
    draw_extent(painter);
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
    selection_=Extent();
    rubberband_->setGeometry(QRect(event->pos(),QSize(0,0)));
    rubberband_->hide();
    last_x_=event->x();
    last_y_=event->y();
    QPoint cp=FracPointToWin(geom::Vec2(clicked_position_));
    clicked_position_ = geom::Vec3(WinToFracPoint(QPoint(event->x(),event->y())));
    clicked_position_[2]=slab_;
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
    if(HasSelection()){
      emit selected(selection_);
    } else {
      rubberband_->hide();
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
  if(event->buttons() == Qt::RightButton && event->modifiers()==Qt::NoModifier) {
    // right mouse drag translates pic
    int dx = event->x()-last_x_;
    int dy = event->y()-last_y_;
    move(dx,dy);
  } else if(event->buttons() == Qt::LeftButton  && event->modifiers()==Qt::NoModifier) {
    static Point drag_start;
    // left mouse drag does selection box
    if(!rubberband_->isVisible()){
      drag_start=WinToPoint(rubberband_->geometry().topLeft());
      rubberband_->show();
    }
    QSize vsize=size();
    Point mouse_pos=WinToPoint(event->x(),event->y());
    Point max_pos=WinToPoint(vsize.width(),vsize.height());
    Point min_pos=WinToPoint(0,0);
    selection_=Extent(Point(std::max(min_pos[0],std::min(drag_start[0],mouse_pos[0])),
                            std::max(min_pos[1],std::min(drag_start[1],mouse_pos[1])), slab_),
                      Point(std::max(drag_start[0],std::min(max_pos[0],mouse_pos[0])),
                            std::max(drag_start[1],std::min(max_pos[1],mouse_pos[1])), slab_));
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
    update_rubberband_from_selection_();
  } else if((event->buttons() == Qt::MidButton) && HasSelection()) {
    update_rubberband_from_selection_();
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
  Real px = i_zoom_scale_*static_cast<Real>(mx)+offset_x_;
  Real py = i_zoom_scale_*static_cast<Real>(my)+offset_y_;

  return geom::Vec2(px,py);
}

geom::Vec2 DataViewerPanelBase::WinToFracPointCenter(int mx, int my) const
{
  Real px = i_zoom_scale_*static_cast<Real>(mx)+offset_x_-0.5;
  Real py = i_zoom_scale_*static_cast<Real>(my)+offset_y_-0.5;

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
  return QPoint(static_cast<int>(zoom_scale_*(v[0]-offset_x_)),
		static_cast<int>(zoom_scale_*(v[1]-offset_y_)));
}

QPoint DataViewerPanelBase::FracPointToWinCenter(const geom::Vec2& v) const
{
  return QPoint(static_cast<int>(zoom_scale_*(v[0]+0.5-offset_x_)),
		static_cast<int>(zoom_scale_*(v[1]+0.5-offset_y_)));
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
  zoom_scale_ = ldexp(1.0,zoom_level_);
  i_zoom_scale_ = 1.0/zoom_scale_;

  if (!HasSelection())  {
    if(GetObservedData().IsSpatial()) {
      Size siz = GetObservedData().GetSize();
      Point start=GetObservedData().GetExtent().GetStart();
      offset_x_ = static_cast<Real>(siz[0])/2.0+start[0]-0.5*i_zoom_scale_*static_cast<Real>(size().width());
      offset_y_ = static_cast<Real>(siz[1])/2.0+start[1]-0.5*i_zoom_scale_*static_cast<Real>(size().height());
    } else {
      offset_x_ = -0.5*i_zoom_scale_*static_cast<Real>(size().width());
      offset_y_ = -0.5*i_zoom_scale_*static_cast<Real>(size().height());
    }

  
  } else {
    Extent selection = GetSelection();
    offset_x_=static_cast<Real>(selection.GetCenter()[0])-0.5*i_zoom_scale_*static_cast<Real>(size().width());
    offset_y_=static_cast<Real>(selection.GetCenter()[1])-0.5*i_zoom_scale_*static_cast<Real>(size().height());
  }
 update_rubberband_from_selection_();
 UpdateView(false);
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
            geom::Vec3(offset_x_, offset_y_, 0.0),
            slab_,
            normalizer_,cmode_,fast_low_mag_,fast_high_mag_,
            std::min(w,std::max(0,tl.x())),std::min(h,std::max(0,tl.y())),
            std::min(w,std::max(0,br.x()-1)),std::min(h,std::max(0,br.y()-1)));
  }else{
    if(zoom_level_==0) {
      img2qt(GetObservedData(),image_,
              zoom_level_,
              geom::Vec3(offset_x_,offset_y_, 0.0),
              slab_,
              normalizer_);
    } else {
      RasterImage ri(size().width(),size().height());
      ri.Fill(GetObservedData(),
              zoom_level_, 
              geom::Vec3(offset_x_, offset_y_, 0.0),
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
  // maximal zoom = 2^8, therefore zoom_level_ goes from 8 to 8 and delta from -16 to 16
  int old_zoom_level=zoom_level_;
  int dl=std::max(d,-16);
  dl=std::min(dl,16);
  zoom_level_+=dl;
  zoom_level_=std::min(zoom_level_,8);
  zoom_level_=std::max(zoom_level_,-8);
  Real old_zoom_scale = ldexp(1.0,old_zoom_level);
  Real old_i_zoom_scale = 1.0/old_zoom_scale;

  zoom_scale_ = ldexp(1.0,zoom_level_);
  i_zoom_scale_ = 1.0/zoom_scale_;

  offset_x_ += static_cast<Real>(size().width())/2.0*(old_i_zoom_scale-i_zoom_scale_);
  offset_y_ += static_cast<Real>(size().height())/2.0*(old_i_zoom_scale-i_zoom_scale_);

  update_rubberband_from_selection_();
  UpdateView(false);
  emit zoomed(zoom_level_);
}


void DataViewerPanelBase::move(int dx, int dy)
{
  offset_x_-=i_zoom_scale_*(Real)dx;
  offset_y_-=i_zoom_scale_*(Real)dy;
  update_rubberband_from_selection_();
  UpdateView(false);
}

void DataViewerPanelBase::MoveTo(const geom::Vec2& p)
{
  offset_x_=p[0];
  offset_y_=p[1];

  update_rubberband_from_selection_();
  UpdateView(false);
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
  return !rubberband_->geometry().isNull() && rubberband_->isVisible();
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
  if(!HasOverlap(visible,GetObservedData().GetExtent())){
    return;
  }
  Extent overlap=Overlap(visible,GetObservedData().GetExtent());
  Point a = overlap.GetStart();
  Point b = overlap.GetEnd();

  QFont fnt("Courier",(zoom_level_-6)*2+7);
  painter.setFont(fnt);
  if(GetObservedData().GetType()==REAL){
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
  }else{
    for(int i = 0; i <= (b[0] - a[0]); ++i) {
      for(int j = 0; j <= (b[1] - a[1]); ++j) {
        Complex pixel_value = GetObservedData().GetComplex(Point(a[0]+i,
                                                                 a[1]+j,slab_));
        QString value_string = QString("%1+i%2").arg(pixel_value.real(),0,'g',5).arg(pixel_value.imag(),0,'g',5);
        Real rv = GetNormalizer()->Convert(abs(pixel_value));
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
}

void DataViewerPanelBase::update_rubberband_from_selection_()
{
  rubberband_->setGeometry(QRect(PointToWin(selection_.GetStart()),PointToWin(selection_.GetEnd()+Point(1,1))));
}

}}}  //ns
