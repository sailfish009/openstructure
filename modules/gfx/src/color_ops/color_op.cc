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

#include <ost/gfx/color_ops/color_op.hh>

namespace ost { namespace gfx {

ColorOp::ColorOp(): selection_(), mask_(DETAIL_COLOR|MAIN_COLOR) 
{ }


ColorOp::ColorOp(const String& selection, int mask):
  selection_(selection), mask_(mask)
{ }


bool ColorOp::CanApplyTo(const GfxObjP& obj) const
{
  return false;
}

void ColorOp::ApplyTo(GfxObjP& obj) const
{
  //Do Nothing
}

void ColorOp::SetSelection(const String& selection)
{
  selection_ = selection;
}

String ColorOp::GetSelection() const
{
  return selection_;
}

void ColorOp::ToInfo(info::InfoGroup& group) const
{
  std::ostringstream ss;
  ss << (int)mask_ << "\t" << selection_;
  group.SetTextData(ss.str());
}

gfx::ColorOp ColorOp::FromInfo(ost::info::InfoGroup& group)
{
  ColorOp op;
  std::istringstream ss(group.GetTextData());
  int mask;
  ss >> mask;
  op.SetMask(mask);
  String selection;
  String part;
  while(ss.good()){
    ss >> part;
    selection.append(part);
    if(ss.good())selection.append(" ");
    part.clear();
  }
  op.SetSelection(selection);
  return op;
}

ColorMask ColorOp::GetMask() const
{
  return mask_;
}

void ColorOp::SetMask(ColorMask mask)
{
  mask_=mask;
}


}}

