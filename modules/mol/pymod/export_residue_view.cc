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

#include <ost/mol/mol.hh>

using namespace ost;
using namespace ost::mol;

namespace {
typedef AtomView (ResidueView::*StringMethod)(const String&) const;
typedef AtomView (ResidueView::*HandleMethod)(const AtomHandle&) const;
typedef AtomView (ResidueView::*ViewMethod)(const AtomView&, ViewAddFlags);
typedef AtomView (ResidueView::*HandleMethodNonConst)(const AtomHandle&, ViewAddFlags);

StringMethod string_find_atom=&ResidueView::FindAtom;
HandleMethod handle_find_atom=&ResidueView::FindAtom;
HandleMethodNonConst add_atom_handle=&ResidueView::AddAtom;
ViewMethod add_atom_view=&ResidueView::AddAtom;
BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS(X_add_atom_overloads,
                                       ResidueView::AddAtom, 1, 2)
}

void export_ResidueView()
{
  class_<ResidueViewList>("ResidueViewList", no_init)
    .def(vector_indexing_suite<ResidueViewList>())
  ;

  void (ResidueView::* apply1)(EntityVisitor&) = &ResidueView::Apply;
  void (ResidueView::* apply2)(EntityViewVisitor&) = &ResidueView::Apply;

  class_<ResidueView, bases<ResidueBase> >("ResidueView", init<>())
    .def("GetChain",&ResidueView::GetChain)
    .def("GetAtomList", &ResidueView::GetAtomList,
         return_value_policy<copy_const_reference>())
    .add_property("atoms",
                  make_function(&ResidueView::GetAtomList,
                                return_value_policy<copy_const_reference>()))
    .def("FindAtom", string_find_atom, args("atom_name"))
    .def("AddAtom", add_atom_handle, X_add_atom_overloads(args("atom_handle", "flags")))
    .def("AddAtom", add_atom_view, X_add_atom_overloads(args("atom_view", "flags")))
    .def("FindAtom", handle_find_atom, args("atom_handle"))
    .def("IsAtomIncluded", &ResidueView::IsAtomIncluded, args("atom_handle"))
    .def("Apply", apply1, args("visitor"))
    .def("Apply", apply2, args("visitor"))
    .def("GetIndex", &ResidueView::GetIndex)
    .add_property("chain", &ResidueView::GetChain)
    .add_property("entity", &ResidueView::GetEntity)
    .add_property("index", &ResidueView::GetIndex)
    .def("RemoveAtoms", &ResidueView::RemoveAtom)
    .add_property("handle", &ResidueView::GetHandle)
    .def("GetHandle", &ResidueView::GetHandle)
    .def("RemoveAtom", &ResidueView::RemoveAtom)
    .def(self==self)
    .def(self!=self)    
    .def("GetGeometricCenter", &ResidueView::GetGeometricCenter)
    .add_property("GeometricCenter", &ResidueView::GetGeometricCenter)    
  ;


}
