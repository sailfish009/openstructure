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

/*
 * Author Juergen Haas
 */
#include <boost/python.hpp>

#include <ost/geom/mat4.hh>
#include <ost/mol/alg/svd_superpose.hh>
#include <ost/mol/entity_handle.hh>
#include <ost/mol/iterator.hh>

#include <boost/python/suite/indexing/vector_indexing_suite.hpp>

using namespace boost::python;
using namespace ost;
using namespace ost::mol::alg;

void export_svdSuperPose()
{
  SuperpositionResult (*sup1)(const mol::EntityView&,const mol::EntityView&,bool) = SuperposeSVD;
  SuperpositionResult (*sup2)(const std::vector<geom::Vec3>&,const std::vector<geom::Vec3>& ) = SuperposeSVD;
  class_<SuperpositionResult>("SuperpositionResult", init<>())
    .def_readwrite("ncycles", &SuperpositionResult::ncycles)
    .def_readwrite("rmsd", &SuperpositionResult::rmsd)
    .def_readwrite("transformation",
               &SuperpositionResult::transformation)
    .def_readwrite("view1",
               &SuperpositionResult::entity_view1)
    .def_readwrite("view2",
               &SuperpositionResult::entity_view2)
  ;
  def("SuperposeAtoms", &SuperposeAtoms,(arg("apply_transform")=true));
  def("SuperposeSVD", sup1, (arg("apply_transform")=true));
  def("SuperposeSVD", sup2, (arg("apply_transform")=true));
  def("CalculateRMSD", &CalculateRMSD, (arg("transformation")=geom::Mat4()));
  def("IterativeSuperposition", &IterativeSuperposition, (arg("ncycles")=200,
                                                          arg("dist_thres")=4.0,
                                                          arg("apply_transform")=true));
}

