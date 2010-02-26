//------------------------------------------------------------------------------
// This file is part of the OpenStructure project <www.openstructure.org>
//
// Copyright (C) 2008-2010 by the OpenStructure authors
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

/*
  Author: Ansgar Philippsen
*/

#include <boost/python.hpp>
using namespace boost::python;

#include <ost/iplt/alg/transcendentals.hh>

using namespace ost::iplt;
using namespace ost::iplt::alg;

void export_Transcendentals()
{
  class_<Cos, bases<ConstModIPAlgorithm> >("Cos", init<>());
  class_<Exp, bases<ConstModIPAlgorithm> >("Exp", init<>());
  class_<Log, bases<ConstModIPAlgorithm> >("Log", init<>());
  class_<Log10, bases<ConstModIPAlgorithm> >("Log10", init<>());
  class_<Sin, bases<ConstModIPAlgorithm> >("Sin", init<>());
  class_<Sqrt, bases<ConstModIPAlgorithm> >("Sqrt", init<>());
  class_<Tan, bases<ConstModIPAlgorithm> >("Tan", init<>());
  class_<Pow, bases<ConstModIPAlgorithm> >("Pow", init<Real>());
}
