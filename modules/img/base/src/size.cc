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
  size

  Author: Ansgar Philippsen
*/

#include <ost/message.hh>
#include <iostream>
#include <sstream>

#include "size.hh"
#include "point.hh"

namespace ost { namespace img {

Size::Size(const Point& p1, const Point& p2):
  w_(std::abs(p2[0]-p1[0])+1<1 ? 1 : std::abs(p2[0]-p1[0])+1),
  h_(std::abs(p2[1]-p1[1])+1<1 ? 1 : std::abs(p2[1]-p1[1])+1),
  d_(std::abs(p2[2]-p1[2])+1<1 ? 1 : std::abs(p2[2]-p1[2])+1)
{}

Point Size::GetHalf() const 
{
  return Point((w_&0x1) ? (w_-1)/2 : w_/2-1,
	       (h_&0x1) ? (h_-1)/2 : h_/2-1,
	       (d_&0x1) ? (d_-1)/2 : d_/2-1);
}

std::ostream& operator<<(std::ostream& os, const img::Size &s) 
{
  os << "(" << s.GetWidth() << "," << s.GetHeight() << "," << s.GetDepth() << ")";
  return os;
}


}} // namespace
