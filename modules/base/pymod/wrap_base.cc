//------------------------------------------------------------------------------
// This file is part of the OpenStructure project <www.openstructure.org>
//
// Copyright (C) 2008-2010 by the OpenStructure authors
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
#include <boost/python/suite/indexing/vector_indexing_suite.hpp>
#include <vector>
#include <ost/export_helper/vector.hh>
#include <ost/base.hh>
#include <ost/platform.hh>
#include <ost/message.hh>
#include <ost/version.hh>

using namespace boost::python;

void export_Logger();
void export_GenericProp();
void export_Range();
void export_Units();

// exception translator
void translator(const ost::Error& x) {
  PyErr_SetString(PyExc_Exception, x.what() );
}


BOOST_PYTHON_MODULE(_base)
{
  register_exception_translator<ost::Error>(&translator);

  def("SetPrefixPath", &ost::SetPrefixPath);
  def("GetPrefixPath", &ost::GetPrefixPath);
  def("GetSharedDataPath", &ost::GetSharedDataPath);  
  
  scope().attr("VERSION")=OST_VERSION_STRING;
  scope().attr("VERSION_MAJOR")=OST_VERSION_MAJOR;
  scope().attr("VERSION_MINOR")=OST_VERSION_MINOR;  
  scope().attr("VERSION_PATCH")=OST_VERSION_PATCH;
  export_Logger();
  export_Range();
  export_Units();
  typedef std::vector<Real> FloatList;
  class_<FloatList>("FloatList", init<>())
    .def(vector_indexing_suite<FloatList>())
    .def(ost::VectorAdditions<FloatList>())
  ;
  
  class_<std::vector<String> >("StringList", init<>())
    .def(vector_indexing_suite<std::vector<String> >())
  ;
  
  typedef std::vector<int> IntList;
  class_<std::vector<int> >("IntList", init<>())
    .def(vector_indexing_suite<std::vector<int> >())
    .def(ost::VectorAdditions<IntList>())
  ;  
}
