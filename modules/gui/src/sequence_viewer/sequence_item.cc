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
  Author: Marco Biasini
 */
 
#include <ost/mol/mol.hh>
#include <boost/bind.hpp>

#include "sequence_item.hh"
#include "sequence_scene.hh"

#include <QPainter>
#include <QGraphicsSceneMouseEvent>
#include <QGraphicsSceneContextMenuEvent>
#include <QGraphicsItem>
#include <QDebug>

namespace ost { namespace gui {

using boost::bind;

Knob::Knob(const QString& text, const QColor& color, 
           SequenceItem* parent):
  QGraphicsItem(parent)
{
  
}
  
void Knob::paint(QPainter* painter, const QStyleOptionGraphicsItem*,
                 QWidget* widget)
{
  painter->setBrush(QBrush(color_));
  painter->setPen(QPen(color_.darker()));
  painter->drawEllipse(this->boundingRect());
  if (this->isUnderMouse()) {
    painter->setPen(QPen(color_.darker()));
  } else {
    painter->setPen(QPen(Qt::white));
  }
  painter->drawText(this->boundingRect(), Qt::AlignCenter, text_);  
}

QRectF Knob::boundingRect() const
{
  return QRectF(0.0, 0.0, 15.0, 15.0);
}


SequenceHeader::SequenceHeader(SequenceItem* parent):
  QGraphicsItem(parent)
{
  font_=QFont("Courier", 12);
  font_.setKerning(false);
  font_.setFixedPitch(true);
}

  
void SequenceHeader::paint(QPainter* painter, const QStyleOptionGraphicsItem*,
                           QWidget* widget)
{
  SequenceItem* si=dynamic_cast<SequenceItem*>(this->parentItem());
  painter->setFont(font_);
  QFontMetrics m(font_);
  QString name(si->GetSequence().GetName().c_str());
  QRectF br=this->boundingRect();
  name=m.elidedText(name, Qt::ElideMiddle, br.width());
  painter->drawText(br, Qt::AlignTop, name);
}

QRectF SequenceHeader::boundingRect() const
{
  QFontMetrics metrics(font_);
  SequenceItem* si=dynamic_cast<SequenceItem*>(this->parentItem());
  QString name(si->GetSequence().GetName().c_str());
  QRectF rect(0, -2,name.size()*si->GetCharWidth()*1.1, 
              si->GetCharHeight()+2);
  SequenceScene* s=dynamic_cast<SequenceScene*>(this->scene());
  rect.setWidth(std::min(qreal(s->GetHeaderWidth()), rect.width()));
  return rect;
}

SequenceBody::SequenceBody(SequenceItem* parent):
  QGraphicsItem(parent)
{
  
}

  
void SequenceBody::paint(QPainter* painter, const QStyleOptionGraphicsItem*,
                         QWidget* widget)
{  
  SequenceItem* si=dynamic_cast<SequenceItem*>(this->parentItem());
  seq::SequenceHandle seq=si->GetSequence();
  if (!seq.IsValid()) {
    return;
  }
  QString qstr(seq.GetString().c_str());
  QColor sel_color(Qt::yellow);  
  if (si->IsSecStructureVisible()) {
    sel_color.setAlpha(50);
    painter->setBrush(QBrush(sel_color));
    painter->setPen(QPen(QColor(Qt::yellow).darker()));  
    painter->drawPath(si->GetSecStructPaths());    
  }
  painter->setFont(si->GetFont());
  painter->setPen(QPen(Qt::black));  
  QString tt("X");
  QRectF br=this->boundingRect();
  for (int i=0; i<qstr.size(); ++i) {
    tt[0]=qstr[i];
    QRectF rect(br.left()+i*si->GetCharWidth(), br.top(), 
                br.width(), br.height());    
    painter->drawText(rect, Qt::AlignLeft|Qt::AlignVCenter, tt);    
  }

  sel_color=QColor(Qt::green).lighter();
  painter->setPen(QPen(Qt::green));  
  sel_color.setAlpha(100);
  painter->setBrush(QBrush(sel_color));
  QRectF text_bounds=painter->boundingRect(boundingRect(), 
                                           Qt::AlignLeft|Qt::AlignVCenter,
                                           qstr);
  const SequenceItem::Selection& sel=si->GetSelection();
  for (size_t i=0; i<sel.size(); ++i) {
    painter->drawRect(QRectF(sel[i].Loc*si->GetCharWidth(), 0, 
                             sel[i].Length*si->GetCharWidth(),
                             si->GetCharHeight()));
  }
}

QRectF SequenceBody::boundingRect() const
{
  SequenceItem* si=dynamic_cast<SequenceItem*>(this->parentItem());
  seq::SequenceHandle seq=si->GetSequence();
  if (!seq.IsValid()) {
    return QRectF();
  }
  return QRectF(0, -2, seq.GetLength()*si->GetCharWidth(), 
                si->GetCharHeight()+2);  
}


SequenceItem::SequenceItem(const seq::SequenceHandle& seq, 
                           QGraphicsItem* parent):
  QGraphicsItem(parent), seq_(seq), show_sec_struct_(true)
{
  header_=new SequenceHeader(this);  
  body_=new SequenceBody(this);
  body_->translate(120.0, 0.0);
  font_=QFont("Courier", 12);
  font_.setKerning(false);
  font_.setFixedPitch(true);
  QFontMetrics metrics(font_);
  advance_=metrics.boundingRect('W').width();
  height_=metrics.boundingRect('|').height();
  ascent_=metrics.ascent();
  this->setAcceptedMouseButtons(Qt::LeftButton);
  this->ExtractSecStructSegments();
  selection_change_=false;
}

SequenceHeader* SequenceItem::GetHeader()
{
  return header_;
}

SequenceBody* SequenceItem::GetBody()
{
  return body_;
}

const seq::SequenceHandle& SequenceItem::GetSequence() const
{
  return seq_;
}

QRectF SequenceItem::GetCharBounds(int pos) const
{
  QPointF a(pos*advance_, 0);
  return QRectF(this->mapToScene(a), QSizeF(advance_, height_));
}

float SequenceItem::GetCharWidth() const
{
  return advance_;
}

float SequenceItem::GetCharHeight() const
{
  return height_;
}

void SequenceItem::paint(QPainter* painter, const QStyleOptionGraphicsItem*,
                         QWidget* widget)
{
}

QRectF SequenceItem::boundingRect() const
{
  QRectF child_r(this->childrenBoundingRect());
  if (show_sec_struct_) {
    return QRectF(child_r.topLeft(), 
                  QSize(child_r.width(), child_r.height()*2.5));    
  } else {
    return QRectF(child_r.topLeft(), 
                  QSize(child_r.width(), child_r.height()));
  }
}

void SequenceItem::ExtractSecStructSegments()
{
  if (!seq_.HasAttachedView()) {
    return;
  }
  mol::ChainView chain=seq_.GetAttachedView().GetChainList()[0];
  sec_=mol::alg::ExtractSecStructureSegments(chain);
  this->SecStructSegmentsToPaths();
}

void SequenceHeader::contextMenuEvent(QGraphicsSceneContextMenuEvent* event)
{
  //...
}
void SequenceBody::mousePressEvent(QGraphicsSceneMouseEvent* event)
{
  SequenceItem* si=dynamic_cast<SequenceItem*>(this->parentItem());  
  event->accept();
  last_index_=-1;
  good_ole_click_=true;
  merge_select_=event->modifiers() & Qt::ShiftModifier;
  si->BeginSelectionChange();
}

void SequenceBody::mouseDoubleClickEvent(QGraphicsSceneMouseEvent* event)
{
  SequenceItem* si=dynamic_cast<SequenceItem*>(this->parentItem());    
  int index=std::max(0, int(event->pos().x()/si->GetCharWidth()));  
  // find the secondary structure element -- if any that we are in
  mol::alg::SecStructureSegments::iterator i=si->GetSecStructSegments().begin(),
                                           e=si->GetSecStructSegments().end();  
  for (;i!=e; ++i) {
    if (i->first<=index && i->last>=index) {
      si->Select(i->first, i->last+1, !merge_select_);
      break;
    }
  }
  good_ole_click_=false;
}
void SequenceHeader::mousePressEvent(QGraphicsSceneMouseEvent* event)
{
  event->accept();
}

void SequenceHeader::mouseReleaseEvent(QGraphicsSceneMouseEvent* event)
{
  SequenceItem* si=dynamic_cast<SequenceItem*>(this->parentItem());
  if (si->IsEverythingSelected()) {
    si->ClearSelection();
  } else {
    si->SelectAll();
  }

}

void SequenceBody::mouseReleaseEvent(QGraphicsSceneMouseEvent* event)
{
  SequenceItem* si=dynamic_cast<SequenceItem*>(this->parentItem());    
  if (good_ole_click_) {
    int index=std::max(0, int(event->pos().x()/si->GetCharWidth()));
    si->Select(index, index+1, !merge_select_);    
  }
  si->EndSelectionChange();
}

void SequenceBody::mouseMoveEvent(QGraphicsSceneMouseEvent* event)
{
  SequenceItem* si=dynamic_cast<SequenceItem*>(this->parentItem());    
  good_ole_click_=false;  
  if (!si->GetSequence().IsValid()) {
    return;
  }
  float x=event->pos().x();
  int index=std::max(0, int(x/si->GetCharWidth()));
  if (last_index_!=-1 && last_index_!=index) {
    si->Select(last_index_, index, !merge_select_);
  } else {
    last_index_=index;
  }
}

void SequenceItem::StackToPath(std::vector<QPointF>& stack)
{
  if (!stack.empty()) {
    sec_paths_.moveTo(QPointF(stack.front().x(), stack.front().y()+.5*ascent_));
    for (std::vector<QPointF>::iterator j=stack.begin()+1, 
         e2=stack.end(); j!=e2; ++j) {
      sec_paths_.lineTo(QPointF(j->x(), j->y()+0.5*ascent_));
    }
    for (std::vector<QPointF>::reverse_iterator j=stack.rbegin(), 
         e2=stack.rend(); j!=e2; ++j) {
      sec_paths_.lineTo(QPointF(j->x(), -j->y()+.5*ascent_));
    }        
    sec_paths_.closeSubpath();
    stack.clear();
  }  
}

void SequenceItem::SecStructSegmentsToPaths()
{
  sec_paths_=QPainterPath();
  
  sec_paths_.setFillRule(Qt::WindingFill);
  std::vector<QPointF> stack;
  int last_end=-2;
  for (mol::alg::SecStructureSegments::iterator i=sec_.begin(), 
       e=sec_.end(); i!=e; ++i) {
    mol::alg::SecStructureSegment s=*i;
    if (last_end!=s.first-1) {
      this->StackToPath(stack);
    }
    if (s.ss_type.IsCoil()) {
      stack.push_back(QPointF(s.first*advance_, -.6*height_));
      stack.push_back(QPointF((s.last+1)*advance_,  -.6*height_));
    } else if (s.ss_type.IsHelical()) {
      stack.push_back(QPointF(s.first*advance_, -.6*height_));
      stack.push_back(QPointF(s.first*advance_, -.9*height_));
      stack.push_back(QPointF((s.last+1)*advance_, -.9*height_));
      stack.push_back(QPointF((s.last+1)*advance_, -.6*height_));
    } else if (s.ss_type.IsExtended()) {
      stack.push_back(QPointF(s.first*advance_, -.6*height_));
      stack.push_back(QPointF(s.first*advance_, -.9*height_));
      stack.push_back(QPointF((s.last-0.5)*advance_, -.9*height_));
      stack.push_back(QPointF((s.last-0.5)*advance_, -1.3*height_));
      stack.push_back(QPointF((s.last+1)*advance_, -.6*height_));
      
    }
    last_end=s.last;    
  }
  this->StackToPath(stack);  
}

void SequenceItem::Select(int i1, int i2, bool clear)
{
  if (selection_change_>0) {
    selection_=ref_sel_;    
  }
  int a1=std::min(i1, i2);
  int len=std::abs(i1-i2);
  Range sel_start(a1, len);
  Range* sel=&sel_start;
  bool modified=false;
  if (clear) {
    selection_.clear();
    selection_.push_back(*sel);    
  } else {
    for (size_t i=0; i<selection_.size(); ++i) {
      if (sel->Loc>=selection_[i].Loc && sel->Loc<selection_[i].End()) {
        int old_end=selection_[i].End();        
        if (sel->End()<=selection_[i].End()) {
          selection_[i].Length=int(sel->Loc-selection_[i].Loc);
          if (old_end-sel->End()>0) {
            selection_.push_back(Range(sel->End(), old_end-sel->End()));
          }          
        } else {
          selection_[i].Length=int(sel->Loc-selection_[i].Loc);
          if (sel->End()-old_end>0) {
            selection_.push_back(Range(old_end, sel->End()-old_end));
          }
        }
        modified=true;
        break;
      } else if (sel->End()>selection_[i].Loc && 
                 sel->End()<=selection_[i].End()) {
        int nn=selection_[i].Loc;
        selection_[i].Length-=sel->End()-selection_[i].Loc;
        selection_[i].Loc=sel->End();
        modified=true;
        selection_.push_back(Range(sel->Loc, nn-sel->Loc));
        break;
      } else if (sel->Loc<selection_[i].Loc && 
                 sel->End()>selection_[i].End()) {
        selection_[i].Length=0;
      } else if (sel->Loc==selection_[i].End()) {
        selection_[i].Length+=sel->Length;
        sel->Length=0;       
        sel=&selection_[i];
        modified=true;
      } else if (sel->End()==selection_[i].Loc) {    
        selection_[i].Loc-=sel->Length;  
        selection_[i].Length+=sel->Length;
        sel->Length=0;
        sel=&selection_[i];        
        modified=true;
      }
    }
    if (!modified) {
      selection_.push_back(Range(a1, len));      
    }
  }
  Selection::iterator i=std::remove_if(selection_.begin(), selection_.end(),
                                       bind(&Range::Length, _1)==size_t(0));
  selection_.erase(i, selection_.end());
  if (!selection_change_) {
    emit this->SelectionChanged(this);
  } else {
    selection_change_=2;
  }
  update();
}

void SequenceItem::ClearSelection()
{
  selection_.clear();
  if (selection_change_>0) {
    selection_change_=2;
  } else {
    emit this->SelectionChanged(this);
    update();    
  }
}

void SequenceItem::SelectAll()
{
  if (!seq_.IsValid()) {
    return;
  }
  selection_.clear();
  selection_.push_back(Range(0, seq_.GetLength()));  
  if (selection_change_>0) {
    selection_change_=1;
  } else {
    emit this->SelectionChanged(this);  
    update();
  }
}

void SequenceItem::BeginSelectionChange()
{
  selection_change_=1;
  ref_sel_=selection_;
}

void SequenceItem::EndSelectionChange()
{
  if (selection_change_==2) {
    emit this->SelectionChanged(this);    
  }
  selection_change_=0;  
}

bool SequenceItem::IsEverythingSelected() const
{
  return selection_.size()==1 && selection_.front().Loc==0 && 
         selection_.front().End()>=static_cast<size_t>(seq_.GetLength());
}

const SequenceItem::Selection& SequenceItem::GetSelection() const
{
  return selection_;
}

mol::alg::SecStructureSegments& SequenceItem::GetSecStructSegments()
{
  return sec_;
}

void SequenceItem::SetShowSecStructure(bool show)
{
  show_sec_struct_=show;
}

bool SequenceItem::IsSecStructureVisible() const
{
  return show_sec_struct_;
}

}}
