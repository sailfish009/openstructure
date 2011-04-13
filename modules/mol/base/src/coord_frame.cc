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

/*
 Author: Niklaus Johner
 */

#include <ost/invalid_handle.hh>
#include <ost/integrity_error.hh>
#include <ost/log.hh>
#include <ost/mol/in_mem_coord_source.hh>
#include <ost/mol/view_op.hh>
#include <ost/mol/mol.hh>
#include "coord_frame.hh"

namespace ost { namespace mol {

  geom::Vec3 CoordFrame::GetAtomPosition(const AtomHandle& atom){
    return this->GetAtomPosition(atom.GetIndex());
  }
  geom::Vec3 CoordFrame::GetAtomPosition(const int& index){
    return (*this)[index];
  }
  
  Real CoordFrame::GetDistance(const AtomHandle& a1, const AtomHandle& a2){
    return this->GetDistance(a1.GetIndex(),a2.GetIndex());
  }
  Real CoordFrame::GetDistance(const int& i1, const int& i2){
    return geom::Distance((*this)[i1],(*this)[i2]);
  }
  
  Real CoordFrame::GetAngle(const AtomHandle& a1, const AtomHandle& a2, const AtomHandle& a3){
    return this->GetAngle(a1.GetIndex(),a2.GetIndex(),a3.GetIndex());  
  }
  Real CoordFrame::GetAngle(const int& i1, const int& i2, const int& i3){
    return geom::Angle((*this)[i1]-(*this)[i2],(*this)[i3]-(*this)[i2]);
  }
  
  Real CoordFrame::GetDihedralAngle(const AtomHandle& a1, const AtomHandle& a2, const AtomHandle& a3, const AtomHandle& a4){
    return this->GetDihedralAngle(a1.GetIndex(),a2.GetIndex(),a3.GetIndex(),a4.GetIndex());
  }
  Real CoordFrame::GetDihedralAngle(const int& i1, const int& i2, const int& i3, const int& i4){
    return geom::DihedralAngle((*this)[i1],(*this)[i2],(*this)[i3],(*this)[i4]);
  }

  void GetIndices(const EntityView& Sele, std::vector<unsigned long>& indices){
    AtomViewList atoms=Sele.GetAtomList();
    indices.reserve(atoms.size());
    for (AtomViewList::const_iterator i=atoms.begin(), 
         e=atoms.end(); i!=e; ++i) {
      indices.push_back(i->GetIndex());
    }
  }
  
  void GetMasses(const EntityView& Sele, std::vector<Real>& masses){
    Real mass_tot=0.0;
    AtomViewList atoms=Sele.GetAtomList();
    masses.reserve(atoms.size());
    for (AtomViewList::const_iterator i=atoms.begin(), 
         e=atoms.end(); i!=e; ++i) {
      masses.push_back(i->GetMass());
      mass_tot=mass_tot+i->GetMass();
    }
    for (std::vector<Real>::iterator 
         j=masses.begin(), e2=masses.end(); j!=e2; ++j) {
      (*j)/=mass_tot;
    }  
  }
  
  
  void GetIndicesAndMasses(const EntityView& Sele, std::vector<unsigned long>& indices,std::vector<Real>& masses){
    GetIndices(Sele, indices);
    GetMasses(Sele, masses);
  }
  
  void GetPositions(const EntityView& Sele, std::vector<geom::Vec3>& ref_pos){
    ref_pos.reserve(Sele.GetAtomCount());
    for (mol::AtomViewIter a=Sele.AtomsBegin(),e=Sele.AtomsEnd(); a!=e; ++a) {
      ref_pos.push_back((*a).GetPos());
    }
  }
  
  geom::Vec3 CoordFrame::GetCMPosition(const EntityView& Sele){
    std::vector<unsigned long> indices;
    std::vector<Real> masses;
    GetIndicesAndMasses(Sele,indices,masses);
    return this->GetCMPosition(indices,masses);
  }
  
  geom::Vec3 CoordFrame::GetCMPosition(std::vector<unsigned long>& indices,std::vector<Real>& masses){
    geom::Vec3 v;
    for (unsigned int i=0,e=indices.size();i!=e; i++) {
      v+=masses[i]*(*this)[indices[i]];
    }
    return v;
  }
  
  Real CoordFrame::GetCMDistance(const EntityView& Sele1,const EntityView& Sele2){
    std::vector<unsigned long> indices1,indices2;
    std::vector<Real> masses1,masses2;
    GetIndicesAndMasses(Sele1,indices1,masses1);
    GetIndicesAndMasses(Sele2,indices2,masses2);
    return this->GetCMDistance(indices1,masses1,indices2,masses2);
  }
  
  Real CoordFrame::GetCMDistance(std::vector<unsigned long>& indices1,std::vector<Real>& masses1,
                           std::vector<unsigned long>& indices2,std::vector<Real>& masses2){
    geom::Vec3 v1=this->GetCMPosition(indices1, masses1);
    geom::Vec3 v2=this->GetCMPosition(indices2, masses2);
    return geom::Distance(v1,v2);
  }
  
  Real CoordFrame::GetRMSD(const std::vector<geom::Vec3>& ref_pos,const std::vector<unsigned long>& indices_sele){
    Real rmsd=0.0,val;
    for (unsigned int i1=0; i1!=indices_sele.size(); ++i1) {
      geom::Vec3 av_sele=(*this)[indices_sele[i1]];
      geom::Vec3 av_ref=ref_pos[i1];
      val=geom::Length2(av_ref-av_sele);
      rmsd+=val;
    }
    return rmsd/indices_sele.size();
  }
  
  Real CoordFrame::GetRMSD(const EntityView& Reference_View,const EntityView& Sele_View){
    int count_ref=Reference_View.GetAtomCount();
    int count_sele=Sele_View.GetAtomCount();
    if (count_ref!=count_sele){
      throw std::runtime_error("atom counts of the two views are not equal");
    }
    std::vector<unsigned long> indices_sele;
    std::vector<geom::Vec3> ref_pos;
    GetIndices(Sele_View,indices_sele);
    GetPositions(Reference_View,ref_pos);
    return this->GetRMSD(ref_pos,indices_sele);
  }
  
}}//ns