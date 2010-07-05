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
#include <ost/gfx/scene.hh>
using namespace ost;
using namespace ost::gfx;

namespace {

BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS(scene_add_overloads, 
                                       Scene::Add, 1, 2)
BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS(scene_autoslab_overloads, 
                                       Scene::Autoslab, 0, 2)
BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS(scene_export_pov_overloads,
                                       Scene::ExportPov, 1,2)
void (Scene::*apply)(const InputEvent&, bool)=&Scene::Apply;
namespace {

Scene* get_scene()
{
  return &Scene::Instance();
}
std::pair<GfxObjP, mol::AtomHandle> pick_atom(Scene* scene, int x, int y)
{
  return get_scene()->PickAtom(x, scene->GetViewport().height-y);
}

GfxObjP scene_getitem(Scene* scene, const String& item)
{
  return scene->operator[](item);
}

}

}

void export_Scene()
{
  def("Scene",get_scene,return_value_policy<reference_existing_object>());

  def("set_offscreen_mode",&Scene::SetOffscreenMode);
  
  // will be removed...
  def("PickAtom", &pick_atom);

  void (Scene::* set_light_prop1)(const Color&,const Color&,const Color&) = &Scene::SetLightProp;
  void (Scene::* set_light_prop2)(float,float,float) = &Scene::SetLightProp;

  void (Scene::* export1)(const String&, uint, uint, bool) = &Scene::Export;
  void (Scene::* export2)(const String&, bool) = &Scene::Export;
  void (Scene::*remove1)(const GfxNodeP&) = &Scene::Remove;
  void (Scene::*remove2)(const String&) = &Scene::Remove;
  void (Scene::*center_on1)(const String&) = &Scene::CenterOn;
  void (Scene::*center_on2)(const GfxObjP&) = &Scene::CenterOn;

  class_<Scene, boost::noncopyable>("SceneSingleton",no_init)
    .def("Add", &Scene::Add, 
         scene_add_overloads())
    .def("Autoslab", &Scene::Autoslab, 
         scene_autoslab_overloads())
    .def("AutoAutoslab",&Scene::AutoAutoslab)
    .def("AutoslabMax",&Scene::AutoslabMax)
    .def("Remove", remove1)
    .def("Remove", remove2)
    .def("RequestRedraw", &Scene::RequestRedraw)
    .def("SetCenter",&Scene::SetCenter)
    .def("GetCenter",&Scene::GetCenter)
    .def("CenterOn",center_on1)
    .def("CenterOn",center_on2)
    .def("UnProject",  &Scene::UnProject, arg("ignore_vp")=false)
    .def("InitGL", &Scene::InitGL)
    .def("RenderGL", &Scene::RenderGL)
    .def("Resize", &Scene::Resize)
    .def("SetBackground", &Scene::SetBackground)
    .add_property("transform", &Scene::GetTransform, &Scene::SetTransform)
    .add_property("fov", &Scene::GetFOV, &Scene::SetFOV)
    .add_property("center", &Scene::GetCenter, &Scene::SetCenter)
    .add_property("near", &Scene::GetNear, &Scene::SetNear)
    .add_property("far", &Scene::GetFar, &Scene::SetFar)
    .def("SetNearFar",&Scene::SetNearFar)
    .def("SetFog",&Scene::SetFog)
    .def("SetFogColor",&Scene::SetFogColor)
    .add_property("fog_near_offset", &Scene::GetFogNearOffset, 
                  &Scene::SetFogNearOffset)
    .add_property("fog_far_offset", &Scene::GetFogFarOffset, 
                  &Scene::SetFogFarOffset)
    .def("SetFOV",&Scene::SetFOV)
    .def("GetFOV",&Scene::GetFOV)
    .def("SetFogOffsets",&Scene::SetFogOffsets)
    .def("Stereo",&Scene::Stereo)
    .def("SetStereoInverted",&Scene::SetStereoInverted)
    .def("Apply", apply)
    .def("SetStereoEye",&Scene::SetStereoEye)
    .def("SetLightDir",&Scene::SetLightDir)
    .def("SetLightProp",set_light_prop1)
    .def("SetLightProp",set_light_prop2)
    .def("Export",export1, arg("transparent")=true)
    .def("Export",export2, arg("transparent")=true)
    .def("ExportPov",&Scene::ExportPov,
         scene_export_pov_overloads())
    .def("GetRTC",&Scene::GetRTC)
    .def("SetRTC",&Scene::SetRTC)
    .def("GetTransform",&Scene::GetTransform)
    .def("SetTransform",&Scene::SetTransform)
    .def("PushView",&Scene::PushView)
    .def("PopView",&Scene::PopView)
    .def("SetSelectionMode",&Scene::SetSelectionMode)
    .def("GetSelectionMode",&Scene::GetSelectionMode)
    .def("SetBlur",&Scene::SetBlur)
    .def("BlurSnapshot",&Scene::BlurSnapshot)
    .def("RemoveAll", &Scene::RemoveAll)
    .def("SetShadow",&Scene::SetShadow)
    .def("SetShadowQuality",&Scene::SetShadowQuality)
    .def("AttachObserver",&Scene::AttachObserver)
    .add_property("selection_mode", &Scene::GetSelectionMode,
                   &Scene::SetSelectionMode)
    .def("__getitem__",scene_getitem)
  ;
}
