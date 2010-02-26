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
using namespace boost::python;

#include <ost/mol/entity_view.hh>
#include <ost/mol/query.hh>
#include <ost/mol/mol.hh>
#include <ost/mol/bounding_box.hh>
using namespace ost;
using namespace ost::mol;

namespace {

BoundingBox create_bbox_a(const EntityView& ent)
{
  return BoundingBoxFromEntity(ent);
}

BoundingBox create_bbox_b(const EntityHandle& ent)
{
  return BoundingBoxFromEntity(ent);
}

}

void export_BoundingBox()
{
  class_<BoxAxis>("BoxAxis", init<>())
    .def(init<const geom::Vec3&, double>())
    .def("GetHalfExtent", &BoxAxis::GetHalfExtent)
    .def("GetVector", &BoxAxis::GetVector,
         return_value_policy<copy_const_reference>())
    .def("GetExtent", &BoxAxis::GetExtent)
  ;
  class_<BoundingBox>("BoundingBox", init<>())
    .def(init<const geom::Vec3&, const BoxAxis&,const BoxAxis&, 
              const BoxAxis&>())
    .def("GetCenter",&BoundingBox::GetCenter)
    .def("GetVecA", &BoundingBox::GetVecA,
         return_value_policy<copy_const_reference>())
    .def("GetVecB", &BoundingBox::GetVecB,
         return_value_policy<copy_const_reference>())
    .def("GetVecC", &BoundingBox::GetVecC,
         return_value_policy<copy_const_reference>())
    .def("GetAxisA", &BoundingBox::GetAxisA, 
         return_value_policy<copy_const_reference>())
    .def("GetAxisB", &BoundingBox::GetAxisB, 
         return_value_policy<copy_const_reference>())
    .def("GetAxisC", &BoundingBox::GetAxisC, 
         return_value_policy<copy_const_reference>())                  
    .def("GetHalfExtents", &BoundingBox::GetHalfExtents)
  ;
  def("BoundingBoxFromEntity", &create_bbox_a);
  def("BoundingBoxFromEntity", &create_bbox_b);
}
