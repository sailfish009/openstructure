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
#include "gradient.hh"
#include <iostream>
#include <vector>

#include <ost/info/info.hh>
#include <ost/info/info_fw.hh>

namespace ost { namespace gfx {


Gradient::Gradient() 
{
}

void Gradient::SetColorAt(float t, const Color& c)
{
  std::vector<Stop>::iterator i;
  for (i=stops_.begin(); i!=stops_.end(); ++i) {
    if (i->t>t) {
      break;
    }
  }
  stops_.insert(i, Stop(t, c));
}

Color Gradient::GetColorAt(float t) const
{
  if (stops_.empty())
    return Color(0.0, 0.0, 0.0, 1.0);
  
  uint c=0;
  while (t>=stops_[c].t && c<stops_.size()) {
    ++c;
  }
  if (c==0) {
    return stops_.front().color;
  }
  if (c==stops_.size()) {
    return stops_.back().color;
  }
  float d=stops_[c].t-stops_[c-1].t;
  float tt=0.0;
  if (d>0.0001) {
    tt=(t-stops_[c-1].t)/d;
  } 
  return stops_[c-1].color*(1.0-tt)+stops_[c].color*tt;
}

void Gradient::GradientToInfo(info::InfoGroup& group) const{
  std::ostringstream ss;

  ss << stops_.size() << "\n";

  for( std::vector<Stop>::size_type i = 0; i < stops_.size(); i++ ) {
    ss << stops_[i].t << "\t" << stops_[i].color.Red() << "\t" << stops_[i].color.Green() << "\t" << stops_[i].color.Blue() << "\t" << stops_[i].color.Alpha() << "\n";
  }
  group.SetTextData(ss.str());
}

StopList Gradient::GetStops() const{
	return stops_;
}

gfx::Gradient Gradient::GradientFromInfo(const info::InfoGroup& group){
  std::istringstream ss(group.GetTextData());
  Gradient grad;

  StopList::size_type size = 0;
  ss >> size;
  float t, r, g, b, a;
  for( StopList::size_type i = 0; i < size; i++ ) {
    ss >> t >> r >> g >> b >> a;
    Color c = Color(r,g,b,a);
    grad.SetColorAt(t, c);
  }

  return grad;
}
}}
