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
#include "surface_builder.hh"

#include "surface_handle.hh"
#include <ost/mol/impl/surface_impl.hh>
#include <ost/mol/impl/rsurf_impl.hh>

#include "entity_view.hh"
#include "iterator.hh"

namespace ost { namespace mol {

SurfaceHandle BuildSurface(const EntityView& ev, Real probe_radius, Real patch_size)
{
  rsurf::RSurf rsurf(probe_radius);

  for(AtomViewIter it=ev.AtomsBegin();it!=ev.AtomsEnd();++it) {
    rsurf.AddSphere((*it).GetPos(),(*it).GetAtomProps().radius,(*it).GetQualifiedName());
  }

  rsurf.Build();

  impl::SurfaceImplP impl(new impl::SurfaceImpl());
  rsurf.Triangulate(impl,patch_size);

  return SurfaceHandle(impl);
}

}}

