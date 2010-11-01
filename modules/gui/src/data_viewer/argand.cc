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


#include <QPainter>
#include <QPen>
#include <QGraphicsLineItem>
#include <ost/base.hh>

#include "argand.hh"



namespace ost { namespace img { namespace gui {

Argand::Argand(QGraphicsItem* p):
  QGraphicsWidget(p),
  buffer_(100,100)
{
  setMinimumSize(100,100);
  buffer_.fill(QColor(0,0,0,0));
}

Argand::~Argand()
{
}

void Argand::SetCurrentPixel(const Point& p)
{
 //current_=p;
 //update();
}

void Argand::SetExtent(const Extent& extent, const Data& data)
{
  buffer_.fill(QColor(0,0,0,0));
  QPainter painter(&buffer_);

  QPointF p0(size().width()/2,size().height()/2);
  painter.drawLine(p0.x(),0,p0.x(),size().height());
  painter.drawLine(0,p0.y(),size().width(),p0.y());

  if (data.GetType()!=COMPLEX) return;

  Real maxlen=0;
  std::vector<Complex> data_list;
  for(ExtentIterator it(extent); !it.AtEnd(); ++it) {
    Complex v = data.GetComplex(it);
    Real l = std::abs(v);
    maxlen = std::max(l,maxlen);
    data_list.push_back(v);
  }

  int dim=std::min(size().width(),size().height());
  Real sf=0.5*(Real)(dim-10)/maxlen;

  for (std::vector<Complex>::iterator it=data_list.begin(); it!=data_list.end();++it) {
    QPointF p((int)floor(it->real()*sf),(int)floor(it->imag()*sf));
    painter.setPen(QPen(QColor::fromHsvF(fmod(std::arg(*it)/(2*M_PI)+0.5,1.0),1,0.8)));
    painter.drawLine(p0,p0+p);
  }
  update();
}

void Argand::ClearExtent()
{
  buffer_.fill(QColor(0,0,0,0));
  update();
}

void Argand::paint(QPainter* painter,const QStyleOptionGraphicsItem * option,QWidget * widget)
{
  painter->drawPixmap(0,0,buffer_);
}  

 



}}}  //ns
