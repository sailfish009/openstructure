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
#include <ost/symop.hh>
#include <ost/symmetry.hh>

using namespace boost::python;
using namespace ost;

void export_Symmetry()
{
  enum_<SymmetryType>("SymmetryType")
    .value("SYM_ORTHORHOMBIC", SYM_ORTHORHOMBIC)
    .value("SYM_MONOCLINIC", SYM_MONOCLINIC)
    .value("SYM_CUBIC", SYM_CUBIC)
    .value("SYM_TETRAGONAL", SYM_TETRAGONAL)
    .value("SYM_TRICLINIC", SYM_TRICLINIC)
    .value("SYM_HEXAGONAL", SYM_HEXAGONAL)
    .value("SYM_TRIGONAL", SYM_TRIGONAL)
    .value("SYM_UNKNOWN", SYM_UNKNOWN)
    .export_values()
  ;
  class_<Symmetry, SymmetryPtr>("Symmetry", no_init)
    .def("FromHermannMauguinSymbol", 
         &Symmetry::FromHermannMauguinSymbol).staticmethod("FromHermannMauguinSymbol")
    .add_property("hermann_mauguin_symbol", 
                  make_function(&Symmetry::GetHermannMauguinSymbol, 
                                return_value_policy<copy_const_reference>()))
    .add_property("ccp4_symbol", 
                  make_function(&Symmetry::GetCCP4Symbol, 
                                return_value_policy<copy_const_reference>()))
    .add_property("symops", 
                  make_function(&Symmetry::GetSymops,
                                return_value_policy<reference_existing_object>()))
    .add_property("type", &Symmetry::GetType)
    
  ;
}
