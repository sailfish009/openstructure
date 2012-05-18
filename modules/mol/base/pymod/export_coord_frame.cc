//------------------------------------------------------------------------------
// This file is part of the OpenStructure project <www.openstructure.org>
//
// Copyright (C) 2008-2011 by the OpenStructure authors
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

#include <ost/mol/coord_frame.hh>
#include <ost/geom/vec3.hh>
#include <ost/mol/entity_handle.hh>

using namespace ost;
using namespace ost::mol;

geom::Vec3 (CoordFrame::*GetCellSize)() = &CoordFrame::GetCellSize;
geom::Vec3 (CoordFrame::*GetCellAngles)() = &CoordFrame::GetCellAngles;
geom::Vec3 (CoordFrame::*get_atom_pos)(const AtomHandle& atom) = &CoordFrame::GetAtomPos;
Real (CoordFrame::*get_dist_atom)(const AtomHandle& a1, const AtomHandle& a2) = &CoordFrame::GetDistanceBetwAtoms;
Real (CoordFrame::*get_angle)(const AtomHandle& a1, const AtomHandle& a2, const AtomHandle& a3) = &CoordFrame::GetAngle;
Real (CoordFrame::*get_dihedral)(const AtomHandle& a1, const AtomHandle& a2, const AtomHandle& a3, const AtomHandle& a4) = &CoordFrame::GetDihedralAngle;
geom::Vec3 (CoordFrame::*get_cm)(const mol::EntityView& sele) = &CoordFrame::GetCenterOfMassPos;
Real (CoordFrame::*get_dist_cm)(const mol::EntityView& sele1, const mol::EntityView& sele2) = &CoordFrame::GetDistanceBetwCenterOfMass;
Real (CoordFrame::*get_min_dist_cm_v)(const mol::EntityView& sele1, const mol::EntityView& sele2) = &CoordFrame::GetMinDistBetwCenterOfMassAndView;
Real (CoordFrame::*get_rmsd)(const mol::EntityView& Reference_View, const mol::EntityView& sele_View) = &CoordFrame::GetRMSD;
Real (CoordFrame::*get_min_dist)(const mol::EntityView& view1, const mol::EntityView& view2) = &CoordFrame::GetMinDistance;
Real (CoordFrame::*get_alpha)(const mol::EntityView& segment) = &CoordFrame::GetAlphaHelixContent;
geom::Line3 (CoordFrame::*get_odr_line)(const mol::EntityView& view1) = &CoordFrame::GetODRLine;
geom::Line3 (CoordFrame::*get_odr_line2)() = &geom::Vec3List::GetODRLine;
geom::Plane (CoordFrame::*get_odr_plane)(const mol::EntityView& view1) = &CoordFrame::GetODRPlane;


void export_CoordFrame()
{
  class_<CoordFrame>("CoordFrame",no_init)
    .def("GetCellSize",GetCellSize)
    .def("GetCellAngles",GetCellAngles)
    .def("GetAtomPos", get_atom_pos)
    .def("GetDistanceBetwAtoms", get_dist_atom)
    .def("GetAngle", get_angle)
    .def("GetDihedralAngle", get_dihedral)
    .def("GetCenterOfMassPos", get_cm)
    .def("GetDistanceBetwCenterOfMass", get_dist_cm)
    .def("GetMinDistBetwCenterOfMassAndView", get_min_dist_cm_v)
    .def("GetRMSD",get_rmsd)
    .def("GetMinDistance",get_min_dist)
    .def("GetODRPlane",get_odr_plane)
    .def("GetODRLine",get_odr_line)
    .def("GetODRLine",get_odr_line2)
    .def("GetAlphaHelixContent",get_alpha)
  ;
  def("CreateCoordFrame",CreateCoordFrame);
}
