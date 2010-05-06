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
using namespace boost::python;

#include <ost/gfx/gfx_object.hh>
using namespace ost;
using namespace ost::gfx;

#include "color_by_def.hh"

void export_GfxObj()
{
  void (GfxObj::* set_mat_amb1)(const Color&) = &GfxObj::SetMatAmb;
  void (GfxObj::* set_mat_amb2)(float) = &GfxObj::SetMatAmb;
  void (GfxObj::* set_mat_diff1)(const Color&) = &GfxObj::SetMatDiff;
  void (GfxObj::* set_mat_diff2)(float) = &GfxObj::SetMatDiff;
  void (GfxObj::* set_mat_spec1)(const Color&) = &GfxObj::SetMatSpec;
  void (GfxObj::* set_mat_spec2)(float) = &GfxObj::SetMatSpec;
  void (GfxObj::* set_mat_emm1)(const Color&) = &GfxObj::SetMatEmm;
  void (GfxObj::* set_mat_emm2)(float) = &GfxObj::SetMatEmm;
  void (GfxObj::* set_mat1)(const Color&,const Color&, 
                            const Color&, float) = &GfxObj::SetMat;
  void (GfxObj::* set_mat2)(float,float,float,float) = &GfxObj::SetMat;

  class_<GfxObj, bases<GfxNode>, boost::noncopyable>("GfxObj",no_init)
    .def("GetCenter",&GfxObj::GetCenter)
    .def("SetRenderMode", &GfxObj::SetRenderMode)
    .def("GetRenderMode", &GfxObj::GetRenderMode)
    .def("SetLineWidth", &GfxObj::SetLineWidth)
    .def("GetLineWidth", &GfxObj::GetLineWidth)
    .def("SetLineHalo",&GfxObj::SetLineHalo)
    .def("GetLineHalo",&GfxObj::GetLineHalo)
    .def("SetSphereDetail",&GfxObj::SetSphereDetail)
    .def("SetArcDetail",&GfxObj::SetArcDetail)
    .def("SetSplineDetail",&GfxObj::SetSplineDetail)
    .def("SetMatAmb",set_mat_amb1)
    .def("SetMatAmb",set_mat_amb2)
    .def("SetMatDiff",set_mat_diff1)
    .def("SetMatDiff",set_mat_diff2)
    .def("GetTF", &GfxObj::GetTF, return_value_policy<copy_const_reference>())
    .def("SetTF", &GfxObj::SetTF)
    .def("SetMatSpec",set_mat_spec1)
    .def("SetMatSpec",set_mat_spec2)
    .def("SetMatEmm",set_mat_emm1)
    .def("SetMatEmm",set_mat_emm2)
    .def("SetMatShin",&GfxObj::SetMatShin)
    .def("SetMat",set_mat1)
    .def("SetMat",set_mat2)
    .def("SetPolyMode",&GfxObj::SetPolyMode)
    .def("SetAALines",&GfxObj::SetAALines)
    .def("SetOpacity",&GfxObj::SetOpacity)
    .def("GetOpacity",&GfxObj::GetOpacity)
    .def("FlagRebuild",&GfxObj::FlagRebuild)
    .def("FlagRefresh",&GfxObj::FlagRefresh)
    .def("SetNormalSmoothFactor",&GfxObj::SetNormalSmoothFactor)
    .def("GetNormalSmoothFactor",&GfxObj::GetNormalSmoothFactor)
    .def("SetOutlineMode",&GfxObj::SetOutlineMode)
    .def("SetOutlineExpandFactor",&GfxObj::SetOutlineExpandFactor)
    .def("SetOutlineExpandColor",&GfxObj::SetOutlineExpandColor)
    .def("SmoothVertices",&GfxObj::SmoothVertices)
    .def("AmbientOcclusion",&GfxObj::AmbientOcclusion)
    .def("Debug",&GfxObj::Debug)
    .add_property("center", &GfxObj::GetCenter)
    COLOR_BY_DEF()
    ;
  register_ptr_to_python<GfxObjP>();

}
