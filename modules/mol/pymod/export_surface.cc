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


#include <ost/mol/surface_handle.hh>
#include <ost/mol/surface_builder.hh>
#include <ost/mol/entity_handle.hh>
#include <ost/mol/impl/rsurf_impl.hh>
#include <ost/mol/impl/surface_impl.hh>

using namespace ost;
using namespace ost::mol;

namespace {

SurfaceHandle create1()
{
  return CreateSurface();
}

void rsurf_tri_dummy(rsurf::RSurf& rs, float patch_size)
{
  impl::SurfaceImplP impl(new impl::SurfaceImpl());
  rs.Triangulate(impl,patch_size);
}


} // anon ns

void export_Surface()
{
  class_<SurfaceVertex>("SurfaceVertex", init<>())
    .def_readwrite("Position", &SurfaceVertex::position)
    .def_readwrite("Normal", &SurfaceVertex::normal)
    .def_readwrite("Atom", &SurfaceVertex::atom)   
  ;     
  class_<SurfaceTriIDList>("SurfaceTriIDList", init<>())
    .def(vector_indexing_suite<SurfaceTriIDList>())
  ;
  class_<SurfaceVertexList>("SurfaceVertexList", init<>())
    .def(vector_indexing_suite<SurfaceVertexList>())
  ;  
  class_<SurfaceHandle>("SurfaceHandle", no_init)
    .def("Attach",&SurfaceHandle::Attach)
    .def("GetVertexIDList", &SurfaceHandle::GetVertexIDList)
    .def("GetTriIDList", &SurfaceHandle::GetTriIDList)    
    .def("GetVertex", &SurfaceHandle::GetVertex)    
    .def("FindWithin", &SurfaceHandle::FindWithin)    
    .def("Invert",&SurfaceHandle::Invert)
  ;

  def("CreateSurface",create1);

  def("BuildSurface",BuildSurface);

  class_<rsurf::RSurf,rsurf::RSurfP>("RSurf", init<double>())
    .def("AddSphere",&rsurf::RSurf::AddSphere)
    .def("Build",&rsurf::RSurf::Build)
    .def("Triangulate",rsurf_tri_dummy)
    ;
}
