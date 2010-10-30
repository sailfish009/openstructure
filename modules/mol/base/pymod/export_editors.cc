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

BondHandle (EditorBase::*connect_a)(const AtomHandle&, 
                                    const AtomHandle&)=&EditorBase::Connect;
BondHandle (EditorBase::*connect_c)(const AtomHandle&, 
                                    const AtomHandle&,
                                    unsigned char)=&EditorBase::Connect;
BondHandle (EditorBase::*connect_b)(const AtomHandle&, const AtomHandle&, 
                                    Real, Real, Real)=&EditorBase::Connect;
BondHandle (EditorBase::*connect_d)(const AtomHandle&, const AtomHandle&, 
                                    Real, Real, Real,
                                    unsigned char)=&EditorBase::Connect;
                                    
ResidueHandle (EditorBase::*append_a)(ChainHandle ch, 
                                      const ResidueKey&)=&EditorBase::AppendResidue;
ResidueHandle (EditorBase::*append_b)(ChainHandle ch, const ResidueKey&, 
                                      const ResNum&)=&EditorBase::AppendResidue;

void (ICSEditor::*set_torsion_a)(TorsionHandle, Real)=&ICSEditor::SetTorsionAngle;
void (ICSEditor::*set_torsion_b)(const AtomHandle&, const AtomHandle&,
                                 const AtomHandle&, const AtomHandle&,
                                 Real)=&ICSEditor::SetTorsionAngle;

void (ICSEditor::*rotate_torsion_a)(TorsionHandle, Real)=&ICSEditor::RotateTorsionAngle;
void (ICSEditor::*rotate_torsion_b)(const AtomHandle&, const AtomHandle&,
                                    const AtomHandle&, const AtomHandle&,
                                    Real)=&ICSEditor::RotateTorsionAngle;
                                 
BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS(X_insert_atom_overloads, 
                                       EditorBase::InsertAtom, 3, 4)
}

void export_Editors()
{  
  class_<EditorBase>("EditorBase", no_init)
    .def("InsertChain", &EditorBase::InsertChain)
    .def("InsertAtom", &EditorBase::InsertAtom,
         X_insert_atom_overloads())
    .def("InsertAltAtom", &EditorBase::InsertAltAtom)
    .def("DeleteResidue", &EditorBase::DeleteResidue)
    .def("DeleteChain", &EditorBase::DeleteChain)
    .def("DeleteAtom", &EditorBase::DeleteAtom)
    .def("InsertResidueBefore", &EditorBase::InsertResidueBefore)
    .def("InsertResidueAfter", &EditorBase::InsertResidueAfter)    
    .def("AppendResidue", append_a)
    .def("AppendResidue", append_b)    
    .def("Connect", connect_a)
    .def("Connect", connect_b)    
    .def("Connect", connect_c)
    .def("Connect", connect_d)    
    .def("RenameChain", &EditorBase::RenameChain)
    .def("AddTorsion", &EditorBase::AddTorsion)
    .def("ReorderResidues",&EditorBase::ReorderResidues)
    .def("ReorderAllResidues",&EditorBase::ReorderAllResidues)
  ;
  
  class_<XCSEditor, bases<EditorBase> >("XCSEditor", no_init)
    .def("SetAtomPos", &XCSEditor::SetAtomPos)
    .def("SetAtomOriginalPos", &XCSEditor::SetAtomOriginalPos)
    .def("ApplyTransform", &XCSEditor::ApplyTransform)
    .def("SetTransform", &XCSEditor::SetTransform)
    .def("UpdateICS", &XCSEditor::UpdateICS)
    .def("__exit__", &XCSEditor::UpdateICS)    
  ;
  
  class_<ICSEditor, bases<EditorBase> >("ICSEditor", no_init)
    .def("SetAngle", &ICSEditor::SetAngle)
    .def("SetBondLength", &ICSEditor::SetBondLength)
    .def("SetTorsionAngle", set_torsion_a)
    .def("SetTorsionAngle", set_torsion_b)    
    .def("RotateTorsionAngle", rotate_torsion_a)
    .def("RotateTorsionAngle", rotate_torsion_b)
    .def("UpdateXCS", &ICSEditor::UpdateXCS)
    .def("__exit__", &ICSEditor::UpdateXCS)
  ;  
}
