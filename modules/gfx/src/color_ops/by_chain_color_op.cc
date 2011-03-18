//------------------------------------------------------------------------------
// This file is part of the OpenStructure project <www.openstructure.org>
//
// Copyright (C) 2008-2011 by the OpenStructure authors
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

ByChainColorOp::ByChainColorOp() : ColorOp(),chain_count_(0),color_grad_("RAINBOW")
{
  this->Init();
}

ByChainColorOp::ByChainColorOp(const String& selection, int mask) : ColorOp(selection,mask),chain_count_(0),color_grad_("RAINBOW")
{
  this->Init();
}

ByChainColorOp::ByChainColorOp(const mol::QueryViewWrapper& query_view, int mask) : ColorOp(query_view,mask),chain_count_(0),color_grad_("RAINBOW")
{
  this->Init();
}

void ByChainColorOp::Init()
{
  this->SetName("By chain");
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

int ByChainColorOp::GetChainCount() const
{
  return chain_count_;
}

void ByChainColorOp::SetChainCount(int chain_count)
{
  chain_count_ = chain_count;
}

gfx::ByChainColorOp ByChainColorOp::FromInfo(info::InfoGroup& group){
  gfx::ColorOp op = ColorOp::FromInfo(group);
  mol::QueryViewWrapper wrapper(op.GetSelection(),op.GetSelectionFlags());
  int mask = op.GetMask();
  return gfx::ByChainColorOp(wrapper, mask);
}

gfx::Color ByChainColorOp::GenerateColor(String& ident) const{
  unsigned int size=colors_.size()-1;
  if(size<=0){
    colors_[ident] = color_grad_.GetColorAt(0.0);
  }
  else{
    colors_[ident] = color_grad_.GetColorAt(float(size) / chain_count_);
  }
  return colors_[ident];
}

}}

