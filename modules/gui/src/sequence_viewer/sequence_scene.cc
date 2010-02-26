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
#include "sequence_scene.hh"
#include <boost/bind.hpp>
#include <QPainter>

namespace ost { namespace gui {

using boost::bind;

void SequenceScene::drawBackground(QPainter* painter, const QRectF& rect)
{
  if (sequences_.empty()) {
    return;
  }
  QBrush stripe_brushes[]={
    QBrush(QColor(Qt::white)),
    QBrush(QColor(245, 245, 245))
  };
  float width=sequences_[0]->GetCharWidth()*5;
  QRectF stripe(QPointF(((int(rect.left()/width)-1)-1)*width+header_width_,
                        rect.top()), 
                QSizeF(width, rect.height()));
  int n=round(rect.width()/width)+2;
  int start=std::max(0, int(floor(rect.left()/width)))-1;  
  for (int i=0; i<n; ++i, stripe.translate(width, 0.0)) {
    painter->fillRect(stripe, stripe_brushes[(i+start+2) % 2]);
  }
  painter->setPen(QPen(Qt::gray));
  const static int STEP_SIZE=10;
  float s=sequences_[0]->GetCharWidth()*STEP_SIZE;
  int n_rulers=round(rect.width()/s);
  int first=std::max(0, int(floor(rect.left()/s)));
  painter->fillRect(QRectF(QPointF(0.0, rect.top()), 
                           QSizeF(header_width_, rect.height())),
                    QBrush(Qt::white));  
  for (int i=first;i<first+n_rulers+2; ++i) {
    painter->drawText(QRectF(QPointF(i*s-50+header_width_, 
                                     rect.top()), QSizeF(100, 30)),
                      Qt::AlignCenter, QString::number(i*STEP_SIZE));
    painter->drawLine(QPointF(i*s+header_width_, rect.top()+30), 
                      QPointF(i*s+header_width_, rect.top()+30+rect.height()));
  }

}

void SequenceScene::SetContextMenu(QMenu* menu)
{
  context_menu_=menu;
}

QMenu* SequenceScene::GetContextMenu()
{
  return context_menu_;
}

SequenceScene::SequenceScene(QObject* parent):
  QGraphicsScene(parent), header_width_(120.0)
{
  connect(this, SIGNAL(sceneRectChanged(const QRectF&)), this, 
          SLOT(OnSceneRectChange(const QRectF&)));
  context_menu_=NULL;
}

void SequenceScene::RepackSequences()
{
  if (sequences_.empty()) {
    return;
  }
  sequences_.front()->setPos(0.0, 50.0);
  for (std::vector<SequenceItem*>::iterator i=sequences_.begin()+1, 
       e=sequences_.end(); i!=e; ++i) {
     (*i)->setPos(0.0, (*(i-1))->pos().y()+
                 (*(i-1))->boundingRect().height());        
  } 
}

void SequenceScene::AddSequence(SequenceItem* seq)
{
  this->addItem(seq);
  if (!sequences_.empty()) {
    seq->setPos(0.0, sequences_.back()->pos().y()+
                sequences_.back()->boundingRect().height());
  } else {
    seq->setPos(0.0, 50.0);
  }
  sequences_.push_back(seq);
  update();
}

void SequenceScene::RemoveSequence(SequenceItem* seq)
{
  this->removeItem(seq);
  std::vector<SequenceItem*>::iterator f=std::find(sequences_.begin(),
    sequences_.end(), seq);
  if(f != sequences_.end())
  {
    sequences_.erase(f);
  }
  RepackSequences();
}

void SequenceScene::SetHeaderWidth(float width)
{
  header_width_=width;
  update();
}

float SequenceScene:: GetHeaderWidth() const
{
  return header_width_;
}

void SequenceScene::OnSceneRectChange(const QRectF& rect)
{
}

}}
