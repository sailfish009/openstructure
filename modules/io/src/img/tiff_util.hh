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
#ifndef OST_IMG_IO_TIFF_HH
#define OST_IMG_IO_TIFF_HH

#include <ost/stdint.hh>
#include <tiff.h>
#include <tiffio.h>
#include <boost/filesystem.hpp>
#include <ost/img/alg/normalizer_factory.hh>
#include <ost/io/img/image_format.hh>

#if defined(TIFFLIB_VERSION) && TIFFLIB_VERSION >= 20111221 
#  define INTNN_T int64_t
#  define UINTNN uint64
#  define UINTNN_T uint64_t
#  define COMPLEXINTNN_T complexint64_t
#else
#  define INTNN_T int32_t
#  define UINTNN uint32
#  define UINTNN_T uint32_t
#  define COMPLEXINTNN_T complexint32_t
#endif

namespace ost { namespace io { namespace detail {
/// \internal
typedef void (*TIFFWarningHandler)(const char*, const char*, va_list);

/// \internal
void tiff_warning_handler(const char *mod, const char* fmt, va_list ap);

/// \internal
struct tiff_warning_handler_wrapper {
  tiff_warning_handler_wrapper() {
    handler_ = TIFFSetWarningHandler(tiff_warning_handler);
  }

  ~tiff_warning_handler_wrapper() {
    TIFFSetWarningHandler(handler_);
  }

  TIFFWarningHandler handler_;
};

/// \internal
class COMPLEXINTNN_T:public std::complex<INTNN_T>{
public:

  operator std::complex<Real>()
  {
    return std::complex<Real>(real(),imag());
  }
};

/// \internal
class complexint16:public std::complex<int16>{
public:

  operator std::complex<Real>()
  {
    return std::complex<Real>(real(),imag());
  }
};

/// \internal
class complexint8:public std::complex<int8>{
public:

  operator std::complex<Real>()
  {
    return std::complex<Real>(real(),imag());
  }
};

/// \internal
INTNN_T CustomTIFFReadProcIStream(void* thandle, void* tdata, INTNN_T tsize);
/// \internal
INTNN_T CustomTIFFReadProcIStream(void* thandle, void* tdata, INTNN_T tsize);
/// \internal
INTNN_T CustomTIFFReadProcOStream(void* thandle, void* tdata, INTNN_T tsize);
/// \internal
INTNN_T CustomTIFFWriteProcIStream(void* thandle, void* tdata, INTNN_T tsize);
/// \internal
INTNN_T CustomTIFFWriteProcOStream(void* thandle, void* tdata, INTNN_T tsize);
/// \internal
UINTNN_T CustomTIFFSeekProcIStream(void* thandle, UINTNN_T toff, int dir);
/// \internal
UINTNN_T CustomTIFFSeekProcOStream(void* thandle, UINTNN_T toff, int dir);
/// \internal
int CustomTIFFCloseProc(void* thandle);
/// \internal
UINTNN_T CustomTIFFSizeProcIStream(void* thandle);
/// \internal
UINTNN_T CustomTIFFSizeProcOStream(void* thandle);
/// \internal
int CustomTIFFMapFileProc(void* thandle, void** tdata, UINTNN* toff);
/// \internal
void CustomTIFFUnmapFileProc(void* thandle, void* tdata, UINTNN toff);

/// \internal
template<typename IN_TYPE,typename OUT_TYPE, class IST>
void do_tiff_read(tdata_t buf,uint16 rps, UINTNN_T width, IST* is,int& current_row, int spp)
{
  IN_TYPE* dp = static_cast<IN_TYPE*>(buf);
  for(uint r=0;r<rps;r++) {
    for(uint c=0;c<width;c++) {
      is->Value(img::Point(c,current_row))=static_cast<OUT_TYPE>(dp[(r*width+c)*spp]);
    }
    current_row++;
  }
}

/// \internal
template<typename IN_TYPE,typename OUT_TYPE, class IST>
void do_tiff_write(TIFF *tif, IST* is,UINTNN_T rowsperstrip,UINTNN_T width,UINTNN_T height, UINTNN_T strip,const  img::NormalizerPtr& nptr)
{
  uint datalength=rowsperstrip*width;
  if((strip+1)*rowsperstrip>height){
    datalength=(height-strip*rowsperstrip)*width;
  }
  OUT_TYPE* buf=new OUT_TYPE[datalength];
  img::Point start = is->GetExtent().GetStart();

  uint i=0;
  for(uint r=strip*rowsperstrip;r<(strip+1)*rowsperstrip && r<height;r++) {
    for(uint c=0;c<width;c++) {
      buf[i] = static_cast<OUT_TYPE>(nptr->Convert(is->Value(img::Point(c,r)+start)));
      ++i;
    }
  }
  TIFFWriteEncodedStrip(tif,strip,buf, sizeof(OUT_TYPE)*datalength);
  delete[] buf;
}

}}} // ns

#endif // TIFF_HH
