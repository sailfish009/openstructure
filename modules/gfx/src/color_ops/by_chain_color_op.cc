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
#include <math.h>

#include "by_chain_color_op.hh"

#include <ost/gfx/entity.hh>
#include <ost/dyn_cast.hh>


namespace ost { namespace gfx {

ByChainColorOp::ByChainColorOp() : ColorOp(){this->init();}

ByChainColorOp::ByChainColorOp(const String& selection, int mask) : ColorOp(selection,mask){this->init();}

ByChainColorOp::ByChainColorOp(const mol::QueryViewWrapper& query_view, int mask) : ColorOp(query_view,mask){this->init();}

void ByChainColorOp::init(){
  color_grad_.SetColorAt(0,Color(1,1,0));
  color_grad_.SetColorAt(0.16666,Color(1,0,0));
  color_grad_.SetColorAt(0.33333,Color(0,1,1));
  color_grad_.SetColorAt(0.5,Color(0,1,0));
  color_grad_.SetColorAt(0.66666,Color(1,0,1));
  color_grad_.SetColorAt(0.83333,Color(0,0,1));
}

bool ByChainColorOp::CanApplyTo(const GfxObjP& obj) const{
  if(dynamic_cast<Entity*>(obj.get()))
    return true;
  return false;
}

void ByChainColorOp::ApplyTo(GfxObjP& objP) const{
  GfxObj* obj = objP.get();
  if(Entity* ent=dynamic_cast<Entity*>(obj)){
    ent->Apply(*this, false);
  }
}

gfx::Color ByChainColorOp::GetColor(String ident) const{
  if(colors_.find(ident) == colors_.end())
  {
    colors_[ident] = GenerateColor(ident);
  }
  return colors_[ident];
}

gfx::ByChainColorOp ByChainColorOp::FromInfo(info::InfoGroup& group){
  gfx::ColorOp op = ColorOp::FromInfo(group);
  String selection = op.GetSelection();
  int mask = op.GetMask();
  return gfx::ByChainColorOp(selection, mask);
}

gfx::Color ByChainColorOp::GenerateColor(String& ident) const{
  unsigned int size=colors_.size()-1;
  int cnt = size % 6;
  int factor = abs(size/6.0)+1;
  float start = 1.0/(6.0*factor);
  if(factor > 1 && !((factor+1) & 1)){
    float old_start = 1.0 / (6.0 *abs(size/6.0));
    start += old_start;
  }
  float value = start + (cnt / 6.0);
  if(value >= 1){
    value -=1;
  }
  return color_grad_.GetColorAt(value);
}

}}

