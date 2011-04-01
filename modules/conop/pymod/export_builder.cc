//------------------------------------------------------------------------------
// This file is part of the OpenStructure project <www.openstructure.org>
//
// Copyright (C) 2008-2011 by the OpenStructure authors
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
#include <ost/conop/builder.hh>
#include <ost/conop/heuristic_builder.hh>
#include <ost/conop/rule_based_builder.hh>

using namespace ost::conop;

void export_Builder() {
  
  enum_<Dialect>("Dialect")
    .value("PDB_DIALECT", PDB_DIALECT)
    .value("CHARMM_DIALECT", CHARMM_DIALECT)
    .export_values()
  ;
  //TODO Export virtual calls as Default* (see export_visitor.cc)
  class_<Builder>("Builder", no_init)
    .add_property("dialect", &Builder::GetDialect, &Builder::SetDialect)
    .add_property("strict_hydrogens", &Builder::GetStrictHydrogenMode, 
                  &Builder::SetStrictHydrogenMode)
    .def("GetDialect", &Builder::GetDialect)
    .def("SetDialect", &Builder::SetDialect)
    .def("CompleteAtoms", &Builder::CompleteAtoms)
    .def("CheckResidueCompleteness", &Builder::CheckResidueCompleteness)
    .def("IdentifyResidue", &Builder::IdentifyResidue)
    .def("ConnectAtomsOfResidue", &Builder::ConnectAtomsOfResidue)
    .def("ConnectResidueToNext", &Builder::ConnectResidueToNext)
    .def("ConnectResidueToPrev", &Builder::ConnectResidueToPrev)
    .def("AssignTorsions", &Builder::AssignTorsions)
    .def("AssignTorsionsToResidue", &Builder::AssignTorsionsToResidue)
    .def("FillAtomProps", &Builder::FillAtomProps)
    .def("IsResidueComplete", &Builder::IsResidueComplete)
  ;

  class_<HeuristicBuilder, bases<Builder> >("HeuristicBuilder", init<>())
  ;
  class_<RuleBasedBuilder, bases<Builder> >("RuleBasedBuilder", 
                                            init<const CompoundLibPtr&>())
     .add_property("compound_lib", &RuleBasedBuilder::GetCompoundLib)
  ;
}
