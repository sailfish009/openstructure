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
  _snprintf(message,1024,fmt,ap);
#else
  snprintf(message,1024,fmt,ap);
#endif
  LOG_INFO(mod << ": " << message);
}

int32_t CustomTIFFReadProcIStream(void* thandle, void* tdata, int32_t tsize)
{
   std::istream* file= reinterpret_cast<std::istream*>(thandle);
   char* data= reinterpret_cast<char*>(tdata);
   file->read(data,tsize);
   return file->gcount();
}

int32_t CustomTIFFReadProcOStream(void* thandle, void* tdata, int32_t tsize)
{
  assert(false);
  return -1;
}

int32_t CustomTIFFWriteProcIStream(void* thandle, void* tdata, int32_t tsize)
{
  assert(false);
  return -1;
}

int32_t CustomTIFFWriteProcOStream(void* thandle, void* tdata, int32_t tsize)
{
   std::ostream* file= reinterpret_cast<std::ostream*>(thandle);
   char* data= reinterpret_cast<char*>(tdata);
   int32_t before = file->tellp();
   file->write(data,tsize);
   int32_t after = file->tellp();
   return after-before;
}

uint32_t CustomTIFFSeekProcIStream(void* thandle, uint32_t toff, int dir)
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

uint32_t CustomTIFFSeekProcOStream(void* thandle, uint32_t toff, int dir)
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

uint32_t CustomTIFFSizeProcIStream(void* thandle)
{
   std::istream* stream= reinterpret_cast<std::istream*>(thandle);
   uint32_t curr_pos = stream->tellg();
   stream->seekg(0,std::ios::end);
   uint32_t size = stream->tellg();
   stream->seekg(curr_pos,std::ios::beg);
   return size;
}

uint32_t CustomTIFFSizeProcOStream(void* thandle)
{
   std::ostream* stream= reinterpret_cast<std::ostream*>(thandle);
   uint32_t curr_pos = stream->tellp();
   stream->seekp(0,std::ios::end);
   uint32_t size = stream->tellp();
   stream->seekp(curr_pos,std::ios::beg);
   return size;
}

int CustomTIFFMapFileProc(void* thandle, void** tdata, uint32* toff)
{
  assert(false);
  return(0);
}

void CustomTIFFUnmapFileProc(void* thandle, void* tdata, uint32 toff)
{
  assert(false);
}

}}} //ns
