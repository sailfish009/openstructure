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
  point

  Author: Ansgar Philippsen
*/

#include <ost/message.hh>
#include <iostream>
#include <sstream>

#include "point.hh"
#include "size.hh"
#include "vecmat.hh"

namespace ost { namespace img {

Point::Point(const Size& s) { data_[0]=s[0]; data_[1]=s[1]; data_[2]=s[2]; }

Point& Point::operator+=(const Size& s) 
{
  data_[0]+=s[0];   
  data_[1]+=s[1];
  data_[2]+=s[2];
  return *this;
}

Point& Point::operator-=(const Size& s) 
{
  data_[0]-=s[0];   
  data_[1]-=s[1];
  data_[2]-=s[2];
  return *this;
}

std::ostream& operator<<(std::ostream& os, const Point &p) 
{
  os << "(" << p[0] << "," << p[1] << "," << p[2] << ")";
  return os;
}


}} // namespace
