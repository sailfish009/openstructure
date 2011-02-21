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

} // anon ns


void export_Scene()
{
  def("Scene",get_scene,return_value_policy<reference_existing_object>());

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
  
  class_<Viewport>("Viewport", init<>())
    .def_readwrite("x", &Viewport::x)
    .def_readwrite("y", &Viewport::y)
    .def_readwrite("width", &Viewport::width)
    .def_readwrite("height", &Viewport::height)
  ;
  
  class_<Scene, boost::noncopyable>("SceneSingleton",no_init)
    .def("Add", &Scene::Add, 
         scene_add_overloads())
    .def("Autoslab", &Scene::Autoslab, 
         scene_autoslab_overloads())
    .def("AutoAutoslab",&Scene::AutoAutoslab)
    .def("GetAutoAutoslab",&Scene::GetAutoAutoslab)
    .def("AutoslabMax",&Scene::AutoslabMax)
    .def("Remove", remove1)
    .def("Remove", remove2)
    .add_property("viewport", &Scene::GetViewport)
    .def("RequestRedraw", &Scene::RequestRedraw)
    .def("SetCenter",&Scene::SetCenter)
    .def("GetCenter",&Scene::GetCenter)
    .add_property("center", &Scene::GetCenter, &Scene::SetCenter)
    .def("CenterOn",center_on1)
    .def("CenterOn",center_on2)
    .def("UnProject",  &Scene::UnProject, arg("ignore_vp")=false)
    .def("Project",  &Scene::Project, arg("ignore_vp")=false)
    .def("InitGL", &Scene::InitGL)
    .def("RenderGL", &Scene::RenderGL)
    .def("Resize", &Scene::Resize)
    .def("HasNode", &Scene::HasNode)
    .def("GetBackground", &Scene::GetBackground)
    .def("SetBackground", &Scene::SetBackground)
    .add_property("bg",
                  &Scene::GetBackground, 
                  &Scene::SetBackground)
    .def("SetNear",&Scene::SetNear)
    .def("GetNear",&Scene::GetNear)
    .add_property("near", &Scene::GetNear, &Scene::SetNear)
    .def("SetFar",&Scene::SetFar)
    .def("GetFar",&Scene::GetFar)
    .add_property("far", &Scene::GetFar, &Scene::SetFar)
    .def("SetNearFar",&Scene::SetNearFar)
    .def("SetFog",&Scene::SetFog)
    .def("GetFog",&Scene::GetFog)
    .add_property("fog", &Scene::GetFog, &Scene::SetFog)
    .def("SetFogColor",&Scene::SetFogColor)
    .def("GetFogColor",&Scene::GetFogColor)
    .add_property("fogcol", &Scene::GetFogColor, &Scene::SetFogColor)
    .def("SetFOV",&Scene::SetFOV)
    .def("GetFOV",&Scene::GetFOV)
    .add_property("fov", &Scene::GetFOV, &Scene::SetFOV)
    .def("SetFogOffsets",&Scene::SetFogOffsets)
    .add_property("fogno",
                  &Scene::GetFogNearOffset, 
                  &Scene::SetFogNearOffset)
    .add_property("fogfo",
                  &Scene::GetFogFarOffset, 
                  &Scene::SetFogFarOffset)
    .def("GetRTC",&Scene::GetRTC)
    .def("SetRTC",&Scene::SetRTC)
    .add_property("rtc",&Scene::GetRTC,&Scene::SetRTC)
    .def("GetTransform",&Scene::GetTransform)
    .def("SetTransform",&Scene::SetTransform)
    .add_property("transform", &Scene::GetTransform, &Scene::SetTransform)
    .def("SetSelectionMode",&Scene::SetSelectionMode)
    .def("GetSelectionMode",&Scene::GetSelectionMode)
    .add_property("smode",
                 &Scene::GetSelectionMode,
                 &Scene::SetSelectionMode)
    .def("SetStereoMode",&Scene::SetStereoMode)
    .def("GetStereoMode",&Scene::GetStereoMode)
    .add_property("stereo_mode",&Scene::GetStereoMode,&Scene::SetStereoMode)
    .def("SetStereoFlip",&Scene::SetStereoFlip)
    .def("GetStereoFlip",&Scene::GetStereoFlip)
    .add_property("stereo_flip",&Scene::GetStereoFlip,&Scene::SetStereoFlip)
    .def("SetStereoView",&Scene::SetStereoView)
    .def("GetStereoView",&Scene::GetStereoView)
    .add_property("stereo_view",&Scene::GetStereoView,&Scene::SetStereoView)
    .def("SetStereoIOD",&Scene::SetStereoIOD)
    .def("GetStereoIOD",&Scene::GetStereoIOD)
    .add_property("stereo_iod",&Scene::GetStereoIOD,&Scene::SetStereoIOD)
    .def("SetStereoDistance",&Scene::SetStereoDistance)
    .def("GetStereoDistance",&Scene::GetStereoDistance)
    .add_property("stereo_distance",&Scene::GetStereoDistance,&Scene::SetStereoDistance)
    .def("SetStereoAlg",&Scene::SetStereoAlg)
    .def("GetStereoAlg",&Scene::GetStereoAlg)
    .add_property("stereo_alg",&Scene::GetStereoAlg,&Scene::SetStereoAlg)
    .def("SetLightDir",&Scene::SetLightDir)
    .def("SetLightProp",set_light_prop1)
    .def("SetLightProp",set_light_prop2)
    .def("Apply", apply)
    .def("Export",export1, arg("transparent")=true)
    .def("Export",export2, arg("transparent")=true)
    .def("ExportPov",&Scene::ExportPov,
         scene_export_pov_overloads())
    .def("PushView",&Scene::PushView)
    .def("PopView",&Scene::PopView)
    .def("SetBlur",&Scene::SetBlur)
    .def("BlurSnapshot",&Scene::BlurSnapshot)
    .def("RemoveAll", &Scene::RemoveAll)
    .def("SetShadow",&Scene::SetShadow)
    .def("SetShadowQuality",&Scene::SetShadowQuality)
    .def("SetShadowWeight",&Scene::SetShadowWeight)
    .def("SetDepthDarkening",&Scene::SetDepthDarkening)
    .def("SetDepthDarkeningWeight",&Scene::SetDepthDarkeningWeight)
    .def("SetAmbientOcclusion",&Scene::SetAmbientOcclusion)
    .def("SetAmbientOcclusionWeight",&Scene::SetAmbientOcclusionWeight)
    .def("SetAmbientOcclusionMode",&Scene::SetAmbientOcclusionMode)
    .def("SetAmbientOcclusionQuality",&Scene::SetAmbientOcclusionQuality)
    .def("AttachObserver",&Scene::AttachObserver)
    .def("StartOffscreenMode",&Scene::StartOffscreenMode)
    .def("StopOffscreenMode",&Scene::StopOffscreenMode)
    .def("SetShadingMode",&Scene::SetShadingMode)
    .def("SetBeacon",&Scene::SetBeacon)
    .add_property("root_node", &Scene::GetRootNode)
    .def("SetBeaconOff",&Scene::SetBeaconOff)
    .def("__getitem__",scene_getitem)
  ;
}
