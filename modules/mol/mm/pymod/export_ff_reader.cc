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
#include <boost/python.hpp>
#include <ost/mol/mm/ff_reader.hh>
#include <ost/mol/residue_handle.hh>

using namespace boost::python;
using namespace ost::mol::mm;


void export_FFReader()
{
  class_<ost::mol::mm::FFReader>("FFReader",init<String>())
    .def("ReadGromacsForcefield",&ost::mol::mm::FFReader::ReadGromacsForcefield)
    .def("SetPreprocessorDefinition",&ost::mol::mm::FFReader::SetPreprocessorDefinition)
    .def("GetForcefield",&ost::mol::mm::FFReader::GetForcefield)
    .def("SetForcefield",&ost::mol::mm::FFReader::SetForcefield)
    .def("ReadResidueDatabase",&ost::mol::mm::FFReader::ReadResidueDatabase)
    .def("ReadITP",&ost::mol::mm::FFReader::ReadITP)   
    .def("ReadCHARMMPRM",&ost::mol::mm::FFReader::ReadCHARMMPRM) 
    .def("ReadCHARMMRTF",&ost::mol::mm::FFReader::ReadCHARMMRTF)                                                                      
  ;

  boost::python::register_ptr_to_python<ost::mol::mm::FFReaderPtr>();
}