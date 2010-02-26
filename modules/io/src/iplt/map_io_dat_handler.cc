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
#include <boost/shared_array.hpp>
#include <boost/filesystem/operations.hpp>
#include <boost/filesystem/fstream.hpp>
#include <ost/iplt/image.hh>
#include <ost/iplt/alg/normalizer_factory.hh>
#include <ost/iplt/progress.hh>
#include <ost/io/io_exception.hh>
#include <ost/io/convert.hh>
#include <ost/io/converting_streams.hh>
#include <cassert>

#include "map_io_dat_handler.hh"

namespace ost { namespace io {

String DAT::FORMAT_STRING = "defined_dat";

DAT::DAT(bool normalize_on_save, Endianess end):
    ImageFormatBase(FORMAT_STRING)
{
    normalize_on_save_ = normalize_on_save;
    endianness_ = end;
}

Endianess DAT::GetEndianess() const
{
  return endianness_;
}

void DAT::SetEndianess(Endianess end)
{
  endianness_ = end;
}

bool DAT::GetNormalizeOnSave() const
{
  return normalize_on_save_;
}

void DAT::SetNormalizeOnSave(bool normalize_on_save)
{
  normalize_on_save_ = normalize_on_save;
}

bool MapIODatHandler::MatchContent(unsigned char* header)
{
  return false;
}

bool MapIODatHandler::MatchType(const ImageFormatBase& type)
{
  if(type.GetFormatString()==DAT::FORMAT_STRING) {
    return true;
  }
  return false;
}

bool MapIODatHandler::MatchSuffix(const String& suffix)
{
    if(suffix==".dat" || suffix==".img") {
      return true;
    }
    return false;
}

void MapIODatHandler::Import(iplt::MapHandle& sh, const boost::filesystem::path& loc,const ImageFormatBase& formatstruct )
{
  boost::filesystem::ifstream infile(loc, std::ios::binary);
  if(!infile) {
    throw IOException("could not open "+loc.string());
  }
  this->Import(sh,infile,formatstruct);
  infile.close();
}

template <int CONVERSIONTYPE>
void real_filler(iplt::MapHandle& sh, std::istream& file)
{
  BinaryIStream<CONVERSIONTYPE> file_bin(file);
  iplt::Size size = sh.GetSize();
  char this_dummy; //create dummy variable to give to iplt::Progress as this
  iplt::Progress::Instance().Register(&this_dummy,size[1],100);
  for(unsigned int row=0;row<size[1];row++) {
    for(unsigned int column=0;column<size[0];column++) {
      short value;
      file_bin >> value;
      sh.SetReal(iplt::Point(column,row),static_cast<Real>(value));
    }
    iplt::Progress::Instance().AdvanceProgress(&this_dummy);
  }
  iplt::Progress::Instance().DeRegister(&this_dummy);
}

template <int CONVERSIONTYPE>
void real_dumper(const iplt::MapHandle& sh, std::ostream& file, const DAT& formatdat)
{
  BinaryOStream<CONVERSIONTYPE> file_bin(file);
  iplt::alg::Normalizer norm = iplt::alg::CreateNoOpNormalizer();
  if (formatdat.GetNormalizeOnSave() == true) {
    norm = iplt::alg::CreateLinearRangeNormalizer(sh,formatdat.GetMinimum(),formatdat.GetMaximum());
  }
  iplt::Size size = sh.GetSize();
  char this_dummy; //create dummy variable to give to iplt::Progress as this
  iplt::Progress::Instance().Register(&this_dummy,size[1],100);
    for(unsigned int row=0;row<size[1];row++) {
      for(unsigned int column=0;column<size[0];column++)
      {
        file_bin << static_cast<short>(norm.Convert(sh.GetReal(iplt::Point(column,row))));
      }
      iplt::Progress::Instance().AdvanceProgress(&this_dummy);
    }
  iplt::Progress::Instance().DeRegister(&this_dummy);
}


void MapIODatHandler::Import(iplt::MapHandle& sh, std::istream& file, const ImageFormatBase& formatstruct)
{

  DAT form;
  DAT& formatdat = form;
  if (formatstruct.GetFormatString()==DAT::FORMAT_STRING) {
    formatdat = formatstruct.As<DAT>();
  } else {
    assert (formatstruct.GetFormatString()==UndefinedImageFormat::FORMAT_STRING);
  }

  uint curr_pos = file.tellg();
  file.seekg(0,std::ios::beg);
  uint start = file.tellg();
  file.seekg(0,std::ios::end);
  uint end = file.tellg();
  uint  filesize = end-start;
  file.seekg(curr_pos,std::ios::beg);

  unsigned int size=static_cast<unsigned int>(sqrt(filesize/2.0));
   // default origin is top-left
  sh.Reset(iplt::Extent(iplt::Point(0,0),iplt::Size(size,size)), iplt::REAL, iplt::SPATIAL);

  switch(formatdat.GetEndianess()) {
  case OST_BIG_ENDIAN:
    real_filler<OST_BIG_ENDIAN>(sh,file);
    break;
  case OST_LITTLE_ENDIAN:
    real_filler<OST_LITTLE_ENDIAN>(sh,file);
    break;
  case OST_VAX_DATA:
    real_filler<OST_VAX_DATA>(sh,file);
    break;
 }
}

void MapIODatHandler::Export(const iplt::MapHandle& mh2,
                                  const boost::filesystem::path& loc,const ImageFormatBase& formatstruct) const
{
  boost::filesystem::ofstream outfile(loc, std::ios::binary);
  if(!outfile)
  {
    throw IOException("could not open "+loc.string());
  }
  this->Export(mh2,outfile,formatstruct);
  outfile.close();
}

void MapIODatHandler::Export(const iplt::MapHandle& sh, std::ostream& file,const ImageFormatBase& formatstruct) const
{

  DAT form;
  DAT& formatdat = form;
  if (formatstruct.GetFormatString()==DAT::FORMAT_STRING) {
    formatdat = formatstruct.As<DAT>();
  } else {
    assert (formatstruct.GetFormatString()==UndefinedImageFormat::FORMAT_STRING);
  }

  iplt::Size size = sh.GetSize();

  if (size[2]!=1 || (size[0]!=size[1])) {
    throw IOException("DAT IO: Dat format only supports square 2D images");
  }

  switch(formatdat.GetEndianess()) {
  case OST_BIG_ENDIAN:
    real_dumper<OST_BIG_ENDIAN>(sh,file,formatdat);
    break;
  case OST_LITTLE_ENDIAN:
    real_dumper<OST_LITTLE_ENDIAN>(sh,file,formatdat);
    break;
  case OST_VAX_DATA:
    real_dumper<OST_VAX_DATA>(sh,file,formatdat);
    break;
 }
}

}} // namespaces


