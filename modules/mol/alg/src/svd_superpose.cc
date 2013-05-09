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
#include <ost/mol/residue_handle.hh>
#include <ost/mol/residue_view.hh>
#include <ost/mol/chain_view.hh>
#include <ost/mol/chain_handle.hh>
#include <ost/mol/view_op.hh>
#include <ost/mol/atom_view.hh>

namespace ost { namespace mol { namespace alg {


using boost::bind;
typedef Eigen::Matrix<Real,3,1> EVec3;
typedef Eigen::Matrix<Real,3,3> EMat3;
typedef Eigen::Matrix<Real,1,3> ERVec3;
typedef Eigen::Matrix<Real,Eigen::Dynamic,Eigen::Dynamic> EMatX;
typedef Eigen::Matrix<Real,1,Eigen::Dynamic> ERVecX;





Real calc_rmsd_for_point_lists(const std::vector<geom::Vec3>& points1,
                              const std::vector<geom::Vec3>& points2,
                              const geom::Mat4& transformation)
{
  
  Real rmsd=0.0;
  std::vector<geom::Vec3>::const_iterator a_ev1=points1.begin();
  std::vector<geom::Vec3>::const_iterator a_ev1_end=points1.end();
  std::vector<geom::Vec3>::const_iterator a_ev2=points2.begin();
  for (; a_ev1_end!=a_ev1; ++a_ev1, ++a_ev2) {
    Real val=geom::Length2(geom::Vec3(transformation*geom::Vec4(*a_ev1))-*a_ev2);
    rmsd+=val;
  }
  rmsd=sqrt(rmsd/points1.size());
  return rmsd;
}

Real calc_rmsd_for_atom_lists(const mol::AtomViewList& atoms1,
                              const mol::AtomViewList& atoms2,
                              const geom::Mat4& transformation)
{
  Real rmsd=0.0;
  mol::AtomViewList::const_iterator a_ev1=atoms1.begin();
  mol::AtomViewList::const_iterator a_ev1_end=atoms1.end();
  mol::AtomViewList::const_iterator a_ev2=atoms2.begin();
  for (; a_ev1_end!=a_ev1; ++a_ev1, ++a_ev2) {
    mol::AtomView av1=*a_ev1;
    geom::Vec3 apos=av1.GetPos();
    mol::AtomView av2=*a_ev2;
    geom::Vec3 bpos=av2.GetPos();
    Real val=geom::Length2(geom::Vec3(transformation*geom::Vec4(apos))-bpos);
    rmsd+=val;
  }
  rmsd=sqrt(rmsd/atoms1.size());
  return rmsd;
}

Real CalculateRMSD(const mol::EntityView& ev1,
                   const mol::EntityView& ev2,
                   const geom::Mat4& transformation) {

  return calc_rmsd_for_atom_lists(ev1.GetAtomList(), ev2.GetAtomList(), 
                                  transformation);
}

geom::Vec3 EigenVec3ToVec3(const EVec3 &vec)
{
  return geom::Vec3(vec.data());
}

geom::Mat3 EigenMat3ToMat3(const EMat3 &mat)
{
  return geom::Mat3(mat.data());
}

EVec3 Vec3ToEigenRVec(const geom::Vec3 &vec)
{
  return EVec3(&vec[0]);
}

EVec3 Vec3ToEigenVec(const geom::Vec3 &vec)
{
  return EVec3(&vec[0]);
}

EMatX MatrixShiftedBy(EMatX mat, ERVecX vec)
{
  EMatX result = mat;
  for (int row=0;  row<mat.rows();++row) {
    result.row(row) -= vec;
  }
  return result;
}

class MeanSquareMinimizerImpl {
public:
  MeanSquareMinimizerImpl(int n_atoms, bool alloc_atoms):
    n_atoms_(n_atoms), alloc_atoms_(alloc_atoms) 
  {
    if (alloc_atoms_) {
      atoms1_=EMatX::Zero(n_atoms_, 3);
      atoms2_=EMatX::Zero(n_atoms_, 3);
    }
  }

  void SetRefPos(size_t index, const geom::Vec3& pos) {
    atoms2_.row(index) = ERVec3(Vec3ToEigenVec(pos));
  }

  void SetPos(size_t index, const geom::Vec3& pos) {
    atoms1_.row(index) = ERVec3(Vec3ToEigenVec(pos));
  }

