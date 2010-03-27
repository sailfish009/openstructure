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

#include <ost/mol/coord_group.hh>
#include <ost/mol/entity_handle.hh>

using namespace ost;
using namespace ost::mol;

namespace {
  geom::Vec3 cg_getitem(const CoordGroupHandle& cg, tuple i)
  {
    return (*cg.GetFrame(extract<int>(i[0])))[extract<int>(i[1])];
  }
  void cg_setitem(CoordGroupHandle& cg, tuple i, const geom::Vec3& v)
  {
    (*cg.GetFrame(extract<int>(i[0])))[extract<int>(i[1])]=v;
  }

  void (CoordGroupHandle::*capture1)()=&CoordGroupHandle::Capture;
  void (CoordGroupHandle::*capture2)(uint)=&CoordGroupHandle::Capture;
}

void export_CoordGroup()
{
  class_<CoordGroupHandle>("CoordGroupHandle",no_init)
    .def("IsValid",&CoordGroupHandle::IsValid)
    .def("GetEntity",&CoordGroupHandle::GetEntity)
    .def("GetAtomCount",&CoordGroupHandle::GetAtomCount)
    .def("GetFrameCount",&CoordGroupHandle::GetFrameCount)
    .def("SetFramePositions",&CoordGroupHandle::SetFramePositions)
    .def("SetAtomPos",&CoordGroupHandle::SetAtomPos)
    .def("GetAtomPos",&CoordGroupHandle::GetAtomPos)
    .def("CopyFrame",&CoordGroupHandle::CopyFrame)
    .def("IsValid", &CoordGroupHandle::IsValid)
    .def("Capture", capture1)
    .def("Capture", capture2)
    .def("__getitem__",cg_getitem)
    .def("__setitem__",cg_setitem)
  ;

  def("CreateCoordGroup",CreateCoordGroup);
}
