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
#if OST_TIFF_ENABLED
#include  <ost/io/img/map_io_tiff_handler.hh>
#include  <ost/io/img/map_io_jpk_handler.hh>
#endif
#if OST_PNG_ENABLED
#include  <ost/io/img/map_io_png_handler.hh>
#endif
#include  <ost/io/img/map_io_dat_handler.hh>
#include  <ost/io/img/map_io_nanoscope_handler.hh>
#include  <ost/io/img/map_io_ipl_handler.hh>
#include  <ost/img/alg/normalizer_factory.hh>

using namespace ost;
using namespace ost::io;

BOOST_AUTO_TEST_SUITE( io )


BOOST_AUTO_TEST_CASE(test_io_img) 
{
  //float tests
  boost::test_tools::close_at_tolerance<Real> close_test(::boost::test_tools::percent_tolerance(0.001));
  ost::img::ImageHandle testimage=ost::img::CreateImage(ost::img::Extent(ost::img::Point(0,0),ost::img::Point(4,3)));
  int counter=0;
  for (img::ExtentIterator i(testimage.GetExtent()); !i.AtEnd(); ++i, ++counter) {
   testimage.SetReal(i, counter);
  }
  testimage+=5.01; //if all values are > 0.0 we can use close_at_tolerance
  const String fname("temp_img.tmp");
  std::map<String,ImageFormatBase*> float_formats;
  float_formats["DX"]=new DX;
  float_formats["Situs"]=new Situs;
  float_formats["CCP4 (float)"]=new MRC;
  float_formats["MRC (float)"]=new MRC(false,MRC_OLD_FORMAT);
  float_formats["SPIDER"]= new Spider;
#if OST_TIFF_ENABLED
  float_formats["JPK (float)"]= new JPK(false,OST_FLOAT_FORMAT);
  float_formats["JPK (double)"]= new JPK(false,OST_DOUBLE_FORMAT);
  float_formats["TIF (float)"]= new TIF(false,OST_FLOAT_FORMAT);
  float_formats["TIF (double)"]= new TIF(false,OST_DOUBLE_FORMAT);
#endif
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
  int_formats["IPL (16 bit)"]=new IPL(true,OST_BIT16_FORMAT);
#if OST_TIFF_ENABLED
  int_formats["TIF (16 bit)"]=new TIF;
  int_formats["JPK (16 bit)"]=new JPK;
#endif
  // int_formats["DF3"]=new DF3(true);
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
      BOOST_ERROR("Image IO failed for plugin " << it->first << " at point " 
                  << ost::img::Point(eit)<< ". Should be " 
                  << static_cast<int>(scaled_image.GetReal(eit)) << ", but "
                  << static_cast<int>(loadedimage.GetReal(eit)) << " found.");
    }
    delete it->second;
  }

  //int 32 formats
  std::map<String,ImageFormatBase*> int32_formats;
  int32_formats["IPL (32 bit)"]=new IPL(true,OST_BIT32_FORMAT);
  for(std::map<String,ImageFormatBase*>::iterator it=int32_formats.begin();it!=int32_formats.end();++it){
    ost::io::SaveImage(testimage,fname,*(it->second));
    ost::img::ImageHandle loadedimage=ost::io::LoadImage(fname,*(it->second));
    ost::img::alg::Normalizer norm=ost::img::alg::CreateLinearRangeNormalizer(testimage,0.0,4294967295.0);
    ost::img::ImageHandle scaled_image=testimage.Apply(norm);
    bool failed=false;
    ost::img::ExtentIterator eit(scaled_image.GetExtent());
    for(;!eit.AtEnd();++eit) {
      if( static_cast<uint>(scaled_image.GetReal(eit))!=static_cast<uint>(loadedimage.GetReal(eit))){
        failed=true;
        break;
      }
    }
    if(failed){
      BOOST_ERROR("Image IO failed for plugin " << it->first << " at point "
                  << ost::img::Point(eit)<< ". Should be "
                  << static_cast<uint>(scaled_image.GetReal(eit)) << ", but "
                  << static_cast<uint>(loadedimage.GetReal(eit)) << " found.");
    }
    delete it->second;
  }

  //byte formats  
  std::map<String,ImageFormatBase*> byte_formats;
