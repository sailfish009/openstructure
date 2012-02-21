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

#include <stdarg.h>
#include <cassert>
#include <complex>
#include <ost/log.hh>

#include "tiff_util.hh"


namespace ost { namespace io { namespace detail {

typedef void (*TIFFWarningHandler)(const char*, const char*, va_list);

void tiff_warning_handler(const char *mod, const char* fmt, va_list ap)
{
  static char message[1024];
#ifdef _MSC_VER
  _vsnprintf(message,1024,fmt,ap);
#else
  vsnprintf(message,1024,fmt,ap);
#endif
  LOG_INFO(mod << ": " << message);
}

INTNN_T CustomTIFFReadProcIStream(void* thandle, void* tdata, INTNN_T tsize)
{
   std::istream* file= reinterpret_cast<std::istream*>(thandle);
   char* data= reinterpret_cast<char*>(tdata);
   file->read(data,tsize);
   return file->gcount();
}

INTNN_T CustomTIFFReadProcOStream(void* thandle, void* tdata, INTNN_T tsize)
{
  assert(false);
  return -1;
}

INTNN_T CustomTIFFWriteProcIStream(void* thandle, void* tdata, INTNN_T tsize)
{
  assert(false);
  return -1;
}

INTNN_T CustomTIFFWriteProcOStream(void* thandle, void* tdata, INTNN_T tsize)
{
   std::ostream* file= reinterpret_cast<std::ostream*>(thandle);
   char* data= reinterpret_cast<char*>(tdata);
   INTNN_T before = file->tellp();
   file->write(data,tsize);
   INTNN_T after = file->tellp();
   return after-before;
}

UINTNN_T CustomTIFFSeekProcIStream(void* thandle, UINTNN_T toff, int dir)
{
  std::istream* stream= reinterpret_cast<std::istream*>(thandle);

  std::ios_base::seekdir sdir = std::ios_base::beg;

  if (dir == 1)
  {
    sdir = std::ios_base::cur;
  } else if (dir == 2 ) {
    sdir = std::ios_base::end;
  }

  stream->seekg(toff,
                sdir);
  return stream->rdstate();
}

UINTNN_T CustomTIFFSeekProcOStream(void* thandle, UINTNN_T toff, int dir)
{
  std::ostream* stream= reinterpret_cast<std::ostream*>(thandle);

  std::ios_base::seekdir sdir = std::ios_base::beg;

  if (dir == 1)
  {
    sdir = std::ios_base::cur;
  } else if (dir == 2 ) {
    sdir = std::ios_base::end;
  }

  stream->seekp(toff,sdir);
  return stream->rdstate();
}

int CustomTIFFCloseProc(void* thandle)
{
  return 0;
}

UINTNN_T CustomTIFFSizeProcIStream(void* thandle)
{
   std::istream* stream= reinterpret_cast<std::istream*>(thandle);
   UINTNN_T curr_pos = stream->tellg();
   stream->seekg(0,std::ios::end);
   UINTNN_T size = stream->tellg();
   stream->seekg(curr_pos,std::ios::beg);
   return size;
}

UINTNN_T CustomTIFFSizeProcOStream(void* thandle)
{
   std::ostream* stream= reinterpret_cast<std::ostream*>(thandle);
   UINTNN_T curr_pos = stream->tellp();
   stream->seekp(0,std::ios::end);
   UINTNN_T size = stream->tellp();
   stream->seekp(curr_pos,std::ios::beg);
   return size;
}

int CustomTIFFMapFileProc(void* thandle, void** tdata, UINTNN* toff)
{
  assert(false);
  return(0);
}

void CustomTIFFUnmapFileProc(void* thandle, void* tdata, UINTNN toff)
{
  assert(false);
}

}}} //ns
