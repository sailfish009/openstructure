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
#include <boost/shared_ptr.hpp>
#include <ost/mol/mm/forcefield.hh>

using namespace boost::python;

namespace{


  ost::mol::mm::MMInteractionPtr GetLJOneType(ost::mol::mm::ForcefieldPtr p, String type){
    return p->GetLJ(type);
  }

  ost::mol::mm::MMInteractionPtr GetLJTwoTypes(ost::mol::mm::ForcefieldPtr p, String type1, String type2, bool pair){
    return p->GetLJ(type1,type2,pair);
  }

}


void export_Forcefield()
{

  class_<ost::mol::mm::Forcefield>("Forcefield", init<>())
    .def("Load",&ost::mol::mm::Forcefield::Load).staticmethod("Load")
    .def("Save",&ost::mol::mm::Forcefield::Save)
    .def("GetBuildingBlock",&ost::mol::mm::Forcefield::GetBuildingBlock)
    .def("GetAtomType",&ost::mol::mm::Forcefield::GetAtomType)
    .def("GetHydrogenConstructor",&ost::mol::mm::Forcefield::GetHydrogenConstructor)
    .def("GetNTerModifier",&ost::mol::mm::Forcefield::GetNTerModifier,(arg("residue_name"),arg("ter_name")=""))
    .def("GetCTerModifier",&ost::mol::mm::Forcefield::GetCTerModifier,(arg("residue_name"),arg("ter_name")=""))
    .def("GetBond",&ost::mol::mm::Forcefield::GetBond)
    .def("GetAngle",&ost::mol::mm::Forcefield::GetAngle)
    .def("GetDihedrals",&ost::mol::mm::Forcefield::GetDihedrals)
    .def("GetImpropers",&ost::mol::mm::Forcefield::GetImpropers)
    .def("GetCMap",&ost::mol::mm::Forcefield::GetCMap)
    .def("GetImplicitGenborn",&ost::mol::mm::Forcefield::GetImplicitGenborn)
    .def("GetLJ",&GetLJOneType)
    .def("GetLJ",&GetLJTwoTypes)
    .def("GetConstraint",&ost::mol::mm::Forcefield::GetConstraint)
    .def("GetMass",&ost::mol::mm::Forcefield::GetMass)
    .def("GetFudgeLJ",&ost::mol::mm::Forcefield::GetFudgeLJ)
    .def("GetFudgeQQ",&ost::mol::mm::Forcefield::GetFudgeQQ)
    .def("AddBuildingBlock",&ost::mol::mm::Forcefield::AddBuildingBlock)
    .def("AddBond",&ost::mol::mm::Forcefield::AddBond) 
    .def("AddAngle",&ost::mol::mm::Forcefield::AddAngle) 
    .def("AddDihedral",&ost::mol::mm::Forcefield::AddDihedral) 
    .def("AddImproper",&ost::mol::mm::Forcefield::AddImproper) 
    .def("AddCMap",&ost::mol::mm::Forcefield::AddCMap) 
    .def("AddImplicitGenborn",&ost::mol::mm::Forcefield::AddImplicitGenborn) 
    .def("AddLJ",&ost::mol::mm::Forcefield::AddLJ) 
    .def("AddLJPair",&ost::mol::mm::Forcefield::AddLJPair) 
    .def("AddMass",&ost::mol::mm::Forcefield::AddMass) 
    .def("AddResidueRenamingRule",&ost::mol::mm::Forcefield::AddResidueRenamingRule) 
    .def("AddAtomRenamingRule",&ost::mol::mm::Forcefield::AddAtomRenamingRule) 
    .def("AddHydrogenConstructor",&ost::mol::mm::Forcefield::AddHydrogenConstructor) 
    .def("AddBlockModifier",&ost::mol::mm::Forcefield::AddBlockModifier) 
    .def("SetStandardCTer",&ost::mol::mm::Forcefield::SetStandardCTer)
    .def("SetStandardNTer",&ost::mol::mm::Forcefield::SetStandardNTer)
    .def("SetFudgeLJ",&ost::mol::mm::Forcefield::SetFudgeLJ) 
    .def("SetFudgeQQ",&ost::mol::mm::Forcefield::SetFudgeQQ) 
    .def("SetGenPairs",&ost::mol::mm::Forcefield::SetGenPairs) 
    .def("GetResidueRenamingMain",&ost::mol::mm::Forcefield::GetResidueRenamingMain,(arg("res_name")))
    .def("GetResidueRenamingNTer",&ost::mol::mm::Forcefield::GetResidueRenamingNTer,(arg("res_name")))
    .def("GetResidueRenamingCTer",&ost::mol::mm::Forcefield::GetResidueRenamingCTer,(arg("res_name")))
    .def("GetResidueRenamingTwoTer",&ost::mol::mm::Forcefield::GetResidueRenamingTwoTer,(arg("res_name")))
    .def("GetAtomRenaming",&ost::mol::mm::Forcefield::GetAtomRenaming,(arg("res_name"),arg("atom_name")))
    .def("AssignFFSpecificNames",&ost::mol::mm::Forcefield::AssignFFSpecificNames,(arg("ent"),arg("reverse")=true))
  ;

  boost::python::register_ptr_to_python<ost::mol::mm::ForcefieldPtr>();
  
}
