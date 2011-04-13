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
public:
  typedef geom::Vec3List base_type;
  CoordFrame() : base_type() {}
  
  CoordFrame(size_t size, const geom::Vec3& value=geom::Vec3()) : base_type(size, value) {}
  CoordFrame(base_type::iterator b, base_type::iterator e): base_type(b, e) { }
  CoordFrame(const base_type& rhs) : base_type(rhs) { }
  CoordFrame(const std::vector<geom::Vec3>& rhs) : base_type(rhs) { }
  
  geom::Vec3 GetAtomPosition(const AtomHandle& atom);
  geom::Vec3 GetAtomPosition(const int& atom_index);
  Real GetDistance(const AtomHandle& a1, const AtomHandle& a2);
  Real GetDistance(const int& atom1_index, const int& atom2_index);
  Real GetAngle(const AtomHandle& a1, const AtomHandle& a2, const AtomHandle& a3);
  Real GetAngle(const int& atom1_index, const int& atom2_index, const int& atom3_index);  
  Real GetDihedralAngle(const AtomHandle& a1, const AtomHandle& a2, const AtomHandle& a3, const AtomHandle& a4);
  Real GetDihedralAngle(const int& a1_index, const int& a2_index, const int& a3_index, const int& a4_index); 
  geom::Vec3 GetCMPosition(const mol::EntityView& Sele);
  geom::Vec3 GetCMPosition(std::vector<unsigned long>& indices,std::vector<Real>& masses);
  Real GetCMDistance(const mol::EntityView& Sele1, const mol::EntityView& Sele2);
  Real GetCMDistance(std::vector<unsigned long>& indices1,std::vector<Real>& masses1,std::vector<unsigned long>& indices2,std::vector<Real>& masses2);
  Real GetRMSD(const std::vector<geom::Vec3>& ref_pos,const std::vector<unsigned long>& indices_sele);
  Real GetRMSD(const mol::EntityView& Reference_View, const mol::EntityView& Sele_View);
};
  
  void GetIndices(const EntityView& Sele, std::vector<unsigned long>& indices);
  void GetMasses(const EntityView& Sele,std::vector<Real>& masses);
  void GetIndicesAndMasses(const EntityView& Sele, std::vector<unsigned long>& indices,std::vector<Real>& masses);
  void GetPositions(const EntityView& Sele, std::vector<geom::Vec3>& ref_pos);
  
typedef boost::shared_ptr<CoordFrame> CoordFramePtr;
typedef std::vector<CoordFramePtr> CoordFrameList;


}}

#endif
