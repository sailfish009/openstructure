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

#include <ost/info/info.hh>
#include <ost/gfx/prim_list.hh>
#include <ost/gfx/gradient.hh>
#include <ost/gfx/gfx_test_object.hh>

#include <boost/python/suite/indexing/vector_indexing_suite.hpp>

extern void export_Scene();
extern void export_GfxNode();
extern void export_GfxObj();
extern void export_Entity();
extern void export_Surface();
extern void export_primitives();
#if OST_IMG_ENABLED
  extern void export_Map();
#endif
extern void export_SymmetryNode();
extern void export_SceneObserver();

extern void export_RenderOptions();

extern void export_ColorOps();

using namespace ost;
using namespace ost::gfx;


float color_get_red(Color* c) {
  return c->Red();
}

float color_get_green(Color* c) {
  return c->Green();
}

float color_get_blue(Color* c) {
  return c->Blue();
}

float color_get_alpha(Color* c) {
  return c->Alpha();
}


BOOST_PYTHON_MODULE(_gfx)
{
  export_Scene();
  export_GfxNode();
  export_GfxObj();
  export_Entity();
  export_Surface();
  export_RenderOptions();
#if OST_IMG_ENABLED  
  export_Map();
#endif  
  export_SymmetryNode();
  export_SceneObserver();
  export_ColorOps();

  enum_<RenderMode::Type>("RenderMode")
    .value("SIMPLE",RenderMode::SIMPLE)
    .value("DEBUG",RenderMode::DEBUG)
    .value("DEBUG2",RenderMode::DEBUG2)
    .value("TEST",RenderMode::TEST)
    .value("TEST2",RenderMode::TEST2)
    .value("CUSTOM",RenderMode::CUSTOM)
    .value("CPK",RenderMode::CPK)
    .value("LINE_TRACE",RenderMode::LINE_TRACE)
    .value("TRACE",RenderMode::TRACE)    
    .value("SLINE",RenderMode::SLINE)
    .value("HSC",RenderMode::HSC)
    .value("TUBE",RenderMode::TUBE)
    .value("FILL",RenderMode::FILL)
    .value("OUTLINE",RenderMode::OUTLINE)
    .export_values()
    ;


  class_<PrimList, bases<GfxObj>, PrimListP, boost::noncopyable>("PrimList", init<const String& >())
    .def("Clear",&PrimList::Clear)
    .def("AddLine",&PrimList::AddLine)
    .def("SetColor",&PrimList::SetColor)
    .def("SetDiameter",&PrimList::SetDiameter)
  ;

  class_<GfxTestObj, bases<GfxObj>, boost::noncopyable>("GfxTestObj", init<>());

  
  class_<Color>("Color",init<>())
    .def(init<int>())
    .def(init<float, float, float, optional<float> >())
    .def(self_ns::str(self))
    .def("Red",color_get_red)
    .def("Green",color_get_green)
    .def("Blue",color_get_blue)
    .def("Alpha",color_get_alpha)
    ;
  
  class_<Gradient>("Gradient", init<>())
    .def("SetColorAt", &Gradient::SetColorAt)
    .def("GetColorAt", &Gradient::GetColorAt)
    .def("GetStops", &Gradient::GetStops)
    .def("GradientToInfo", &Gradient::GradientToInfo)
    .def("GradientFromInfo", &Gradient::GradientFromInfo).staticmethod("GradientFromInfo")
  ;

  class_<StopList>("StopList", init<>())
    .def(vector_indexing_suite<StopList>())
  ;

  class_<Stop>("Stop", init<>())
	.def("GetColor", &Stop::GetColor)
	.def("GetRel", &Stop::GetRel)
  ;


  export_primitives();
}



