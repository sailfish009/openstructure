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

void export_AtomView()
{
  void (AtomView::* apply1)(EntityVisitor&) = &AtomView::Apply;
  void (AtomView::* apply2)(EntityViewVisitor&) = &AtomView::Apply;

  class_<AtomView, bases<AtomBase> >("AtomView", init<>())
    .def("GetResidue",&AtomView::GetResidue)
    .add_property("residue",&AtomView::GetResidue)
    .def("Apply", apply1, args("visitor"))
    .def("Apply", apply2, args("visitor"))
    .def(self==self)
    .def(self!=self)
    .add_property("handle", &AtomView::GetHandle)
    .def("RemoveBonds", &AtomView::RemoveBonds)
    .def("GetHandle", &AtomView::GetHandle)
    .def("GetBondCount", &AtomView::GetBondCount)
    .def("GetBondList", &AtomView::GetBondList)
  ;
  class_<AtomViewList>("AtomViewList", no_init)
    .def(vector_indexing_suite<AtomViewList>())
  ;
}

