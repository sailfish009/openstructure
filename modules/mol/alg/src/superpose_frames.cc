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

#include <Eigen/Core>
#include <Eigen/Array>
#include <Eigen/SVD>
#include <Eigen/LU>
#include <ost/mol/mol.hh>
#include <ost/mol/alg/superpose_frames.hh>

namespace ost { namespace mol { namespace alg {


typedef Eigen::Matrix<Real, 3, 3> EMat3;
typedef Eigen::Matrix<Real, 1, 3> ECVec3;
typedef Eigen::Matrix<Real, 3, 1> ERVec3;
typedef Eigen::Matrix<Real, 3, Eigen::Dynamic> EMat3X;
typedef Eigen::Matrix<Real, Eigen::Dynamic, 3> EMatX3;


inline geom::Vec3 rvec_to_gvec(const ERVec3 &vec) {
  return *reinterpret_cast<const geom::Vec3*>(&vec);
}

inline geom::Vec3 cvec_to_gvec(const ECVec3 &vec) {
  return *reinterpret_cast<const geom::Vec3*>(&vec);
}

inline geom::Mat3 emat_to_gmat(const EMat3 &mat)
{
  return *reinterpret_cast<const geom::Mat3*>(&mat);
}

inline ERVec3 gvec_to_rvec(const geom::Vec3 &vec)
{
  return *reinterpret_cast<const ERVec3*>(&vec);
}

inline ECVec3 gvec_to_cvec(const geom::Vec3 &vec)
{
  return *reinterpret_cast<const ECVec3*>(&vec);
}

inline EMat3X row_sub(const EMat3X& m, const ERVec3& s)
{
  EMat3X r(m);
  for (int col=0;  col<m.cols();++col) {
    r.col(col)-=s;
  }
  return r;
}

inline EMatX3 col_sub(const EMatX3& m, const ECVec3& s)
{
  EMatX3 r(m);
  for (int row=0;  row<m.rows();++row) {
    r.row(row)-=s;
  }
  return r;
}

CoordGroupHandle SuperposeFrames(CoordGroupHandle cg, EntityView sel,
                                 int begin, int end, int ref)
{
  int real_end=end==-1 ? cg.GetFrameCount() : end;
  CoordFramePtr ref_frame;
  std::vector<unsigned long> indices;
  if (!sel.IsValid()) {
    indices.reserve(cg.GetAtomCount());
    for (size_t i=0;i<cg.GetAtomCount(); ++i) {
      indices.push_back(i);
    }
  } else {
    AtomViewList atoms=sel.GetAtomList();
    indices.reserve(atoms.size());
    for (AtomViewList::const_iterator i=atoms.begin(), 
         e=atoms.end(); i!=e; ++i) {
      indices.push_back(i->GetIndex());
    }
  }
  CoordGroupHandle superposed=CreateCoordGroup(cg.GetEntity().GetAtomList());
  EMatX3 ref_mat=EMatX3::Zero(indices.size(), 3);
  EMatX3 ref_centered=EMatX3::Zero(indices.size(), 3);
  EMat3X frame_centered=EMat3X::Zero(3, indices.size());
  EMat3X frame_mat=EMat3X::Zero(3, indices.size());
  ECVec3 ref_center;
  ERVec3 frame_center;
  if (ref!=-1) {
    ref_frame=cg.GetFrame(ref);
    for (size_t i=0; i<indices.size(); ++i) {
      ref_mat.row(i)=gvec_to_cvec((*ref_frame)[indices[i]]);
    }
    ref_center=ref_mat.colwise().sum()/ref_mat.rows();
    ref_centered=col_sub(ref_mat, ref_center);
  } else {
    ref_frame=cg.GetFrame(begin);
  }
  for (int i=begin; i<real_end; ++i) {
    if (ref==i || (ref==-1 && i==begin)) {
      superposed.AddFrame(*ref_frame);
      ref_frame=superposed.GetFrame(superposed.GetFrameCount()-1);
      continue;
    }
    if (ref==-1) {
      for (size_t j=0; j<indices.size(); ++j) {
        ref_mat.row(j)=gvec_to_cvec((*ref_frame)[indices[j]]);
      }
      ref_center=ref_mat.colwise().sum()/ref_mat.rows();
      ref_centered=col_sub(ref_mat, ref_center);
    }
    CoordFramePtr frame=cg.GetFrame(i);
    for (size_t j=0; j<indices.size(); ++j) {
      frame_mat.col(j)=gvec_to_rvec((*frame)[indices[j]]);
    }
    std::vector<geom::Vec3> frame_data=*frame;
    frame_center=frame_mat.rowwise().sum()/frame_mat.cols();
    frame_centered=row_sub(frame_mat, frame_center);
    //single value decomposition
    Eigen::SVD<EMat3> svd(frame_centered*ref_centered);
    EMat3 matrixVT=svd.matrixV().transpose();
    //determine rotation
    Real detv=matrixVT.determinant();
    Real dett=svd.matrixU().determinant();
    Real det=detv*dett;
    EMat3 e_rot;
    if (det<0) {
      EMat3 tmat=EMat3::Identity();
      tmat(2,2)=-1;
      e_rot=(svd.matrixU()*tmat)*matrixVT;
    }else{
      e_rot=svd.matrixU()*matrixVT;
    }
    //  prepare rmsd calculation
    geom::Vec3 shift=rvec_to_gvec(ref_center);
    geom::Vec3 com_vec=-cvec_to_gvec(frame_center);
    geom::Mat3 rot=emat_to_gmat(e_rot);
    geom::Mat4 mat4_com, mat4_rot, mat4_shift;
    mat4_rot.PasteRotation(rot);
    mat4_shift.PasteTranslation(shift);
    mat4_com.PasteTranslation(com_vec);
    geom::Mat4 tf=geom::Mat4(mat4_shift*mat4_rot*mat4_com);
    for (std::vector<geom::Vec3>::iterator c=frame_data.begin(), 
         e2=frame_data.end(); c!=e2; ++c) {
      *c=geom::Vec3(tf*geom::Vec4(*c));
    }
    superposed.AddFrame(frame_data);
    if (ref==-1) {
      ref_frame=superposed.GetFrame(superposed.GetFrameCount()-1);
    }
  }
  return superposed;
}

}}}
