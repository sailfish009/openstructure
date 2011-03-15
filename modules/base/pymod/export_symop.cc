//------------------------------------------------------------------------------
// This file is part of the OpenStructure project <www.openstructure.org>
//
// Copyright (C) 2008-2011 by the OpenStructure authors
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

#include <boost/python.hpp>
#include <boost/python/suite/indexing/vector_indexing_suite.hpp>
#include <ost/symop.hh>
#include <ost/unit_cell.hh>
using namespace boost::python;
using namespace ost;

void export_Symop()
{
  class_<FractionalTransform>("FractionalFransform", init<>())
    .def(init<const geom::Mat3&, const geom::Vec3&>())
    .def(self*self)
    .def(self*geom::Vec3())
    .add_property("rot", 
                  make_function(&FractionalTransform::Rot, 
                                return_value_policy<copy_const_reference>()))
    .add_property("trans",
                  make_function(&FractionalTransform::Trans,
                                return_value_policy<copy_const_reference>()))
    .def("ToMat", &FractionalTransform::ToMat)
    .def("GetOrthoTF", &FractionalTransform::GetOrthoTF)
  ;
  class_<Symop, bases<FractionalTransform> >("Symop", init<>())
    .def(init<const geom::Mat3&, const geom::Vec3&>())
    .def("FromString", &Symop::FromString).staticmethod("FromString")
    
  ;
  
  class_<SymopList>("SymopList", init<>())
    .def(vector_indexing_suite<SymopList>())
  ;
}
