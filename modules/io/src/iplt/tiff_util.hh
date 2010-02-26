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
#ifndef OST_IPLT_IO_TIFF_HH
#define OST_IPLT_IO_TIFF_HH

#include <ost/stdint.hh>
#include <tiff.h>
#include <tiffio.h>
#include <boost/filesystem.hpp>
#include <ost/iplt/alg/normalizer_factory.hh>
#include <ost/io/iplt/image_format.hh>


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
class complexint32_t:public std::complex<int32_t>{
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
int32_t CustomTIFFReadProcIStream(void* thandle, void* tdata, int32_t tsize);
/// \internal
int32_t CustomTIFFReadProcIStream(void* thandle, void* tdata, int32_t tsize);
/// \internal
int32_t CustomTIFFReadProcOStream(void* thandle, void* tdata, int32_t tsize);
/// \internal
int32_t CustomTIFFWriteProcIStream(void* thandle, void* tdata, int32_t tsize);
/// \internal
int32_t CustomTIFFWriteProcOStream(void* thandle, void* tdata, int32_t tsize);
/// \internal
uint32_t CustomTIFFSeekProcIStream(void* thandle, uint32_t toff, int dir);
/// \internal
uint32_t CustomTIFFSeekProcOStream(void* thandle, uint32_t toff, int dir);
/// \internal
int CustomTIFFCloseProc(void* thandle);
/// \internal
uint32_t CustomTIFFSizeProcIStream(void* thandle);
/// \internal
uint32_t CustomTIFFSizeProcOStream(void* thandle);
/// \internal
int CustomTIFFMapFileProc(void* thandle, void** tdata, uint32* toff);
/// \internal
void CustomTIFFUnmapFileProc(void* thandle, void* tdata, uint32 toff);

/// \internal
template<typename IN_TYPE,typename OUT_TYPE, class IST>
void do_tiff_read(tdata_t buf,uint16 rps, uint32_t width, IST* is,int& current_row, int spp)
{
  IN_TYPE* dp = static_cast<IN_TYPE*>(buf);
  for(uint r=0;r<rps;r++) {
    for(uint c=0;c<width;c++) {
      is->Value(iplt::Point(c,current_row))=static_cast<OUT_TYPE>(dp[(r*width+c)*spp]);
    }
    current_row++;
  }
}

/// \internal
template<typename IN_TYPE,typename OUT_TYPE, class IST>
void do_tiff_write(TIFF *tif, IST* is,uint32_t rowsperstrip,uint32_t width,uint32_t height, uint32_t strip,const  iplt::NormalizerPtr& nptr)
{
  uint datalength=rowsperstrip*width;
  if((strip+1)*rowsperstrip>height){
    datalength=(height-strip*rowsperstrip)*width;
  }
  OUT_TYPE* buf=new OUT_TYPE[datalength];
  iplt::Point start = is->GetExtent().GetStart();

  uint i=0;
  for(uint r=strip*rowsperstrip;r<(strip+1)*rowsperstrip && r<height;r++) {
    for(uint c=0;c<width;c++) {
      buf[i] = static_cast<OUT_TYPE>(nptr->Convert(is->Value(iplt::Point(c,r)+start)));
      ++i;
    }
  }
  TIFFWriteEncodedStrip(tif,strip,buf, sizeof(OUT_TYPE)*datalength);
  delete[] buf;
}

}}} // ns

#endif // TIFF_HH
