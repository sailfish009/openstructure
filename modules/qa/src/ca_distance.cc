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
#include "ca_distance.hh"
#include <ost/geom/geom.hh>
#include <Eigen/Array>
/*
  Author: Marco Biasini
 */

namespace ost { namespace qa {

namespace {
class DistanceCalculator : public mol::EntityVisitor {
public:
  DistanceCalculator(const mol::AtomHandle& atom_handle,
                     Eigen::MatrixXd mat)
    : atom_a_(atom_handle), distance_mat_(mat) {

  }
  virtual bool VisitAtom(const mol::AtomHandle& atom) {
    Real d=geom::Length(atom_a_.GetPos()-atom.GetPos());
    int i=atom_a_.GetResidue().GetNumber().GetNum();
    int j=atom.GetResidue().GetNumber().GetNum();
    distance_mat_(i-1, j-1)= d;
    return true;
  }
private:
  mol::AtomHandle  atom_a_;
  Eigen::MatrixXd distance_mat_;
};

}

mol::ResNum CalphaDistance::GetBiggestResNumber(const mol::EntityView& e) const {
  assert(e.GetChainCount()==1);
  return e.GetChainList().front().GetResidueList().back().GetNumber();
}

CalphaDistancePtr CalculateCalphaDistance(const mol::EntityView& a,
                                          const mol::EntityView& b) {
  if (a.GetChainCount()!=1 || b.GetChainCount()!=1) {
    throw Error("One or more views have chain count not equal to 1");
  }
  return CalphaDistancePtr(new CalphaDistance(a, b));
}

CalphaDistance::CalphaDistance(const mol::EntityView& a, const mol::EntityView& b)
  : a_(a), b_(b), distance_mat_() {
  assert(a_.GetChainCount()==1);
  assert(b_.GetChainCount()==1);
  mol::ResNum last_a=this->GetBiggestResNumber(a_);
  mol::ResNum last_b=this->GetBiggestResNumber(b_);
  distance_mat_=Eigen::MatrixXd::Constant(last_a.GetNum(), last_b.GetNum(), -1);
  this->CalculateDistances();
}

void CalphaDistance::CalculateDistances() {
  a_.Apply(*this);
}

bool CalphaDistance::IsDistanceDefined(const mol::ResNum& number_a,
                                       const mol::ResNum& number_b) const {
  int i=number_a.GetNum();
  int j=number_b.GetNum();
  return (1<=i && 1<=j && static_cast<int>(distance_mat_.cols())>=i &&
          static_cast<int>(distance_mat_.rows())>=j &&
          distance_mat_(i-1, j-1)>=0.0);

}


bool CalphaDistance::VisitAtom(const mol::AtomHandle& atom) {
  DistanceCalculator dist_calc(atom, distance_mat_);
  b_.Apply(dist_calc);
  return true;
}

Real CalphaDistance::GetDistance(const mol::ResNum& number_a,
                                   const mol::ResNum& number_b) const {
  if (this->IsDistanceDefined(number_a, number_b))
    return distance_mat_(number_a.GetNum()-1,
                         number_b.GetNum()-1);
  throw Error("distance between C-alpha atoms is undefined");
}

}}

