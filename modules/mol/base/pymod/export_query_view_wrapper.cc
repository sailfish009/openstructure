//------------------------------------------------------------------------------
// This file is part of the OpenStructure project <www.openstructure.org>
//
// Copyright (C) 2008-2015 by the OpenStructure authors
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
using namespace boost::python;

#include <ost/mol/entity_view.hh>
#include <ost/mol/query.hh>
#include <ost/mol/query_view_wrapper.hh>


using namespace ost;
using namespace ost::mol;

void export_QueryViewWrapper()
{
  class_<QueryViewWrapper>("QueryViewWrapper", init<>())
    .def(init<const EntityHandle&>())
    .def(init<const EntityView&>())
    .def(init<const Query&, const EntityHandle&>())
    .def(init<const Query&, optional<const EntityView&> >())
    .def(init<const Query&, QueryFlags, const EntityHandle&>())
    .def(init<const Query&, QueryFlags, optional<const EntityView&> >())
    .def("GetEntityView",&QueryViewWrapper::GetEntityView)
    .def("DependsOnQuery",&QueryViewWrapper::DependsOnQuery)
    .def("IsDataValid", &QueryViewWrapper::IsDataValid)
    .def("SetQuery", &QueryViewWrapper::SetQuery)
    .def("GetQuery", &QueryViewWrapper::GetQuery
        ,return_value_policy<copy_const_reference>())
    .def("GetFlags", &QueryViewWrapper::GetFlags)
    .def("SetFlags", &QueryViewWrapper::SetFlags)
    .add_property("entity_view", &QueryViewWrapper::GetEntityView)
    .add_property("entity", &QueryViewWrapper::GetEntity)
    .add_property("query", make_function(&QueryViewWrapper::GetQuery
        ,return_value_policy<copy_const_reference>()))
  ;

}
