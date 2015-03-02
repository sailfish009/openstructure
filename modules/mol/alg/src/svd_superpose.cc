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
 * Author Juergen Haas
 */
#include <stdexcept>
#include <iostream>
#include <boost/bind.hpp>

#include <Eigen/Core>
#include <Eigen/Array>
#include <Eigen/SVD>
#include <Eigen/LU>


#include <ost/base.hh>
#include <ost/geom/vec3.hh>
#include <ost/mol/alg/svd_superpose.hh>
#include <ost/mol/xcs_editor.hh>
#include <ost/mol/view_op.hh>
#include <ost/mol/atom_view.hh>
#include <ost/mol/iterator.hh>
namespace ost { namespace mol { namespace alg {


using boost::bind;
typedef Eigen::Matrix<Real,3,1> EVec3;
typedef Eigen::Matrix<Real,3,3> EMat3;
typedef Eigen::Matrix<Real,1,3> ERVec3;
typedef Eigen::Matrix<Real,Eigen::Dynamic,Eigen::Dynamic> EMatX;
typedef Eigen::Matrix<Real,1,Eigen::Dynamic> ERVecX;

Real CalculateRMSD(const mol::EntityView& ev1,
                   const mol::EntityView& ev2,
                   const geom::Mat4& transformation) {
  mol::EntityView evt;
  mol::EntityHandle eht, eh1;
  int counta=ev1.GetAtomCount();
  int countb=ev2.GetAtomCount();
  if (counta!=countb){
    throw Error("atom counts of the two views are not equal");
  }
  eht=ev1.GetHandle();
  int natoms=ev1.GetAtomCount();
  Real rmsd=0.0;
  mol::AtomViewIter a_ev1=ev1.AtomsBegin();
  mol::AtomViewIter a_ev1_end=ev1.AtomsEnd();
  mol::AtomViewIter a_ev2=ev2.AtomsBegin();
  for (int counter=0; a_ev1_end!=a_ev1; ++a_ev1, ++a_ev2, ++counter) {
    mol::AtomView av1=*a_ev1;
    geom::Vec3 apos=av1.GetPos();
    mol::AtomView av2=*a_ev2;
    geom::Vec3 bpos=av2.GetPos();
    Real val=geom::Length2(geom::Vec3(transformation*geom::Vec4(apos))-bpos);
    rmsd+=val;
  }
  rmsd=sqrt(rmsd/natoms);
  return rmsd;
}




Real calc_rmsd_for_atom_lists(const mol::AtomViewList& atoms1,
                                const mol::AtomViewList& atoms2,
                                const geom::Mat4& transformation)
{
  mol::EntityView evt;
  mol::EntityHandle eht, eh1;
//  convert view ev1 to handle
  int natoms=static_cast<int>(atoms1.size());
  Real rmsd=0.0;
  mol::AtomViewList::const_iterator a_ev1=atoms1.begin();
  mol::AtomViewList::const_iterator a_ev1_end=atoms1.end();
  mol::AtomViewList::const_iterator a_ev2=atoms2.begin();
  for (int counter=0; a_ev1_end!=a_ev1; ++a_ev1, ++a_ev2, ++counter) {
    mol::AtomView av1=*a_ev1;
    geom::Vec3 apos=av1.GetPos();
    mol::AtomView av2=*a_ev2;
    geom::Vec3 bpos=av2.GetPos();
    Real val=geom::Length2(geom::Vec3(transformation*geom::Vec4(apos))-bpos);
    rmsd+=val;
  }
  rmsd=sqrt(rmsd/natoms);
  return rmsd;
}

class SuperposerSVDImpl {
public:
  SuperposerSVDImpl(int natoms, bool alloc_atoms):
    natoms_(natoms), alloc_atoms_(alloc_atoms)
  {

    ERVec3 avg1_=EVec3::Zero();
    ERVec3 avg2_=EVec3::Zero();

    if (alloc_atoms_) {
      atoms1_=EMatX::Zero(natoms,3);
      atoms2_=EMatX::Zero(natoms,3);
    }

  }
private:
  int natoms_;
  bool alloc_atoms_;
  EMatX atoms1_;
  EMatX atoms2_;
public:  
  void EntToMatrices(const mol::EntityView& ev1, const mol::EntityView& ev2);
  geom::Vec3 EigenVec3ToVec3(const EVec3 &vec);
  geom::Mat3 EigenMat3ToMat3(const EMat3 &mat);
  EVec3 Vec3ToEigenRVec(const geom::Vec3 &vec);
  EVec3 Vec3ToEigenVec(const geom::Vec3 &vec);
  EMatX SubtractVecFromMatrixRows(EMatX Mat,
                                  ERVecX Vec);
  SuperpositionResult Run(const mol::AtomViewList& atoms1,          
                          const mol::AtomViewList& atoms2);
  SuperpositionResult Run(const std::vector<geom::Vec3>& pl1,
                          const std::vector<geom::Vec3>& pl2);
  SuperpositionResult DoSuperposition();
  EIGEN_MAKE_ALIGNED_OPERATOR_NEW //needed only if *fixed* sized matrices are
                                    //members of classes  
};

SuperposerSVD::~SuperposerSVD()
{
  assert(impl_);
  delete impl_;
}


SuperposerSVD::SuperposerSVD(int natoms, bool alloc_atoms):
  impl_(new SuperposerSVDImpl(natoms, alloc_atoms))
{
  
}

geom::Vec3 SuperposerSVDImpl::EigenVec3ToVec3(const EVec3 &vec)
{
  return geom::Vec3(vec.data());
}

geom::Mat3 SuperposerSVDImpl::EigenMat3ToMat3(const EMat3 &mat)
{
  return geom::Mat3(mat.data());
}

EVec3 SuperposerSVDImpl::Vec3ToEigenRVec(const geom::Vec3 &vec)
{
  return EVec3(&vec[0]);
}

EVec3 SuperposerSVDImpl::Vec3ToEigenVec(const geom::Vec3 &vec)
{
  return EVec3(&vec[0]);
}

EMatX SuperposerSVDImpl::SubtractVecFromMatrixRows(EMatX Mat,
                                               ERVecX Vec)
{

  for (int row=0;  row<Mat.rows();++row) {
    Mat.row(row)-=Vec;
  }
  return Mat;
}

void SuperposerSVDImpl::EntToMatrices(const mol::EntityView& ev1,
                                      const mol::EntityView& ev2)
{
  //Now iterate over atoms in entities and extract coords into Nx3 matrices
  //for superposition
  mol::AtomViewIter a_ev1=ev1.AtomsBegin();
  mol::AtomViewIter a_ev1_end=ev1.AtomsEnd();
  mol::AtomViewIter a_ev2=ev2.AtomsBegin();
  ERVec3 e;
  geom::Vec3 pos;
  mol::AtomView av1, av2;

  for (int counter=0; a_ev1_end!=a_ev1; ++a_ev1, ++a_ev2, ++counter) {
  av1=*a_ev1;
  pos=av1.GetPos();
  e=SuperposerSVDImpl::Vec3ToEigenVec(pos);
  atoms1_.row(counter)=e ;
  av2=*a_ev2;
  pos=av2.GetPos();
  e=SuperposerSVDImpl::Vec3ToEigenVec(pos);
  atoms2_.row(counter)=e;
  }
}


SuperpositionResult SuperposerSVDImpl::DoSuperposition()
{
  ERVec3 avg1_=atoms1_.colwise().sum()/atoms1_.rows();
  ERVec3 avg2_=atoms2_.colwise().sum()/atoms2_.rows();
  //center the structures
  atoms1_=this->SubtractVecFromMatrixRows(atoms1_, avg1_);
  atoms2_=this->SubtractVecFromMatrixRows(atoms2_, avg2_);
  EMatX atoms2=atoms2_.transpose();

  //single value decomposition
  Eigen::SVD<EMat3> svd(atoms2*atoms1_);

  EMatX matrixVT=svd.matrixV().transpose();
  //determine rotation
  Real detv=matrixVT.determinant();
  Real dett=svd.matrixU().determinant();
  Real det=detv*dett;
  EMat3 ERot;
  if (det<0) {
    EMat3 tmat=EMat3::Identity();
    tmat(2,2)=-1;
    ERot=(svd.matrixU()*tmat)*matrixVT;
  }else{
    ERot=svd.matrixU()*matrixVT;
  }

  SuperpositionResult res;
  //  prepare rmsd calculation
  geom::Vec3 shift=SuperposerSVDImpl::EigenVec3ToVec3(avg2_);
  geom::Vec3 com_vec=-SuperposerSVDImpl::EigenVec3ToVec3(avg1_);
  geom::Mat3 rot=SuperposerSVDImpl::EigenMat3ToMat3(ERot.transpose());
  geom::Mat4 mat4_com, mat4_rot, mat4_shift;
  mat4_rot.PasteRotation(rot);
  mat4_shift.PasteTranslation(shift);
  mat4_com.PasteTranslation(com_vec);
  //save whole transformation
  res.transformation=geom::Mat4(mat4_shift*mat4_rot*mat4_com);
  return res;

}


SuperpositionResult SuperposerSVD::Run(const mol::EntityView& ev1,
                                        const mol::EntityView& ev2)
{
  impl_->EntToMatrices(ev1, ev2);
  SuperpositionResult r=impl_->DoSuperposition();
  r.rmsd=CalculateRMSD(ev1, ev2, r.transformation);
  return r;
}

SuperpositionResult SuperposerSVD::Run(const mol::AtomViewList& atoms1,
                                       const mol::AtomViewList& atoms2)
{
  return impl_->Run(atoms1, atoms2);
}

SuperpositionResult SuperposerSVD::Run(const std::vector<geom::Vec3>& pl1,
                                       const std::vector<geom::Vec3>& pl2)
{
  return impl_->Run(pl1,pl2);
}
  
SuperpositionResult SuperposerSVDImpl::Run(const mol::AtomViewList& atoms1,
                                           const mol::AtomViewList& atoms2)
{
   mol::AtomViewList::const_iterator a_ev1=atoms1.begin();
   mol::AtomViewList::const_iterator a_ev1_end=atoms1.end();
   mol::AtomViewList::const_iterator a_ev2=atoms2.begin();

  for (int counter=0; a_ev1_end!=a_ev1; ++a_ev1, ++a_ev2, ++counter) {
    mol::AtomView av=*a_ev1;
    geom::Vec3 pos=av.GetPos();
    ERVec3 e=SuperposerSVDImpl::Vec3ToEigenVec(pos);
    atoms1_.row(counter)=e ;
    av=*a_ev2;
    pos=av.GetPos();
    e=SuperposerSVDImpl::Vec3ToEigenVec(pos);
    atoms2_.row(counter)=e;
  }
  return this->DoSuperposition();
}

SuperpositionResult SuperposerSVDImpl::Run(const std::vector<geom::Vec3>& pl1,
                                           const std::vector<geom::Vec3>& pl2)
{
  for (unsigned int counter=0; counter<pl1.size();++counter) {
    atoms1_.row(counter)=ERVec3(SuperposerSVDImpl::Vec3ToEigenVec(pl1[counter]));
    atoms2_.row(counter)=ERVec3(SuperposerSVDImpl::Vec3ToEigenVec(pl2[counter]));
  }
  return this->DoSuperposition();
}


SuperpositionResult SuperposeAtoms(const mol::AtomViewList& atoms1,
                                   const mol::AtomViewList& atoms2,
                                   bool apply_transform=true)
{
  int counta=static_cast<int>(atoms1.size());
  int countb=static_cast<int>(atoms2.size());
  if (counta!=countb){
    throw Error("atom counts do not match");
  }
  if ((counta<3)){
    throw Error("at least 3 atoms are required");
  }
  SuperposerSVD sp(counta, true);

  SuperpositionResult res=sp.Run(atoms1, atoms2);
  //save rmsd info
  res.rmsd=calc_rmsd_for_atom_lists(atoms1, atoms2, res.transformation);
  res.ncycles=1;
  mol::AtomView jv=atoms1.front();
  if (apply_transform){
    mol::XCSEditor ed=jv.GetResidue().GetChain().GetEntity().GetHandle().EditXCS();
    ed.ApplyTransform(res.transformation);
  }
  return res;
}

SuperpositionResult SuperposeSVD(const mol::EntityView& ev1,
                                 const mol::EntityView& ev2,
                                 bool apply_transform=true) {
  int counta=ev1.GetAtomCount();
  int countb=ev2.GetAtomCount();
  if (counta!=countb){
    throw Error("atom counts of the two views are not equal");
  }
  if ((counta<3)){
    throw Error("atom counts of any view must be larger or "
                             "equal 3");
  }
  int nrows=counta;
  SuperposerSVD sp(nrows, true);
  SuperpositionResult res=sp.Run(ev1, ev2);
  res.entity_view1=ev1;
  res.entity_view2=ev2;
  res.ncycles=1;
  //save rmsd info
  res.rmsd=CalculateRMSD(ev1, ev2, res.transformation);
  if (apply_transform){
    mol::XCSEditor ed=ev1.GetHandle().EditXCS();
    ed.ApplyTransform(res.transformation);
  }
  return res;
}

SuperpositionResult SuperposeSVD(const std::vector<geom::Vec3>& pl1,
                                 const std::vector<geom::Vec3>& pl2)
{
  if (pl1.size()!=pl2.size()){
    throw Error("pointlist lengths not equal");
  }
  if ((pl1.size()<3)){
    throw Error("three or more points required");
  }
  SuperposerSVD sp(pl1.size(), true);
  SuperpositionResult res=sp.Run(pl1, pl2);
  res.entity_view1=EntityView();
  res.entity_view2=EntityView();
  res.ncycles=1;
  res.rmsd=0.0;
  return res;
}

SuperpositionResult IterativeSuperposition(mol::EntityView& ev1,
                                           mol::EntityView& ev2,
                                           int ncycles,
                                           Real dist_thres,
                                           bool apply_transform=true) {
  SuperpositionResult result;
  int counter=0;
  mol::AtomViewList atoms_a, atoms_b;
  std::copy(ev1.AtomsBegin(), ev1.AtomsEnd(), std::back_inserter(atoms_a));
  std::copy(ev2.AtomsBegin(), ev2.AtomsEnd(), std::back_inserter(atoms_b));
  bool converged=false;

  if (dist_thres<=0){
    dist_thres=5.0;
  }
  counter=0;
  while (!converged) {
    if (atoms_a.size()<3) {
      return result;
    }
    result=SuperposeAtoms(atoms_a, atoms_b, false);
    converged=ncycles<=counter;
    if (converged) {
      continue;
    }
    converged=true;
    for (mol::AtomViewList::iterator i=atoms_a.begin(), j=atoms_b.begin(),
         e=atoms_a.end(); i!=e; ++i, ++j) {
      mol::AtomView iv=*i;
      mol::AtomView jv=*j;

      if (geom::Length(geom::Vec3(result.transformation*geom::Vec4(iv.GetPos()))-jv.GetPos())>dist_thres) {
        *i=*j=mol::AtomView();
        converged=false;
      }
    }
    if (!converged) {
      mol::AtomViewList::iterator end;
      mol::AtomView invalid_atom;
      end=std::remove(atoms_a.begin(), atoms_a.end(), invalid_atom);
      atoms_a.erase(end, atoms_a.end());
      end=std::remove(atoms_b.begin(), atoms_b.end(), invalid_atom);
      atoms_b.erase(end, atoms_b.end());
    }
    counter++;

  }
  result.ncycles=counter;
  result.entity_view1=CreateViewFromAtomList(atoms_a);
  result.entity_view2=CreateViewFromAtomList(atoms_b);

  if (apply_transform){
    mol::XCSEditor ed=ev1.GetHandle().EditXCS();
    ed.ApplyTransform(result.transformation);
  }
  return result;
}


}}} //ns
