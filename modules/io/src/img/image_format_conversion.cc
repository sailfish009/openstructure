//------------------------------------------------------------------------------
// This file is part of the OpenStructure project <www.openstructure.org>
//
// Copyright (C) 2008-2015 by the OpenStructure authors
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
  wrapper around Lawrence Baker's vax conversion routines

  Author: Ansgar Philippsen
*/

#ifndef IMG_IMAGE_FORMAT_H
#error "image_format_conversion.cc must be included from image_format.hh"
#endif

#include <ost/message.hh>
#include <ost/io/io_exception.hh>

extern "C" {
#include "convert_vax_data.h"
}

namespace ost { namespace io {

template<typename T>
inline
void VaxToIEEE(T* t,unsigned int n);

template<>
inline
void VaxToIEEE(char* t,unsigned int n)
{
  // noop
}

template<>
inline
void VaxToIEEE(unsigned char* t,unsigned int n)
{
  // noop
}

template<>
inline
void VaxToIEEE(float* t,unsigned int n)
{
  static int count[]={0};
  count[0]=n;
  from_vax_r4(t,t,count);
}

template<>
inline
void VaxToIEEE(int* t,unsigned int n)
{
  static int count[]={0};
  count[0]=n;
  from_vax_i4(t,t,count);
}

template<>
inline
void VaxToIEEE(unsigned int* t,unsigned int n)
{
  static int count[]={0};
  count[0]=n;
  from_vax_i4(t,t,count);
}

template<>
inline
void VaxToIEEE(short* t,unsigned int n)
{
  static int count[]={0};
  count[0]=n;
  from_vax_i2(t,t,count);
}

template<>
inline
void VaxToIEEE(unsigned short* t,unsigned int n)
{
  static int count[]={0};
  count[0]=n;
  from_vax_i2(t,t,count);
}

template<typename T>
inline
void VaxToIEEE(T* t,unsigned int n)
{
  throw IOException("unsupported VaxToIEEE conversion");
}

}} // ns