#if OST_PNG_ENABLED
  byte_formats["PNG"]=new PNG;
#endif
#if OST_TIFF_ENABLED
  byte_formats["TIF (byte)"]= new TIF(true,OST_BIT8_FORMAT);
  byte_formats["JPK (byte)"]= new JPK(true,OST_BIT8_FORMAT);
#endif
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

BOOST_AUTO_TEST_CASE(test_io_img_dat)
{
  // test for the dat file format using a square image (non square images not supported by dat)
  //float test
  boost::test_tools::close_at_tolerance<Real> close_test(::boost::test_tools::percent_tolerance(0.001));
  ost::img::ImageHandle testimage=ost::img::CreateImage(ost::img::Extent(ost::img::Point(0,0),ost::img::Point(3,3)));
  int counter=0;
  for (img::ExtentIterator i(testimage.GetExtent()); !i.AtEnd(); ++i, ++counter) {
   testimage.SetReal(i, counter);
  }
  testimage+=5.01; //if all values are > 0.0 we can use close_at_tolerance
  const String fname("temp_img.tmp");
  ost::io::SaveImage(testimage,fname,DAT(false,OST_FLOAT_FORMAT));
  ost::img::ImageHandle loadedimage=ost::io::LoadImage(fname,DAT(false,OST_FLOAT_FORMAT));
  bool failed=false;
  ost::img::ExtentIterator eit(testimage.GetExtent());
  for(;!eit.AtEnd();++eit) {
    if( ! close_test(testimage.GetReal(eit),loadedimage.GetReal(eit))){
      failed=true;
      break;
    }
  }
  if(failed){
    BOOST_ERROR("Image IO failed for plugin DAT (float) at point " << ost::img::Point(eit)<< ". The values are: " << testimage.GetReal(eit)<< ","<< loadedimage.GetReal(eit) );
  }
  //int 16 format
  ost::io::SaveImage(testimage,fname,DAT(true,OST_BIT16_FORMAT));
  loadedimage=ost::io::LoadImage(fname,DAT(true,OST_BIT16_FORMAT));
  ost::img::alg::Normalizer norm=ost::img::alg::CreateLinearRangeNormalizer(testimage,0.0,65535.0);
  ost::img::ImageHandle scaled_image=testimage.Apply(norm);
  failed=false;
  eit=ost::img::ExtentIterator(testimage.GetExtent());
  for(;!eit.AtEnd();++eit) {
    if( static_cast<int>(scaled_image.GetReal(eit))!=static_cast<int>(loadedimage.GetReal(eit))){
      failed=true;
      break;
    }
  }
  if(failed){
    BOOST_ERROR("Image IO failed for plugin DAT  (int16) at point "
                << ost::img::Point(eit)<< ". Should be "
                << static_cast<int>(scaled_image.GetReal(eit)) << ", but "
                << static_cast<int>(loadedimage.GetReal(eit)) << " found.");
  }

  //byte format
  ost::io::SaveImage(testimage,fname,DAT(true,OST_BIT8_FORMAT));
  loadedimage=ost::io::LoadImage(fname,DAT(true,OST_BIT8_FORMAT));
  norm=ost::img::alg::CreateLinearRangeNormalizer(testimage,0.0,255.0);
  scaled_image=testimage.Apply(norm);
  failed=false;
  eit=ost::img::ExtentIterator(testimage.GetExtent());
  for(;!eit.AtEnd();++eit) {
    if( static_cast<int>(scaled_image.GetReal(eit))!=static_cast<int>(loadedimage.GetReal(eit))){
      failed=true;
      break;
    }
  }
  if(failed){
    BOOST_ERROR("Image IO failed for plugin DAT  (int8) at point "
                << ost::img::Point(eit)<< ". Should be "
                << static_cast<int>(scaled_image.GetReal(eit)) << ", but "
                << static_cast<int>(loadedimage.GetReal(eit)) << " found.");
  }
}

BOOST_AUTO_TEST_SUITE_END()