  SuperpositionResult MinimizeOnce() const;
private:
  int   n_atoms_;
  bool  alloc_atoms_;
  EMatX atoms1_;
  EMatX atoms2_;
};


MeanSquareMinimizer MeanSquareMinimizer::FromAtomLists(const AtomViewList& atoms,
                                                       const AtomViewList& atoms_ref) {
  int n_atoms = atoms.size();
  if (n_atoms != atoms_ref.size()) {
    throw Error("atom counts do not match");
  }
  if (n_atoms<3) {
    throw Error("at least 3 atoms are required for superposition");
  }
  MeanSquareMinimizer msm;
  msm.impl_ = new MeanSquareMinimizerImpl(n_atoms, true);

  for (size_t i = 0; i < atoms.size(); ++i ) {
    msm.impl_->SetRefPos(i, atoms_ref[i].GetPos());
    msm.impl_->SetPos(i, atoms[i].GetPos());
  }
  return msm;
}

MeanSquareMinimizer MeanSquareMinimizer::FromPointLists(const std::vector<geom::Vec3>& points,
                                                        const std::vector<geom::Vec3>& points_ref) {
  int n_points = points.size();
  if (n_points != points.size()) {
    throw Error("point counts do not match");
  }
  if (n_points<3) {
    throw Error("at least 3 points are required for superposition");
  }
  MeanSquareMinimizer msm;
  msm.impl_ = new MeanSquareMinimizerImpl(n_points, true);

  for (size_t i = 0; i < points.size(); ++i ) {
    msm.impl_->SetRefPos(i, points_ref[i]);
    msm.impl_->SetPos(i, points[i]);
  }
  return msm;
}

SuperpositionResult MeanSquareMinimizerImpl::MinimizeOnce() const {
  ERVec3 avg1 = atoms1_.colwise().sum()/atoms1_.rows();
  ERVec3 avg2 = atoms2_.colwise().sum()/atoms2_.rows();

  // SVD only determines the rotational component of the superposition
  // we need to manually shift the centers of the two point sets on onto 
  // origin

  EMatX atoms1 = MatrixShiftedBy(atoms1_, avg1);
  EMatX atoms2 = MatrixShiftedBy(atoms2_, avg2).transpose();

  // determine rotational component
  Eigen::SVD<EMat3> svd(atoms2*atoms1);
  EMatX matrixVT=svd.matrixV().transpose();

  //determine rotation
  Real detv=matrixVT.determinant();
  Real dett=svd.matrixU().determinant();
  Real det=detv*dett;
  EMat3 rotation;
  if (det<0) {
    EMat3 tmat=EMat3::Identity();
    tmat(2,2)=-1;
    rotation = (svd.matrixU()*tmat)*matrixVT;
  } else {
    rotation = svd.matrixU()*matrixVT;
  }
  
  SuperpositionResult res;

  geom::Vec3 shift = EigenVec3ToVec3(avg2);
  geom::Vec3 com_vec = -EigenVec3ToVec3(avg1);
  geom::Mat3 rot = EigenMat3ToMat3(rotation.transpose());
  geom::Mat4 mat4_com, mat4_rot, mat4_shift;
  mat4_rot.PasteRotation(rot);
  mat4_shift.PasteTranslation(shift);
  mat4_com.PasteTranslation(com_vec);
  //save whole transformation
  res.transformation = geom::Mat4(mat4_shift*mat4_rot*mat4_com);
  return res;
}

MeanSquareMinimizer& MeanSquareMinimizer::operator=(const MeanSquareMinimizer& rhs) {
  MeanSquareMinimizer tmp(rhs);
  this->swap(tmp);
  return *this;
}

MeanSquareMinimizer::MeanSquareMinimizer(const MeanSquareMinimizer& rhs) {
  if (rhs.impl_)
    impl_ = new MeanSquareMinimizerImpl(*rhs.impl_);
  else
    impl_ = NULL;
}


MeanSquareMinimizer::~MeanSquareMinimizer() {
  if (impl_) delete impl_;
}

SuperpositionResult MeanSquareMinimizer::MinimizeOnce() const {
  return impl_->MinimizeOnce();
}

SuperpositionResult SuperposeAtoms(const mol::AtomViewList& atoms1,
                                   const mol::AtomViewList& atoms2,
                                   bool apply_transform=true)
{
  MeanSquareMinimizer msm = MeanSquareMinimizer::FromAtomLists(atoms1, atoms2);
  SuperpositionResult result = msm.MinimizeOnce();
  
  result.ncycles=1;
  result.rmsd = calc_rmsd_for_atom_lists(atoms1, atoms2, result.transformation);
  if (apply_transform) {
    mol::AtomView jv=atoms1.front();
    mol::XCSEditor ed=jv.GetEntity().GetHandle().EditXCS();
    ed.ApplyTransform(result.transformation);
  }
  return result;

}

SuperpositionResult SuperposeSVD(const mol::EntityView& ev1,
                                 const mol::EntityView& ev2,
                                 bool apply_transform=true) {
  AtomViewList atoms1 = ev1.GetAtomList();
  AtomViewList atoms2 = ev2.GetAtomList();

  SuperpositionResult result = SuperposeAtoms(atoms1, atoms2, apply_transform);
  result.entity_view1 = ev1;
  result.entity_view2 = ev2;
  return result;
}

SuperpositionResult SuperposeSVD(const std::vector<geom::Vec3>& pl1,
                                 const std::vector<geom::Vec3>& pl2)
{
  MeanSquareMinimizer msm = MeanSquareMinimizer::FromPointLists(pl1, pl2);
  SuperpositionResult result = msm.MinimizeOnce();
  
  result.ncycles=1;
  result.rmsd = calc_rmsd_for_point_lists(pl1, pl2, result.transformation);
  return result;
}


}}} //ns
