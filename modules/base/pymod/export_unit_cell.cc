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
#include <ost/symmetry.hh>
#include <ost/unit_cell.hh>

using namespace boost::python;
using namespace ost;

void export_UnitCell()
{
  class_<UnitCell>("UnitCell", init<>())
    .def(init<Real, Real,Real,Real,Real,Real, 
              const SymmetryPtr&>((arg("a"),arg("b"),arg("c"),
                                               arg("alpha"),arg("beta"),
                                               arg("gamma"), 
                                               arg("sym")=SymmetryPtr())))
    .add_property("alpha", &UnitCell::GetAlpha)
    .add_property("beta", &UnitCell::GetBeta)
    .add_property("gamma", &UnitCell::GetGamma)
    .add_property("a", &UnitCell::GetA)
    .add_property("b", &UnitCell::GetB)
    .add_property("c", &UnitCell::GetC)
    .def("GetOrthoTF", &UnitCell::GetOrthoTF, 
         return_value_policy<copy_const_reference>())
         
    .def("GetFractionalTF", &UnitCell::GetFractionalTF, 
         return_value_policy<copy_const_reference>())
    .add_property("ortho_tf", make_function(&UnitCell::GetOrthoTF,
                  return_value_policy<copy_const_reference>()))
    .add_property("symmetry", make_function(&UnitCell::GetSymmetry,
                  return_value_policy<copy_const_reference>()))                  
    .add_property("fractional_tf", make_function(&UnitCell::GetFractionalTF,
                  return_value_policy<copy_const_reference>()))
  ;
}
