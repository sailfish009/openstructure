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

namespace {
  // convenience for python
  void set_mat_amb2(GfxObjBase* b, float c) {b->SetMatAmb(Color(c,c,c,1.0));}
  void set_mat_diff2(GfxObjBase* b, float c) {b->SetMatDiff(Color(c,c,c,1.0));}
  void set_mat_spec2(GfxObjBase* b, float c) {b->SetMatSpec(Color(c,c,c,1.0));}
  void set_mat_emm2(GfxObjBase* b, float c) {b->SetMatEmm(Color(c,c,c,1.0));}
  void set_mat1(GfxObjBase* b, float a, float d, float s, float p)
  {
    set_mat_amb2(b,a);
    set_mat_diff2(b,d);
    set_mat_spec2(b,s);
    b->SetMatShin(p);
    set_mat_emm2(b,0.0);
  }
  void set_mat2(GfxObjBase* b, Color a, Color d, Color s, float p)
  {
    b->SetMatAmb(a);
    b->SetMatDiff(d);
    b->SetMatSpec(s);
    b->SetMatShin(p);
    set_mat_emm2(b,0.0);
  }

  void set_outline(GfxObjBase* b, bool f)
  {
    LOG_INFO("Outline(bool) is deprecated, use SetOutline(bool) instead");
    b->SetOutline(f);
  }
  void set_aalines(GfxObjBase* b, bool f)
  {
    LOG_INFO("AALines(bool) is deprecated, use SetAALines(bool) instead");
    b->SetAALines(f);
  }
}

void export_GfxObj()
{
  class_<GfxObjBase, boost::shared_ptr<GfxObjBase>, bases<GfxNode>, boost::noncopyable>("GfxObjBase",no_init)
    .def("SetMatAmb",&GfxObjBase::SetMatAmb)
    .def("SetMatAmb",set_mat_amb2)
    .def("SetMatDiff",&GfxObjBase::SetMatDiff)
    .def("SetMatDiff",set_mat_diff2)
    .def("SetMatSpec",&GfxObjBase::SetMatSpec)
    .def("SetMatSpec",set_mat_spec2)
    .def("SetMatEmm",&GfxObjBase::SetMatEmm)
    .def("SetMatEmm",set_mat_emm2)
    .def("SetMatShin",&GfxObjBase::SetMatShin)
    .def("SetMat",set_mat1)
    .def("SetMat",set_mat2)
    .def("ContextSwitch", &GfxObjBase::ContextSwitch)
    .def("SetRenderMode", &GfxObjBase::SetRenderMode)
    .def("GetRenderMode", &GfxObjBase::GetRenderMode)
    .def("GetCenter",&GfxObjBase::GetCenter)
    .def("SetLineWidth", &GfxObjBase::SetLineWidth)
    .def("SetPolyMode",&GfxObjBase::SetPolyMode)
    .def("AALines",set_aalines)
    .def("SetAALines",&GfxObjBase::SetAALines)
    .def("SetLineHalo",&GfxObjBase::SetLineHalo)
    .def("Outline",set_outline)
    .def("SetOutline",&GfxObjBase::SetOutline)
    .def("SetOutlineMode",&GfxObjBase::SetOutlineMode)
    .def("SetOutlineWidth",&GfxObjBase::SetOutlineWidth)
    .def("SetOutlineExpandFactor",&GfxObjBase::SetOutlineExpandFactor)
    .def("SetOutlineExpandColor",&GfxObjBase::SetOutlineExpandColor)
    .def("SetOpacity",&GfxObjBase::SetOpacity)
    .def("GetOpacity",&GfxObjBase::GetOpacity)
    .add_property("center", &GfxObjBase::GetCenter)
    COLOR_BY_DEF()
   ;
  //register_ptr_to_python<GfxObjBaseP>();

  class_<GfxObj, boost::shared_ptr<GfxObj>, bases<GfxObjBase>, boost::noncopyable>("GfxObj",no_init)
    .def("GetTF", &GfxObj::GetTF, return_value_policy<copy_const_reference>())
    .def("SetTF", &GfxObj::SetTF)
    .def("FlagRebuild",&GfxObj::FlagRebuild)
    .def("FlagRefresh",&GfxObj::FlagRefresh)
    .def("SetNormalSmoothFactor",&GfxObj::SetNormalSmoothFactor)
    .def("GetNormalSmoothFactor",&GfxObj::GetNormalSmoothFactor)
    .def("SmoothVertices",&GfxObj::SmoothVertices)
    .def("Debug",&GfxObj::Debug)
    .def("GetAALines",&GfxObj::GetAALines)
    .def("GetLineWidth",&GfxObj::GetLineWidth)
    .def("GetLineHalo",&GfxObj::GetLineHalo)
    ;
  //register_ptr_to_python<GfxObjP>();

}
