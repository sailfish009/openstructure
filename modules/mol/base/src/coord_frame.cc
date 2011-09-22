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
#include <ost/base.hh>

namespace ost { namespace mol {

  CoordFrame CreateCoordFrame(const geom::Vec3List& atom_pos)
  {
    return CoordFrame(atom_pos);
  }
  
  geom::Vec3 CoordFrame::GetAtomPos(const AtomHandle& atom){
  //Returns the position of the atom in this frame
    return this->GetAtomPos(atom.GetIndex());
  }
  geom::Vec3 CoordFrame::GetAtomPos(int i1){
  //Returns the position in this frame of the atom with index i1 
    return (*this)[i1];
  }
  
  Real CoordFrame::GetDistanceBetwAtoms(const AtomHandle& a1, const AtomHandle& a2){
  //Return the distance in this frame between two atoms
    return this->GetDistanceBetwAtoms(a1.GetIndex(),a2.GetIndex());
  }
  Real CoordFrame::GetDistanceBetwAtoms(int i1, int i2){
  //Return the distance in this frame between the two atoms with indices i1 and i2 
    return geom::Distance((*this)[i1],(*this)[i2]);
  }
  
  Real CoordFrame::GetAngle(const AtomHandle& a1, const AtomHandle& a2, const AtomHandle& a3){
  //Returns the Angle between the three atoms, a2 being considered as the central atom
  //i.e the angle between the vector (a1.pos-a2.pos) and (a3.pos-a2.pos)
    return this->GetAngle(a1.GetIndex(),a2.GetIndex(),a3.GetIndex());  
  }
  Real CoordFrame::GetAngle(int i1, int i2, int i3){
  //Returns the angl between the three atoms with indices i1,i2,i3
    return geom::Angle((*this)[i1]-(*this)[i2],(*this)[i3]-(*this)[i2]);
  }
  
  Real CoordFrame::GetDihedralAngle(const AtomHandle& a1, const AtomHandle& a2, 
                                    const AtomHandle& a3, const AtomHandle& a4){
  //Returns the Dihedral angle between the four atoms a1,a2,a3,a4
    return this->GetDihedralAngle(a1.GetIndex(),a2.GetIndex(),a3.GetIndex(),a4.GetIndex());
  }
  Real CoordFrame::GetDihedralAngle(int i1, int i2, int i3, int i4){
  //Returns the Dihedral angle between the four atoms with indices i1,i2,i3,i4
    return geom::DihedralAngle((*this)[i1],(*this)[i2],(*this)[i3],(*this)[i4]);
  }

  void GetIndices(const EntityView& sele, std::vector<unsigned long>& indices){
  // This function returns a vector containing the atom indices of the atoms in an EntityView
  // It is used to accelerate the extraction of information from a trajectory
    AtomViewList atoms=sele.GetAtomList();
    indices.reserve(atoms.size());
    for (AtomViewList::const_iterator i=atoms.begin(), 
         e=atoms.end(); i!=e; ++i) {
      indices.push_back(i->GetIndex());
    }
  }

  void GetCaIndices(const EntityView& segment, std::vector<unsigned long>& indices_ca){
    mol::AtomView ca;
    ResidueViewList residues=segment.GetResidueList();
    indices_ca.reserve(residues.size());
    //for (ResidueViewList::const_iterator res=residues.begin()+1,
    //     e=residues.end(); res!=e-1; ++res){
    //  if (!InSequence((*res).GetHandle(),(*(res+1)).GetHandle())) throw std::runtime_error("Residues are not in a continuous sequence");
    //}
    for (ResidueViewList::const_iterator res=residues.begin(),
         e=residues.end(); res!=e; ++res) {
      ca=res->FindAtom("CA");
      CheckHandleValidity(ca);
      indices_ca.push_back(ca.GetIndex());
      }
  }
  
  void GetCaCONIndices(const EntityView& segment, std::vector<unsigned long>& indices_ca,
                      std::vector<unsigned long>& indices_c, std::vector<unsigned long>& indices_o, std::vector<unsigned long>& indices_n){
    mol::AtomView a,n,c,o;
    ResidueViewList residues=segment.GetResidueList();
    indices_ca.reserve(residues.size());
    indices_n.reserve(residues.size());
    indices_c.reserve(residues.size());
    indices_o.reserve(residues.size());
    for (ResidueViewList::const_iterator res=residues.begin()+1,
         e=residues.end(); res!=e-1; ++res){
      if (!InSequence((*res).GetHandle(),(*(res+1)).GetHandle())) throw std::runtime_error("Residues are not in a continuous sequence");
    }
    for (ResidueViewList::const_iterator res=residues.begin(),
         e=residues.end(); res!=e; ++res) {
      a=res->FindAtom("CA");
      CheckHandleValidity(a);
      indices_ca.push_back(a.GetIndex());
      c=res->FindAtom("C");
      CheckHandleValidity(c);
      indices_c.push_back(c.GetIndex());
      o=res->FindAtom("O");
      CheckHandleValidity(o);
      indices_o.push_back(o.GetIndex());
      n=res->FindAtom("N");
      CheckHandleValidity(n);
      indices_n.push_back(n.GetIndex());
    }
  }

