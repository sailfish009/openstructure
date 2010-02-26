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
#include <ost/mol/chain_view.hh>
#include <ost/mol/query.hh>
#include <ost/mol/chain_handle.hh>
#include <ost/mol/entity_visitor.hh>

using namespace ost;
using namespace ost::mol;

namespace {
typedef ResidueView (ChainView::*RnumMethod)(const ResNum&) const;
typedef ResidueView (ChainView::*HandleMethod)(const ResidueHandle&) const;

RnumMethod rnum_find_residue=&ChainView::FindResidue;
HandleMethod handle_find_residue=&ChainView::FindResidue;    

typedef ResidueView (ChainView::*HM)(const ResidueHandle&, ViewAddFlags);
typedef ResidueView (ChainView::*VM)(const ResidueView&, ViewAddFlags);
VM vm=&ChainView::AddResidue;
HM hm=&ChainView::AddResidue;
BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS(X_add_residue_overloads, 
                                      ChainView::AddResidue, 1, 2)
                                      
                                      
BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS(X_add_atom_overloads, 
                                      ChainView::AddAtom, 1, 2)
                                      
AtomView (ChainView::*find_atom_a)(const ResNum&, const String&) const=&ChainView::FindAtom;
AtomView (ChainView::*find_atom_b)(const AtomHandle&) const=&ChainView::FindAtom;                                      
}

void export_ChainView()
{
  class_<ChainViewList>("ChainViewList", no_init)
    .def(vector_indexing_suite<ChainViewList>())
  ;

  void (ChainView::* apply1)(EntityVisitor&) = &ChainView::Apply;
  void (ChainView::* apply2)(EntityViewVisitor&) = &ChainView::Apply;

  class_<ChainView, bases<ChainBase> >("ChainView", init<>())
    .def("GetResidueList", &ChainView::GetResidueList,
         return_value_policy<copy_const_reference>())
    .add_property("residues", 
                  make_function(&ChainView::GetResidueList,
                                return_value_policy<copy_const_reference>()))
    .def("GetEntity", &ChainView::GetEntity)
    .def("FindResidue",rnum_find_residue, arg("residue_number"))
    .def("FindResidue", handle_find_residue, arg("residue_handle"))
    .def("FindAtom", find_atom_a)
    .def("FindAtom", find_atom_b)    
    .def("AddAtom", &ChainView::AddAtom, 
         X_add_atom_overloads(args("atom_handle", "view_add_flags")))
    .def("AddResidue", hm, 
         X_add_residue_overloads(args("residue_handle", "view_add_flags")))
    .def("AddResidue", vm, 
         X_add_residue_overloads(args("residue_view", "view_add_flags")))         
    .def("Apply", apply1, args("visitor"))
    .def("Apply", apply2, args("visitor"))
    .def("RemoveResidue", &ChainView::RemoveResidue)
    .def("RemoveResidues", &ChainView::RemoveResidues)
    .def("InSequence", &ChainView::InSequence)
    .def("GetResidueByIndex", &ChainView::GetResidueByIndex)
    .def("GetAtomCount", &ChainView::GetAtomCount)
    .def("GetResidueCount", &ChainView::GetResidueCount) 
    .def("GetHandle", &ChainView::GetHandle)
    .add_property("handle", &ChainView::GetHandle)
    .add_property("residue_count", &ChainView::GetResidueCount)
    .def("GetGeometricCenter", &ChainView::GetGeometricCenter)
    .add_property("geometric_center", &ChainView::GetGeometricCenter)
  ;


}
