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
  Author: Ansgar Philippsen
*/

#include "image_state_base.hh"
#include "binop.hh"

namespace ost { namespace iplt { namespace image_state {

ImageStateBase& ImageStateBase::operator+=(const ImageStateBase& b)
{
  static image_state::binop::add_ip f_add_ip;
  f_add_ip(this,&b);
  return *this;
}

ImageStateBase& ImageStateBase::operator-=(const ImageStateBase& b)
{
  static image_state::binop::sub_ip f_sub_ip;
  f_sub_ip(this,&b);
  return *this;
}

ImageStateBase& ImageStateBase::operator*=(const ImageStateBase& b)
{
  static image_state::binop::mul_ip f_mul_ip;
  f_mul_ip(this,&b);
  return *this;
}

ImageStateBase& ImageStateBase::operator/=(const ImageStateBase& b)
{
  static image_state::binop::div_ip f_div_ip;
  f_div_ip(this,&b);
  return *this;
}

}}}
