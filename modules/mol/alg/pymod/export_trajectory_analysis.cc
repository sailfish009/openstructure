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

#include <ost/mol/alg/trajectory_analysis.hh>

using namespace ost;
using namespace ost::mol::alg;

//"thin wrappers" for default parameters
BOOST_PYTHON_FUNCTION_OVERLOADS(extractapos, ExtractAtomPosition, 2, 3)
BOOST_PYTHON_FUNCTION_OVERLOADS(extractcmpos, ExtractCMPosition, 2, 3)
BOOST_PYTHON_FUNCTION_OVERLOADS(extractdist, ExtractDistance, 3, 4)
BOOST_PYTHON_FUNCTION_OVERLOADS(extractang, ExtractAngle, 4, 5)
BOOST_PYTHON_FUNCTION_OVERLOADS(extractdih, ExtractDihedral, 5, 6)
BOOST_PYTHON_FUNCTION_OVERLOADS(extractcmdist, ExtractCMDistance, 3, 4)
BOOST_PYTHON_FUNCTION_OVERLOADS(extractrmsd, ExtractRMSD, 3, 4)

void export_TrajectoryAnalysis()
{
  def("ExtractAtomPosition",ExtractAtomPosition,extractapos());
  def("ExtractCMPosition",&ExtractCMPosition,extractcmpos());
  def("ExtractDistance",&ExtractDistance,extractdist());
  def("ExtractAngle",&ExtractAngle,extractang());
  def("ExtractDihedral",&ExtractDihedral,extractdih());
  def("ExtractCMDistance",&ExtractCMDistance,extractcmdist());
  def("ExtractRMSD",&ExtractRMSD,extractrmsd());
}
