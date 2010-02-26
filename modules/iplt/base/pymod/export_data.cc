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

#include <boost/python.hpp>
using namespace boost::python;

#include <ost/iplt/data.hh>
#include <ost/iplt/algorithm.hh>
#include <ost/iplt/vecmat.hh>

void export_Data()
{
  using namespace ost::iplt;
  using namespace ost;

  enum_<DataType>("DataType")
    .value("REAL",REAL)
    .value("COMPLEX",COMPLEX)
    .value("WORD",WORD)
    ;

  enum_<DataDomain>("DataDomain")
    .value("SPATIAL",SPATIAL)
    .value("FREQUENCY",FREQUENCY)
    .value("HALF_FREQUENCY",HALF_FREQUENCY)
    ;

  // data wrapper
  void (ConstData::*apply1)(NonModAlgorithm&) const = &ConstData::Apply;

  Real (ConstData::*getintpolreal0)(const Vec3&) const=&Data::GetIntpolReal;
  Real (ConstData::*getintpolreal1)(const Vec2&) const=&Data::GetIntpolReal;
  Real (ConstData::*getintpolreal2)(const Real&) const=&Data::GetIntpolReal;

  Complex (ConstData::*getintpolcomplex0)(const Vec3&) const=&Data::GetIntpolComplex;
  Complex (ConstData::*getintpolcomplex1)(const Vec2&) const=&Data::GetIntpolComplex;
  Complex (ConstData::*getintpolcomplex2)(const Real&) const=&Data::GetIntpolComplex;

  class_<ConstData, boost::noncopyable>("ConstData", no_init )
    .def("GetType",&ConstData::GetType)
    .def("GetDomain",&ConstData::GetDomain)
    .def("GetExtent",&ConstData::GetExtent)
    .def("GetReal",&ConstData::GetReal)
    .def("GetComplex",&ConstData::GetComplex)
    .def("Apply",apply1)
    .def("GetSize",&ConstData::GetSize)
    .def("IsSpatial",&ConstData::IsSpatial)
    .def("IsFrequency",&ConstData::IsFrequency)
    .def("GetPixelSampling",&ConstData::GetPixelSampling)
    .def("GetSpatialSampling",&ConstData::GetSpatialSampling)
    .def("GetFrequencySampling",&ConstData::GetFrequencySampling)
    //.def("IndexToCoord",&ConstData::IndexToCoord)
    //.def("CoordToindex",&ConstData::CoordToIndex)
    .def("GetIntpolReal",getintpolreal0)
    .def("GetIntpolReal",getintpolreal1)
    .def("GetIntpolReal",getintpolreal2)
    .def("GetIntpolComplex",getintpolcomplex0)
    .def("GetIntpolComplex",getintpolcomplex1)
    .def("GetIntpolComplex",getintpolcomplex2)
    ;

  void (Data::*setpixelsampling0)(const Vec3&)=&Data::SetPixelSampling;
  void (Data::*setpixelsampling1)(Real)=&Data::SetPixelSampling;
  void (Data::*setspatialsampling0)(const Vec3&)=&Data::SetSpatialSampling;
  void (Data::*setspatialsampling1)(Real)=&Data::SetSpatialSampling;

  class_<Data, bases<ConstData>,boost::noncopyable>("Data", no_init)
    .def("SetSpatialOrigin",&Data::SetSpatialOrigin)
    .def("SetPixelSampling",setpixelsampling0)
    .def("SetPixelSampling",setpixelsampling1)
    .def("SetSpatialSampling",setspatialsampling0)
    .def("SetSpatialSampling",setspatialsampling1)
    ;
}
