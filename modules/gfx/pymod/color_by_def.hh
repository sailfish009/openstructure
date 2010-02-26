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
#ifndef OST_GFX_PYMOD_COLOR_BY_DEF_HH
#define OST_GFX_PYMOD_COLOR_BY_DEF_HH

#include <ost/config.hh>
#include <ost/gfx/gfx_object.hh>

using namespace ost;
using namespace ost::gfx;

namespace ost_gfx {

#if OST_IPLT_ENABLED
inline void color_by_01(GfxObj* go,
                const mol::EntityView& ev, 
                const String& prop,
                const Gradient& g, float minv, float maxv)
{
  go->ColorBy(ev,prop,g,minv,maxv);
}

inline void color_by_02(GfxObj* go,
                const ::iplt::MapHandle& mh, 
                const String& prop,
                const Gradient& g, float minv, float maxv)
{
  go->ColorBy(mh,prop,g,minv,maxv);
}


inline void color_by_11(GfxObj* go,
                 const ::iplt::MapHandle& mh, 
                 const String& prop,
                 const Gradient& g)
{
  go->ColorBy(mh,prop,g);
}

inline void color_by_12(GfxObj* go,
                 const ::iplt::MapHandle& mh, 
                 const String& prop,
                 const Color& c1, const Color& c2)
{
  go->ColorBy(mh,prop,c1,c2);
}


inline void color_by_04(GfxObj* go,
                const ::iplt::MapHandle& mh, 
                const String& prop,
                const Color& c1, const Color& c2, float minv, float maxv)
{
  go->ColorBy(mh,prop,c1,c2,minv,maxv);
}

#endif

inline void color_by_03(GfxObj* go,
                const mol::EntityView& ev, 
                const String& prop,
                const Color& c1, const Color& c2, float minv, float maxv)
{
  go->ColorBy(ev,prop,c1,c2,minv,maxv);
}



inline void color_by_05(GfxObj* go,
                const mol::EntityHandle& eh, 
                const String& prop,
                const Gradient& g, float minv, float maxv)
{
  go->ColorBy(eh,prop,g,minv,maxv);
}

inline void color_by_06(GfxObj* go,
                const mol::EntityHandle& eh, 
                const String& prop,
                const Color& c1, const Color& c2, float minv, float maxv)
{
  go->ColorBy(eh,prop,c1,c2,minv,maxv);
}

inline void color_by_07(GfxObj* go,
                const mol::EntityView& ev, 
                const String& prop,
                const Gradient& g){
  go->ColorBy(ev,prop,g);
}

inline void color_by_08(GfxObj* go,
                const mol::EntityView& ev, 
                const String& prop,
                const Color& c1, const Color& c2)
{
  go->ColorBy(ev,prop,c1,c2);
}

inline void color_by_09(GfxObj* go,
                const mol::EntityHandle& ev, 
                const String& prop,
                const Gradient& g)
{
  go->ColorBy(ev,prop,g);
}

inline void color_by_10(GfxObj* go,
                 const mol::EntityHandle& ev, 
                 const String& prop,
                 const Color& c1, const Color& c2)
{
  go->ColorBy(ev,prop,c1,c2);
}

}

#define COLOR_BY_DEF_STD() \
    .def("ColorBy",ost_gfx::color_by_03)\
    .def("ColorBy",ost_gfx::color_by_05)\
    .def("ColorBy",ost_gfx::color_by_06)\
    .def("ColorBy",ost_gfx::color_by_07)\
    .def("ColorBy",ost_gfx::color_by_08)\
    .def("ColorBy",ost_gfx::color_by_09)\
    .def("ColorBy",ost_gfx::color_by_10)

#if OST_IPLT_ENABLED
#  define COLOR_BY_DEF_MAP() \
    .def("ColorBy",ost_gfx::color_by_01)\
    .def("ColorBy",ost_gfx::color_by_02)\
    .def("ColorBy",ost_gfx::color_by_04)\
    .def("ColorBy",ost_gfx::color_by_11)\
    .def("ColorBy",ost_gfx::color_by_12)
#else
#  define COLOR_BY_DEF_MAP()
#endif

#define COLOR_BY_DEF() \
  COLOR_BY_DEF_STD() \
  COLOR_BY_DEF_MAP()
#endif

