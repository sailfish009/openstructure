//------------------------------------------------------------------------------
// This file is part of the OpenStructure project <www.openstructure.org>
//
// Copyright (C) 2008-2011 by the OpenStructure authors
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
#include <map>
#include <ost/io/load_map.hh>
#include <ost/img/image_factory.hh>
#include <ost/img/alg/randomize.hh>
#define BOOST_TEST_DYN_LINK
#include <boost/test/unit_test.hpp>
#include  <ost/io/img/map_io_df3_handler.hh>
#include  <ost/io/img/map_io_dat_handler.hh>
#include  <ost/io/img/map_io_dx_handler.hh>
#include  <ost/io/img/map_io_spi_handler.hh>
#include  <ost/io/img/map_io_mrc_handler.hh>
#include  <ost/io/img/map_io_dm3_handler.hh>
#include  <ost/io/img/map_io_situs_handler.hh>
#include  <ost/io/img/map_io_tiff_handler.hh>
#include  <ost/io/img/map_io_png_handler.hh>
#include  <ost/io/img/map_io_dat_handler.hh>
#include  <ost/io/img/map_io_jpk_handler.hh>
#include  <ost/io/img/map_io_nanoscope_handler.hh>
#include  <ost/img/alg/normalizer_factory.hh>

using namespace ost;
using namespace ost::io;

BOOST_AUTO_TEST_SUITE( io )


BOOST_AUTO_TEST_CASE(test_io_img) 
{
  //float tests
  boost::test_tools::close_at_tolerance<Real> close_test(::boost::test_tools::percent_tolerance(0.0001));
  ost::img::ImageHandle testimage=ost::img::CreateImage(ost::img::Extent(ost::img::Point(0,0),ost::img::Point(3,3)));
  testimage.ApplyIP(ost::img::alg::Randomize());
  testimage+=0.01; //if all values are > 0.0 we can use close_at_tolerance
  const String fname("temp_img.tmp");
  std::map<String,ImageFormatBase*> float_formats;
  float_formats["DX"]=new DX;
  float_formats["Situs"]=new Situs;
  float_formats["DAT (float)"]=new DAT(false,OST_FLOAT_FORMAT);
  float_formats["CCP4 (float)"]=new MRC;
  float_formats["MRC (float)"]=new MRC(false,MRC_OLD_FORMAT);
  float_formats["SPIDER"]= new Spider;
  float_formats["JPK (float)"]= new JPK(false,OST_FLOAT_FORMAT);
  float_formats["JPK (double)"]= new JPK(false,OST_DOUBLE_FORMAT);
  float_formats["TIF (float)"]= new TIF(false,OST_FLOAT_FORMAT);
  float_formats["TIF (double)"]= new TIF(false,OST_DOUBLE_FORMAT);
  for(std::map<String,ImageFormatBase*>::iterator it=float_formats.begin();it!=float_formats.end();++it){
    ost::io::SaveImage(testimage,fname,*(it->second));
    ost::img::ImageHandle loadedimage=ost::io::LoadImage(fname,*(it->second));
    bool failed=false;
    ost::img::ExtentIterator eit(testimage.GetExtent());
    for(;!eit.AtEnd();++eit) {
      if( ! close_test(testimage.GetReal(eit),loadedimage.GetReal(eit))){
        failed=true;
        break;
      }
    }
    if(failed){
      BOOST_ERROR("Image IO failed for plugin " << it->first << " at point " << ost::img::Point(eit)<< ". The values are: " << testimage.GetReal(eit)<< ","<< loadedimage.GetReal(eit) );
    }
    delete it->second;
  }
  //int 16 formats
  std::map<String,ImageFormatBase*> int_formats;
  int_formats["DAT (16 bit)"]=new DAT(true,OST_BIT16_FORMAT);
  int_formats["TIF (16 bit)"]=new TIF;
  int_formats["JPK (16 bit)"]=new JPK;
  for(std::map<String,ImageFormatBase*>::iterator it=int_formats.begin();it!=int_formats.end();++it){
    ost::io::SaveImage(testimage,fname,*(it->second));
    ost::img::ImageHandle loadedimage=ost::io::LoadImage(fname,*(it->second));
    ost::img::alg::Normalizer norm=ost::img::alg::CreateLinearRangeNormalizer(testimage,0.0,65535.0);
    ost::img::ImageHandle scaled_image=testimage.Apply(norm);
    bool failed=false;
    ost::img::ExtentIterator eit(scaled_image.GetExtent());
    for(;!eit.AtEnd();++eit) {
      if( static_cast<int>(scaled_image.GetReal(eit))!=static_cast<int>(loadedimage.GetReal(eit))){
        failed=true;
        break;
      }
    }
    if(failed){
      BOOST_ERROR("Image IO failed for plugin " << it->first << " at point " << ost::img::Point(eit)<< ". The values are: " << static_cast<int>(scaled_image.GetReal(eit))<< ","<< static_cast<int>(loadedimage.GetReal(eit)) );
    }
    delete it->second;
  }

  //byte formats  
  std::map<String,ImageFormatBase*> byte_formats;
  byte_formats["DAT (byte)"]=new DAT(true,OST_BIT8_FORMAT);
  byte_formats["PNG"]=new PNG;
  byte_formats["DF3"]=new DF3;
  byte_formats["JPK (byte)"]= new JPK(false,OST_BIT8_FORMAT);
  byte_formats["TIF (byte)"]= new TIF(false,OST_BIT8_FORMAT);
  for(std::map<String,ImageFormatBase*>::iterator it=byte_formats.begin();it!=byte_formats.end();++it){
    ost::io::SaveImage(testimage,fname,*(it->second));
    ost::img::ImageHandle loadedimage=ost::io::LoadImage(fname,*(it->second));
    ost::img::alg::Normalizer norm=ost::img::alg::CreateLinearRangeNormalizer(testimage,0.0,255.0);
    ost::img::ImageHandle scaled_image=testimage.Apply(norm);
    bool failed=false;
    ost::img::ExtentIterator eit(scaled_image.GetExtent());
    for(;!eit.AtEnd();++eit) {
      if( static_cast<int>(scaled_image.GetReal(eit))!=static_cast<int>(loadedimage.GetReal(eit))){
        failed=true;
        break;
      }
    }
    if(failed){
      BOOST_ERROR("Image IO failed for plugin " << it->first << " at point " << ost::img::Point(eit)<< ". The values are: " << static_cast<int>(scaled_image.GetReal(eit))<< ","<< static_cast<int>(loadedimage.GetReal(eit)) );
    }
    delete it->second;
  }
}

BOOST_AUTO_TEST_SUITE_END()
