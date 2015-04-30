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
#include <ost/mol/mm/block_modifiers.hh>
#include <ost/mol/mm/gromacs_block_modifiers.hh>
#include <ost/mol/mm/heuristic_block_modifiers.hh>

using namespace boost::python;

namespace{

template<typename T>
std::vector<T> ListToVec(boost::python::list& l){
  std::vector<T> vec;
  for (int i = 0; i < boost::python::len(l); ++i){
    vec.push_back(boost::python::extract<T>(l[i]));
  }
  return vec;
}


void WrapAddHydrogenRule(ost::mol::mm::GromacsHydrogenConstructorPtr p, int number, int method, 
                         boost::python::list hydrogen_names, boost::python::list anchors){
  std::vector<String> v_hydrogen_names = ListToVec<String>(hydrogen_names);
  std::vector<String> v_anchors = ListToVec<String>(anchors);
  p->AddHydrogenRule(number,method,v_hydrogen_names,v_anchors);
}


}


void export_BlockModifiers()
{

  class_<ost::mol::mm::HydrogenConstructor, boost::noncopyable>("HydrogenConstructor",no_init);
  boost::python::register_ptr_to_python<ost::mol::mm::HydrogenConstructorPtr>();

  class_<ost::mol::mm::BlockModifier, boost::noncopyable>("BlockModifier",no_init);
  boost::python::register_ptr_to_python<ost::mol::mm::BlockModifierPtr>();

  class_<ost::mol::mm::GromacsHydrogenConstructor, bases<ost::mol::mm::HydrogenConstructor> >("GromacsHydrogenConstructor", init<>())
    .def("ApplyOnBuildingBlock",&ost::mol::mm::GromacsHydrogenConstructor::ApplyOnBuildingBlock)
    .def("ApplyOnResidue",&ost::mol::mm::GromacsHydrogenConstructor::ApplyOnResidue)
    .def("AddHydrogenRule",&WrapAddHydrogenRule)
  ;

  class_<ost::mol::mm::GromacsBlockModifier, bases<ost::mol::mm::BlockModifier> >("GromacsBlockModifier", init<>())
    .def("ApplyOnBuildingBlock",&ost::mol::mm::GromacsBlockModifier::ApplyOnBuildingBlock)
    .def("ApplyOnResidue",&ost::mol::mm::GromacsBlockModifier::ApplyOnResidue)
    .def("AddReplaceRule",&ost::mol::mm::GromacsBlockModifier::AddReplaceRule)
    .def("AddAddRule",&ost::mol::mm::GromacsBlockModifier::AddAddRule)
    .def("AddBond",&ost::mol::mm::GromacsBlockModifier::AddBond)
    .def("AddAngle",&ost::mol::mm::GromacsBlockModifier::AddAngle)
    .def("AddDihedral",&ost::mol::mm::GromacsBlockModifier::AddDihedral)
    .def("AddImproper",&ost::mol::mm::GromacsBlockModifier::AddImproper)
    .def("AddCMap",&ost::mol::mm::GromacsBlockModifier::AddDeleteAtom)
    .def("AddDeleteAtom",&ost::mol::mm::GromacsBlockModifier::AddDeleteAtom)
  ;
 
  class_<ost::mol::mm::HeuristicHydrogenConstructor, bases<ost::mol::mm::HydrogenConstructor> >("HeuristicHydrogenConstructor", init<ost::mol::mm::BuildingBlockPtr>())
  .def("ApplyOnBuildingBlock",&ost::mol::mm::HeuristicHydrogenConstructor::ApplyOnBuildingBlock)
  .def("ApplyOnResidue",&ost::mol::mm::HeuristicHydrogenConstructor::ApplyOnResidue)
  ;

  boost::python::register_ptr_to_python<ost::mol::mm::GromacsHydrogenConstructorPtr>();
  boost::python::register_ptr_to_python<ost::mol::mm::GromacsBlockModifierPtr>();
  boost::python::register_ptr_to_python<ost::mol::mm::HeuristicHydrogenConstructorPtr>();
  
}