  void GetMasses(const EntityView& sele, std::vector<Real>& masses){
  // This function returns a vector containing the atom masses of the atoms in an EntityView
  // It is used together with GetIndices to accelerate the extraction of RMSD from a trajectory
    Real mass_tot=0.0;
    AtomViewList atoms=sele.GetAtomList();
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
  
  
  void GetIndicesAndMasses(const EntityView& sele, std::vector<unsigned long>& indices,std::vector<Real>& masses){
    GetIndices(sele, indices);
    GetMasses(sele, masses);
  }
  
  void GetPositions(const EntityView& sele, std::vector<geom::Vec3>& ref_pos){
  //Returns the positions of all the atoms in the EntityView
    ref_pos.reserve(sele.GetAtomCount());
    for (mol::AtomViewIter a=sele.AtomsBegin(),e=sele.AtomsEnd(); a!=e; ++a) {
      ref_pos.push_back((*a).GetPos());
    }
  }
  
  geom::Vec3 CoordFrame::GetCenterOfMassPos(const EntityView& sele){
  //Returns the position of the centor of mass of the atoms in the EntityView
    std::vector<unsigned long> indices;
    std::vector<Real> masses;
    GetIndicesAndMasses(sele,indices,masses);
    return this->GetCenterOfMassPos(indices,masses);
  }
  
  geom::Vec3 CoordFrame::GetCenterOfMassPos(std::vector<unsigned long>& indices,std::vector<Real>& masses){
  //Returns the position of the centor of mass of the atoms from which the indices and masses are passed 
  //as vectors in argument
    geom::Vec3 v;
    for (unsigned int i=0,e=indices.size();i!=e; i++) {
      v+=masses[i]*(*this)[indices[i]];
    }
    return v;
  }
  
  Real CoordFrame::GetDistanceBetwCenterOfMass(const EntityView& sele1,const EntityView& sele2){
  //Returns the distance between the centers of mass of the two EntityViews
    std::vector<unsigned long> indices1,indices2;
    std::vector<Real> masses1,masses2;
    GetIndicesAndMasses(sele1,indices1,masses1);
    GetIndicesAndMasses(sele2,indices2,masses2);
    return this->GetDistanceBetwCenterOfMass(indices1,masses1,indices2,masses2);
  }
  
  Real CoordFrame::GetDistanceBetwCenterOfMass(std::vector<unsigned long>& indices1,std::vector<Real>& masses1,
                           std::vector<unsigned long>& indices2,std::vector<Real>& masses2){
  //Returns the distance between the centers of mass of the two groups of atoms from which the
  //indices and masses are given as vectors as argument
    geom::Vec3 v1=this->GetCenterOfMassPos(indices1, masses1);
    geom::Vec3 v2=this->GetCenterOfMassPos(indices2, masses2);
    return geom::Distance(v1,v2);
  }
  
  Real CoordFrame::GetRMSD(const std::vector<geom::Vec3>& ref_pos,const std::vector<unsigned long>& indices_sele){
  //Returns the RMSD between the positions of the atoms whose indices are given in indices_sele and the positions
  //given in ref_pos
    Real rmsd=0.0,val;
    for (unsigned int i1=0; i1!=indices_sele.size(); ++i1) {
      geom::Vec3 av_sele=(*this)[indices_sele[i1]];
      geom::Vec3 av_ref=ref_pos[i1];
      val=geom::Length2(av_ref-av_sele);
      rmsd+=val;
    }
    return pow(rmsd/indices_sele.size(),0.5);
  }
  
  Real CoordFrame::GetRMSD(const EntityView& reference_view,const EntityView& sele_view){
  //Return the rmsd between two EntityViews. The reference positions are taken directly from the reference_view
  //whereas they are taken from this CoordFrame for the sele_view
    int count_ref=reference_view.GetAtomCount();
    int count_sele=sele_view.GetAtomCount();
    if (count_ref!=count_sele){
      throw std::runtime_error("atom counts of the two views are not equal");
    }
    std::vector<unsigned long> indices_sele;
    std::vector<geom::Vec3> ref_pos;
    GetIndices(sele_view,indices_sele);
    GetPositions(reference_view,ref_pos);
    return this->GetRMSD(ref_pos,indices_sele);
  }
  
  Real CoordFrame::GetMinDistance(std::vector<unsigned long>& index_list1, std::vector<unsigned long>& index_list2){
  //Returns the minimal distance between two groups of atoms with indices given by index_list1 and index_list2
    geom::Vec3List pos_list1,pos_list2;
    for (std::vector<unsigned long>::const_iterator i1=index_list1.begin(),e=index_list1.end(); i1!=e; ++i1) {
      pos_list1.push_back((*this)[*i1]);
    }
    for (std::vector<unsigned long>::const_iterator i1=index_list2.begin(),e=index_list2.end(); i1!=e; ++i1) {
      pos_list2.push_back((*this)[*i1]);
    }
    return geom::MinDistance(pos_list1,pos_list2);
  }
  Real CoordFrame::GetMinDistance(const mol::EntityView& view1, const mol::EntityView& view2){
  //Returns the minimal distance between the atoms of two views (view1 and view2)
    std::vector<unsigned long> index_list1,index_list2;
    GetIndices(view1,index_list1);
    GetIndices(view2,index_list2);
    return this->GetMinDistance(index_list1,index_list2);
  }
  
  Real CoordFrame::GetMinDistBetwCenterOfMassAndView(std::vector<unsigned long>& indices_cm, std::vector<Real>& masses_cm,
                                                     std::vector<unsigned long>& indices_atoms){
  //Returns the minimal distance between the center of mass of a group of atoms (with indices given in indices_cm and masses
  //in masses_cm) and the atoms of another group (with indices given in indices_atoms)
    geom::Vec3List cm_pos,atoms_pos_list;
    cm_pos.push_back(this->GetCenterOfMassPos(indices_cm, masses_cm));
    for (std::vector<unsigned long>::const_iterator i1=indices_atoms.begin(),e=indices_atoms.end(); i1!=e; ++i1) {
      atoms_pos_list.push_back((*this)[*i1]);
    }
    return geom::MinDistance(cm_pos,atoms_pos_list);
  }
  Real CoordFrame::GetMinDistBetwCenterOfMassAndView(const mol::EntityView& view_cm, const mol::EntityView& view_atoms){
  //Returns the minimal distance between the center of mass of a view (view_cm) and the atoms of another view (view_atoms)
    std::vector<unsigned long> indices_cm,indices_atoms;
    std::vector<Real> masses_cm;
    GetIndices(view_atoms,indices_atoms);
    GetIndicesAndMasses(view_cm,indices_cm,masses_cm);
    return this->GetMinDistBetwCenterOfMassAndView(indices_cm,masses_cm,indices_atoms);
  }

  geom::Line3 CoordFrame::GetODRLine(std::vector<unsigned long>& indices_ca){
  //Returns the best fit line to atoms with indices in indices_ca
    geom::Vec3List atoms_pos_list;
    atoms_pos_list.reserve(indices_ca.size());
    for (std::vector<unsigned long>::const_iterator i1=indices_ca.begin(),e=indices_ca.end(); i1!=e; ++i1) {
      atoms_pos_list.push_back((*this)[*i1]);
    }
    return atoms_pos_list.GetODRLine();
  }
 
  geom::Line3 CoordFrame::FitCylinder(std::vector<unsigned long>& indices_ca){
  //Returns a line which is the axis of a fitted cylinder to the atoms with indices given in indices_ca
  //It is assumed that we fit an alpha-helix and that the CA atoms are oredered sequentially
  //This is used for the initial guess of the helix axis
    geom::Vec3List atoms_pos_list;
    int n_atoms=indices_ca.size();
    atoms_pos_list.reserve(n_atoms);
    for (std::vector<unsigned long>::const_iterator i1=indices_ca.begin(),e=indices_ca.end(); i1!=e; ++i1) {
      atoms_pos_list.push_back((*this)[*i1]);
    }
    //Initial guess
    geom::Vec3 initial_axis=geom::Vec3(0.0,0.0,0.0),center=geom::Vec3(0.0,0.0,0.0);
    if (n_atoms<5) {
      initial_axis=atoms_pos_list[n_atoms-1]-atoms_pos_list[0];
    }
    else {
      for (geom::Vec3List::const_iterator i=atoms_pos_list.begin(),e=atoms_pos_list.end()-4; i!=e; ++i) {
        initial_axis+=(*(i+4))-(*i);
      }
    }
    for (geom::Vec3List::const_iterator i=atoms_pos_list.begin(),e=atoms_pos_list.end(); i!=e; ++i) {
      center+=(*i);
    }
    center/=atoms_pos_list.size();
    return atoms_pos_list.FitCylinder(initial_axis,center);
  }
 
  Real CoordFrame::GetAlphaHelixContent(const mol::EntityView& segment){
    //Returns the percentage of residues in the EntityView (segment) that are in an alpha-helix
    //Each residue is assigned as being in an alpha-helix or not, based on and backbone torsion angles
    //and the hydrogen bonding (O-N) (this term is used only if the segment contains at least 8 residues
    //The entity view should be a single segment with no gaps and no missing N,CA,C,O backbone atoms
    //the first and last residues will not be asessed for helicity
    CheckHandleValidity(segment);
    std::vector<unsigned long> indices_c, indices_n, indices_ca, indices_o;
    GetCaCONIndices(segment, indices_ca, indices_c, indices_o, indices_n);
    return CoordFrame::GetAlphaHelixContent(indices_ca,indices_c,indices_o,indices_n);
  }

  Real Eval2AngleDist(Real phi, Real phi0, Real psi, Real psi0, Real delta){
    //Returns a score between 0 and 1 measuring the distance between
    //a pair of angles and a pair of reference angles
    Real d1,d2,d;
    d1=abs(phi-phi0);
    d2=abs(psi-psi0);
    if (d1>M_PI) d1=abs(d1-2.*M_PI);
    if (d2>M_PI) d1=abs(d2-2.*M_PI);
    d=(pow(d1,2.)+pow(d2,2.))/pow(delta,2.);
    return 1./(1+d);
  }

  Real CoordFrame::GetAlphaHelixContent(std::vector<unsigned long>& indices_ca, std::vector<unsigned long>& indices_c,
                                         std::vector<unsigned long>& indices_o, std::vector<unsigned long>& indices_n){
  //See CoordFrame::GetAlphaHelixContent(const mol::EntityView) above.
  unsigned long n_atoms=indices_ca.size();
  geom::Vec3 c_previous,n,ca,c,n_next;
  Real phi,psi,phi_0=-1.2,psi_0=-0.785,delta=0.8,d_0=3.3;
  unsigned long n_helical_res=0;
  std::vector<Real> score,dist,score2,dist2;
  score.reserve(n_atoms-2);
  score2.reserve(n_atoms-2);
  dist.reserve(n_atoms-2);
  dist2.reserve(n_atoms-2);
  if (n_atoms!=indices_n.size()||n_atoms!=indices_c.size()||n_atoms!=indices_o.size()){
    throw std::runtime_error("not same numbers of CA, C, O and N atoms in the selection");
  }
  if (n_atoms<=5){
    throw std::runtime_error("At least five residues are needed to calulate an alpha helix similarity");
  }
  c=(*this)[indices_c[0]];
  n_next=(*this)[indices_n[1]];
  for (unsigned long i=1; i!=n_atoms-1; ++i){
    c_previous=c;
    n=n_next;
    ca=(*this)[indices_ca[i]];
    c=(*this)[indices_c[i]];
    n_next=(*this)[indices_n[i+1]];
    phi=geom::DihedralAngle(c_previous,n,ca,c);
    psi=geom::DihedralAngle(n,ca,c,n_next);
    score.push_back(Eval2AngleDist(phi,phi_0,psi,psi_0,delta));
  }
  score2[0]=pow(score[0]*score[1],3./2.);
  score2[n_atoms-3]=pow(score[n_atoms-3]*score[n_atoms-4],3./2.);
  for (unsigned long i=1; i!=n_atoms-3; ++i){
    score2[i]=score[i-1]*score[i]*score[i+1];
  }
  if (n_atoms>=8){
    for (unsigned long i=1; i!=n_atoms-1; ++i){
      if (i<n_atoms-4){
        dist.push_back(geom::Distance((*this)[indices_o[i]],(*this)[indices_n[i+4]]));
        if (i>=5) {
          dist2.push_back(std::min(dist[i-1],dist[i-5]));
        }
        else dist2.push_back(dist[i-1]);
      }
      else dist2.push_back(geom::Distance((*this)[indices_n[i]],(*this)[indices_o[i-4]]));
    }
    for (unsigned long i=0; i!=n_atoms-2; ++i){
      if (dist2[i]<=d_0 || score2[i]>=0.3)n_helical_res+=1;
    }
  }
  else {
    for (unsigned long i=0; i!=n_atoms-2; ++i){
      if (score2[i]>=0.3)n_helical_res+=1;
    }
  }
  return Real(n_helical_res)/Real(n_atoms-2);
  }
 


}}//ns

