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

#include <ost/message.hh>

#include "data_viewer_panel.hh"
#include "pointlist_overlay.hh"

namespace ost { namespace img { namespace gui {

PointlistOverlay::PointlistOverlay(const String& name):
  PointlistOverlayBase(name)
{
  build();
}

PointlistOverlay::PointlistOverlay(const PointList& pl, const String& name):
  PointlistOverlayBase(name)
{
  build();
  Add(pl);
}

void PointlistOverlay::OnMenuEvent(QAction* e)
{
  if(e==a_clr_) {
    Clear();
  } else {
    PointlistOverlayBase::OnMenuEvent(e);
  }
}

void PointlistOverlay::Add(const Point& pnt)
{
  pointlist_.push_back(pnt);
}

void PointlistOverlay::Add(const PointList& pl)
{
  for(PointList::const_iterator it=pl.begin();it!=pl.end();++it) {
    Add(*it);
  }
}

void PointlistOverlay::Remove(const Point& pnt)
{
  for(PointList::iterator it=pointlist_.begin();it!=pointlist_.end();++it){
    if (*it==pnt) {
      pointlist_.erase(it);
    }
  }
}

void PointlistOverlay::Clear()
{
  pointlist_.clear();
}

void PointlistOverlay::OnDraw(QPainter& pnt, DataViewerPanel* dvp, bool is_active)
{
  std::vector<QPoint> qpointlist;
  for(PointList::iterator it=pointlist_.begin();it!=pointlist_.end();++it){
    qpointlist.push_back(dvp->FracPointToWinCenter((*it).ToVec2()));
  }
  
  DrawPointList(pnt,dvp, is_active? active_color_ : passive_color_,
                qpointlist);
}

bool PointlistOverlay::OnMouseEvent(QMouseEvent* e, 
                                    DataViewerPanel* dvp,
                                    const QPoint& lastmouse)
{
  if(e->buttons()==Qt::LeftButton && e->modifiers()&Qt::ShiftModifier) {
    // toggle point at mouse position
    Point mpoint = dvp->WinToPoint(e->x(),e->y());
    PointList::iterator pos=find(pointlist_.begin(),
                                       pointlist_.end(),mpoint);
    if(pos!=pointlist_.end()) {
      pointlist_.erase(pos);
    } else {
      pointlist_.push_back(mpoint);
    }
    return true;
  }
  return false;
}

void PointlistOverlay::build()
{
  menu_->addSeparator();
  a_clr_ = menu_->addAction("Clear");
}

}}}  //ns
