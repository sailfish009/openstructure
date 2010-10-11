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

#include <ost/gfx/render_options/render_options.hh>
#include <ost/gfx/render_options/cpk_render_options.hh>
#include <ost/gfx/render_options/custom_render_options.hh>
#include <ost/gfx/render_options/cartoon_render_options.hh>
#include <ost/gfx/render_options/line_render_options.hh>
#include <ost/gfx/render_options/simple_render_options.hh>
#include <ost/gfx/render_options/line_trace_render_options.hh>
#include <ost/gfx/render_options/sline_render_options.hh>
#include <ost/gfx/render_options/trace_render_options.hh>
using namespace boost::python;

using namespace ost;
using namespace ost::gfx;

void export_RenderOptions()
{
  class_<RenderOptions, boost::shared_ptr<RenderOptions>, boost::noncopyable>("RenderOptions", no_init)
    .def("GetRenderMode",&RenderOptions::GetRenderMode)
    .def("NotifyStateChange",&RenderOptions::NotifyStateChange)
    .def("AddObserver",&RenderOptions::AddObserver)
    .def("RemoveObserver",&RenderOptions::RemoveObserver)
  ;

  class_<LineRenderOptions, boost::shared_ptr<LineRenderOptions>, bases<RenderOptions>, boost::noncopyable>("LineRenderOptions", no_init)
    .def("SetLineWidth",&LineRenderOptions::SetLineWidth)
    .def("GetLineWidth",&LineRenderOptions::GetLineWidth)
    .def("SetAALines",&LineRenderOptions::SetAALines)
    .def("GetAALines",&LineRenderOptions::GetAALines)
    .add_property("aa_lines", &LineRenderOptions::GetAALines, 
                  &LineRenderOptions::SetAALines)
    .add_property("line_width", &LineRenderOptions::GetLineWidth, 
                  &LineRenderOptions::SetLineWidth)    
  ;

  class_<SimpleRenderOptions, boost::shared_ptr<SimpleRenderOptions>, bases<LineRenderOptions>, boost::noncopyable>("SimpleRenderOptions")
    .def(init<const SimpleRenderOptions&>())
    .def("SetBondOrderFlag", &SimpleRenderOptions::SetBondOrderFlag)
    .def("SetBondOrderDistance", &SimpleRenderOptions::SetBondOrderDistance)
    .def("GetBondOrderFlag", &SimpleRenderOptions::GetBondOrderFlag)
    .def("GetBondOrderDistance", &SimpleRenderOptions::GetBondOrderDistance)
    .add_property("draw_bond_order", &SimpleRenderOptions::GetBondOrderFlag, 
                  &SimpleRenderOptions::SetBondOrderFlag)
    .add_property("bond_order_distance", &SimpleRenderOptions::GetBondOrderDistance, 
                  &SimpleRenderOptions::SetBondOrderDistance)    
  ;

  class_<LineTraceRenderOptions, boost::shared_ptr<LineTraceRenderOptions>, bases<LineRenderOptions>, boost::noncopyable>("LineTraceRenderOptions")
    .def(init<const LineTraceRenderOptions&>())
  ;

  class_<SlineRenderOptions, boost::shared_ptr<SlineRenderOptions>, bases<LineRenderOptions>, boost::noncopyable>("SlineRenderOptions")
    .def(init<const SlineRenderOptions&>())
    .def("SetSplineDetail", &SlineRenderOptions::SetSplineDetail)
    .def("GetSplineDetail", &SlineRenderOptions::GetSplineDetail)
  ;

  class_<CPKRenderOptions, boost::shared_ptr<CPKRenderOptions>, bases<RenderOptions>, boost::noncopyable>("CPKRenderOptions")
    .def(init<const CPKRenderOptions&>())
    .def("SetSphereDetail", &CPKRenderOptions::SetSphereDetail)
    .def("GetSphereDetail", &CPKRenderOptions::GetSphereDetail)
    .def("SetSphereMode", &CPKRenderOptions::SetSphereMode)
    .def("GetSphereMode", &CPKRenderOptions::GetSphereMode)
    .def("SetCPKMode", &CPKRenderOptions::SetCPKMode)
    .def("GetCPKMode", &CPKRenderOptions::GetCPKMode)
    .add_property("cpk_mode", &CPKRenderOptions::GetCPKMode, 
                  &CPKRenderOptions::SetCPKMode)
    .add_property("sphere_detail", &CPKRenderOptions::GetSphereDetail, 
                  &CPKRenderOptions::SetSphereDetail)
  ;

  class_<CustomRenderOptions, boost::shared_ptr<CustomRenderOptions>, bases<RenderOptions>, boost::noncopyable>("CustomRenderOptions")
    .def(init<const CustomRenderOptions&>())
    .def("SetSphereDetail", &CustomRenderOptions::SetSphereDetail)
    .def("GetSphereDetail", &CustomRenderOptions::GetSphereDetail)
    .def("SetArcDetail", &CustomRenderOptions::SetArcDetail)
    .def("GetArcDetail", &CustomRenderOptions::GetArcDetail)
    .def("SetSphereRad", &CustomRenderOptions::SetSphereRad)
    .def("GetSphereRad", &CustomRenderOptions::GetSphereRad)
    .def("SetBondRad", &CustomRenderOptions::SetBondRad)
    .def("GetBondRad", &CustomRenderOptions::GetBondRad)
    .add_property("sphere_detail", &CustomRenderOptions::GetSphereDetail, &CustomRenderOptions::SetSphereDetail)
    .add_property("bond_rad", &CustomRenderOptions::GetBondRad, &CustomRenderOptions::SetBondRad)
    .add_property("sphere_rad", &CustomRenderOptions::GetSphereRad, &CustomRenderOptions::SetSphereRad)    
    .add_property("arc_detail", &CustomRenderOptions::GetArcDetail, 
                  &CustomRenderOptions::SetArcDetail)
  ;

  class_<CartoonRenderOptions, boost::shared_ptr<CartoonRenderOptions>, bases<RenderOptions>, boost::noncopyable>("CartoonRenderOptions")
    .def(init<const CartoonRenderOptions&>())
    .def(init<optional<bool> >())
    .def("SetSplineDetail", &CartoonRenderOptions::SetSplineDetail)
    .def("GetSplineDetail", &CartoonRenderOptions::GetSplineDetail)
    .def("SetPolyMode", &CartoonRenderOptions::SetPolyMode)
    .def("GetPolyMode", &CartoonRenderOptions::GetPolyMode)
    .def("SetArcDetail", &CartoonRenderOptions::SetArcDetail)
    .def("GetArcDetail", &CartoonRenderOptions::GetArcDetail)
    .def("SetNormalSmoothFactor", &CartoonRenderOptions::SetNormalSmoothFactor)
    .def("GetNormalSmoothFactor", &CartoonRenderOptions::GetNormalSmoothFactor)
    .def("SetTubeRadius", &CartoonRenderOptions::SetTubeRadius)
    .def("GetTubeRadius", &CartoonRenderOptions::GetTubeRadius)
    .def("SetTubeRatio", &CartoonRenderOptions::SetTubeRatio)
    .def("GetTubeRatio", &CartoonRenderOptions::GetTubeRatio)
    .def("SetTubeProfileType", &CartoonRenderOptions::SetTubeProfileType)
    .def("GetTubeProfileType", &CartoonRenderOptions::GetTubeProfileType)
    .def("SetHelixWidth", &CartoonRenderOptions::SetHelixWidth)
    .def("GetHelixWidth", &CartoonRenderOptions::GetHelixWidth)
    .def("SetHelixThickness", &CartoonRenderOptions::SetHelixThickness)
    .def("GetHelixThickness", &CartoonRenderOptions::GetHelixThickness)
    .def("SetHelixEcc", &CartoonRenderOptions::SetHelixEcc)
    .def("GetHelixEcc", &CartoonRenderOptions::GetHelixEcc)
    .def("SetHelixProfileType", &CartoonRenderOptions::SetHelixProfileType)
    .def("GetHelixProfileType", &CartoonRenderOptions::GetHelixProfileType)
    .def("SetHelixMode", &CartoonRenderOptions::SetHelixMode)
    .def("GetHelixMode", &CartoonRenderOptions::GetHelixMode)
    .def("SetStrandWidth", &CartoonRenderOptions::SetStrandWidth)
    .def("GetStrandWidth", &CartoonRenderOptions::GetStrandWidth)
    .def("SetStrandThickness", &CartoonRenderOptions::SetStrandThickness)
    .def("GetStrandThickness", &CartoonRenderOptions::GetStrandThickness)
    .def("SetStrandEcc", &CartoonRenderOptions::SetStrandEcc)
    .def("GetStrandEcc", &CartoonRenderOptions::GetStrandEcc)
    .def("SetStrandProfileType", &CartoonRenderOptions::SetStrandProfileType)
    .def("GetStrandProfileType", &CartoonRenderOptions::GetStrandProfileType)
    .def("SetStrandMode", &CartoonRenderOptions::SetStrandMode)
    .def("GetStrandMode", &CartoonRenderOptions::GetStrandMode)
    .def("SetColorBlendMode", &CartoonRenderOptions::SetColorBlendMode)
    .def("GetColorBlendMode", &CartoonRenderOptions::GetColorBlendMode)
  ;
  
  class_<TraceRenderOptions, boost::shared_ptr<TraceRenderOptions>, bases<RenderOptions>, boost::noncopyable>("TraceRenderOptions")
    .def(init<const TraceRenderOptions&>())
    .def("SetArcDetail", &TraceRenderOptions::SetArcDetail)
    .def("GetArcDetail", &TraceRenderOptions::GetArcDetail)
    .def("SetNormalSmoothFactor", &TraceRenderOptions::SetNormalSmoothFactor)
    .def("GetNormalSmoothFactor", &TraceRenderOptions::GetNormalSmoothFactor)
    .def("SetTubeRadius", &TraceRenderOptions::SetTubeRadius)
    .def("GetTubeRadius", &TraceRenderOptions::GetTubeRadius)
    .add_property("tube_radius", &TraceRenderOptions::GetTubeRadius, 
                  &TraceRenderOptions::SetTubeRadius)
    .add_property("arc_detail", &TraceRenderOptions::GetArcDetail, 
                  &TraceRenderOptions::SetArcDetail)
    .add_property("normal_smooth_factor", 
                  &TraceRenderOptions::GetNormalSmoothFactor, 
                  &TraceRenderOptions::SetNormalSmoothFactor)    
  ;
}



