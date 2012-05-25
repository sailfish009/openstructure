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
#ifndef OST_MOL_COORD_FRAME_HH
#define OST_MOL_COORD_FRAME_HH

/*
  Author: Marco Biasini and Niklaus Johner
 */
#include <boost/shared_ptr.hpp>
#include <ost/geom/geom.hh>
#include <ost/mol/module_config.hh>
#include <ost/mol/entity_view.hh>
#include "atom_handle.hh"

namespace ost { namespace mol {

class DLLEXPORT_OST_MOL CoordFrame : public geom::Vec3List{
private:
  geom::Vec3 periodic_cell_sizes_;
  geom::Vec3 periodic_cell_angles_;
public:
  typedef geom::Vec3List base_type;
  
  CoordFrame() : base_type() {}
  CoordFrame(size_t size, const geom::Vec3& value=geom::Vec3()) : base_type(size, value) {}
  CoordFrame(base_type::iterator b, base_type::iterator e): base_type(b, e) { }
  CoordFrame(const base_type& rhs) : base_type(rhs) { }
  CoordFrame(const std::vector<geom::Vec3>& rhs) : base_type(rhs) { }
  CoordFrame(const std::vector<geom::Vec3>& rhs,const geom::Vec3 box_size, const geom::Vec3 box_angles) : base_type(rhs) {
    this->periodic_cell_sizes_=box_size;
    this->periodic_cell_angles_=box_angles;
  }
  
  geom::Vec3 GetCellSize() {
    return this->periodic_cell_sizes_;
  }
  geom::Vec3 GetCellAngles() {
    return this->periodic_cell_angles_;
  }
  geom::Vec3 GetAtomPos(const AtomHandle& atom);
  geom::Vec3 GetAtomPos(int atom_index);
  Real GetDistanceBetwAtoms(const AtomHandle& a1, const AtomHandle& a2);
  Real GetDistanceBetwAtoms(int atom1_index, int atom2_index);
  Real GetAngle(const AtomHandle& a1, const AtomHandle& a2, const AtomHandle& a3);
  Real GetAngle(int atom1_index, int atom2_index, int atom3_index);  
  Real GetDihedralAngle(const AtomHandle& a1, const AtomHandle& a2, const AtomHandle& a3, const AtomHandle& a4);
  Real GetDihedralAngle(int a1_index, int a2_index, int a3_index, int a4_index); 
  geom::Vec3 GetCenterOfMassPos(const mol::EntityView& sele);
  geom::Vec3 GetCenterOfMassPos(std::vector<unsigned long>& indices, std::vector<Real>& masses);
  Real GetDistanceBetwCenterOfMass(const mol::EntityView& sele1, const mol::EntityView& sele2);
  Real GetDistanceBetwCenterOfMass(std::vector<unsigned long>& indices1, std::vector<Real>& masses1, 
                                   std::vector<unsigned long>& indices2, std::vector<Real>& masses2);
  Real GetRMSD(const std::vector<geom::Vec3>& ref_pos, const std::vector<unsigned long>& indices_sele);
  Real GetRMSD(const mol::EntityView& reference_view, const mol::EntityView& sele_view);
  Real GetMinDistance(std::vector<unsigned long>& index_list1, std::vector<unsigned long>& index_list2);
  Real GetMinDistance(const mol::EntityView& view1, const mol::EntityView& view2);
  Real GetMinDistBetwCenterOfMassAndView(std::vector<unsigned long>& indices_cm, std::vector<Real>& masses_cm,
                                         std::vector<unsigned long>& indices_atoms);
  Real GetMinDistBetwCenterOfMassAndView(const mol::EntityView& view_cm, const mol::EntityView& view_atoms);
  geom::Line3 GetODRLine(std::vector<unsigned long>& indices_ca);
  geom::Plane GetODRPlane(std::vector<unsigned long>& indices_ca);
  geom::Line3 GetODRLine(const mol::EntityView& view1);
  geom::Plane GetODRPlane(const mol::EntityView& view1);
  geom::Line3 FitCylinder(std::vector<unsigned long>& indices_ca);
  geom::Line3 FitCylinder(const mol::EntityView& view1);
  Real GetAlphaHelixContent(std::vector<unsigned long>& indices_ca, std::vector<unsigned long>& indices_c,
                             std::vector<unsigned long>& indices_o, std::vector<unsigned long>& indices_n);
  Real GetAlphaHelixContent(const mol::EntityView& segment);
};
  
  void GetIndices(const EntityView& sele, std::vector<unsigned long>& indices);
  void GetMasses(const EntityView& sele, std::vector<Real>& masses);
  void GetIndicesAndMasses(const EntityView& sele, std::vector<unsigned long>& indices,std::vector<Real>& masses);
  void GetPositions(const EntityView& sele, std::vector<geom::Vec3>& ref_pos);
  void GetCaIndices(const EntityView& segment, std::vector<unsigned long>& indices_ca);
  void GetCaCONIndices(const EntityView& segment, std::vector<unsigned long>& indices_ca, std::vector<unsigned long>& indices_c,
                      std::vector<unsigned long>& indices_o, std::vector<unsigned long>& indices_n);

typedef boost::shared_ptr<CoordFrame> CoordFramePtr;
typedef std::vector<CoordFramePtr> CoordFrameList;

// factory method
// create a frame froma Vec3List containing the positions of the atoms
  DLLEXPORT_OST_MOL CoordFrame CreateCoordFrame(const geom::Vec3List& atom_pos);

}}

#endif
