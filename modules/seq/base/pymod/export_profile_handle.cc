//------------------------------------------------------------------------------
// This file is part of the OpenStructure project <www.openstructure.org>
//
// Copyright (C) 2008-2020 by the OpenStructure authors
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
  Author: Gerardo Tauriello, Gabriel Studer
 */

#include <boost/python/suite/indexing/vector_indexing_suite.hpp>
#include <boost/python.hpp>
#include <ost/seq/profile_handle.hh>

using namespace ost::seq;
using namespace boost::python;

namespace {

boost::python::list wrap_get_names(ProfileDBPtr db) {
  std::vector<String> v_names = db->GetNames();
  boost::python::list names;
  for (std::vector<String>::iterator i = v_names.begin();
       i != v_names.end(); ++i) {
    names.append(*i);
  }
  return names;
}

} // anon ns

void export_profile_handle() 
{
  class_<ProfileColumn>("ProfileColumn", init<>())
    .add_property("entropy", &ProfileColumn::GetEntropy)
    .def("GetFreq", &ProfileColumn::GetFreq, (arg("aa")))
    .def("SetFreq", &ProfileColumn::SetFreq, (arg("aa"), arg("freq")))
    .def("GetScore", &ProfileColumn::GetScore,
         (arg("other"), arg("null_model")))
    .def("BLOSUMNullModel", &ProfileColumn::BLOSUMNullModel)
    .staticmethod("BLOSUMNullModel")
    .def("HHblitsNullModel", &ProfileColumn::HHblitsNullModel)
    .staticmethod("HHblitsNullModel")
  ;

  class_<ProfileColumnList>("ProfileColumnList", init<>())
    .def(vector_indexing_suite<ProfileColumnList>())
  ;

  class_<ProfileHandle, ProfileHandlePtr>("ProfileHandle", init<>())
    .def("__len__",&ProfileHandle::size)
    .def("AddColumn", &ProfileHandle::AddColumn, (arg("col"), arg("olc")='X'))
    .def("Extract", &ProfileHandle::Extract, (arg("from"), arg("to")))
    .def("GetAverageScore", &ProfileHandle::GetAverageScore,
         (arg("other"), arg("offset")=0))
    .add_property("null_model",
                  make_function(&ProfileHandle::GetNullModel,
                                return_value_policy<copy_const_reference>()),
                  &ProfileHandle::SetNullModel)
    .add_property("columns",
                  make_function(&ProfileHandle::GetColumns,
                                return_value_policy<copy_const_reference>()))
    .add_property("avg_entropy", &ProfileHandle::GetAverageEntropy)
    .add_property("sequence", &ProfileHandle::GetSequence,
                              &ProfileHandle::SetSequence)
  ;

  class_<ProfileDB, ProfileDBPtr>("ProfileDB", init<>())
    .def("Load", &ProfileDB::Load, (arg("filename"))).staticmethod("Load")
    .def("Save", &ProfileDB::Save, (arg("filename")))
    .def("AddProfile", &ProfileDB::AddProfile, (arg("name"), arg("prof")))
    .def("GetProfile", &ProfileDB::GetProfile, (arg("name")))
    .def("Size", &ProfileDB::size)
    .def("GetNames",&wrap_get_names)
  ;
}
