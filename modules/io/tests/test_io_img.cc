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
#include <vector>
#include <ost/io/load_map.hh>
#include <ost/img/image_factory.hh>
#include <ost/img/alg/randomize.hh>
#define BOOST_TEST_DYN_LINK
#include <boost/test/unit_test.hpp>
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
  ost::img::ImageHandle testimage=ost::img::CreateImage(ost::img::Extent(ost::img::Point(0,0),ost::img::Point(3,3)));
  testimage.ApplyIP(ost::img::alg::Randomize());
  const String fname("temp_img.tmp");
  std::vector<ImageFormatBase*> float_formats;
  float_formats.push_back(new MRC);
  float_formats.push_back(new MRC(false,MRC_OLD_FORMAT));
  float_formats.push_back(new Spider);
  float_formats.push_back(new JPK(false,OST_FLOAT_FORMAT));
  float_formats.push_back(new JPK(false,OST_DOUBLE_FORMAT));
  float_formats.push_back(new TIF(false,OST_FLOAT_FORMAT));
  float_formats.push_back(new TIF(false,OST_DOUBLE_FORMAT));
  for(std::vector<ImageFormatBase*>::iterator it=float_formats.begin();it!=float_formats.end();++it){
    ost::io::SaveImage(testimage,fname,*(*it));
    ost::img::ImageHandle loadedimage=ost::io::LoadImage(fname,*(*it));
    for(ost::img::ExtentIterator eit(testimage.GetExtent());!eit.AtEnd();++eit) {
      BOOST_CHECK_CLOSE(testimage.GetReal(eit),loadedimage.GetReal(eit),0.0001);
    }
    delete *it;
  }
  std::vector<ImageFormatBase*> int_formats;
  int_formats.push_back(new TIF);
  int_formats.push_back(new JPK);
  int_formats.push_back(new PNG);
  for(std::vector<ImageFormatBase*>::iterator it=int_formats.begin();it!=int_formats.end();++it){
    ost::io::SaveImage(testimage,fname,*(*it));
    ost::img::ImageHandle loadedimage=ost::io::LoadImage(fname,*(*it));
    ost::img::alg::Normalizer norm=ost::img::alg::CreateLinearRangeNormalizer(testimage,(*it)->GetMinimum(),(*it)->GetMaximum());
    ost::img::ImageHandle scaled_image=testimage.Apply(norm);
    for(ost::img::ExtentIterator eit(testimage.GetExtent());!eit.AtEnd();++eit) {
      BOOST_CHECK_CLOSE(scaled_image.GetReal(eit),loadedimage.GetReal(eit),0.0001);
    }
    delete *it;
  }
  
}

BOOST_AUTO_TEST_SUITE_END()
