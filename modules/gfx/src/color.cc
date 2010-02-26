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
  Author: Ansgar Philippsen, Marco Biasini
*/

#include <ostream>

#include "color.hh"

namespace ost { namespace gfx {

Color::Color(int code)
{
  rgba[0]=static_cast<float>((code>>16)&0xff)/255.0;
  rgba[1]=static_cast<float>((code>>8)&0xff)/255.0;
  rgba[2]=static_cast<float>(code&0xff)/255.0;
  rgba[3]=1.0;
}

std::ostream& operator<<(std::ostream& s, const Color& c)
{
  s << "{" << c.Red() << "," << c.Green() << "," << c.Blue() << "," << c.Alpha() << "}";
  return s;
}


Color& Color::operator*=(float rhs)
{
  rgba[0]*=rhs;
  rgba[1]*=rhs;
  rgba[2]*=rhs;
  rgba[3]*=rhs; 
  return *this;
}

Color& Color::operator+=(float rhs)
{
  rgba[0]+=rhs;
  rgba[1]+=rhs;
  rgba[2]+=rhs;
  rgba[3]+=rhs;
  return *this;
}
  
Color& Color::operator+=(const Color& rhs)
{
  rgba[0]+=rhs[0];
  rgba[1]+=rhs[1];
  rgba[2]+=rhs[2];
  rgba[3]+=rhs[3];  
  return *this;  
}

Color& Color::operator-=(const Color& rhs)
{
  rgba[0]-=rhs[0];
  rgba[1]-=rhs[1];
  rgba[2]-=rhs[2];
  rgba[3]-=rhs[3];  
  return *this;
}

Color& Color::operator-=(float rhs)
{
  rgba[0]-=rhs;
  rgba[1]-=rhs;
  rgba[2]-=rhs;
  rgba[3]-=rhs;
  return *this;
}

Color& Color::operator/=(float rhs)
{
  rgba[0]/=rhs;
  rgba[1]/=rhs;
  rgba[2]/=rhs;
  rgba[3]/=rhs;  
  return *this;
}

}} // ns
