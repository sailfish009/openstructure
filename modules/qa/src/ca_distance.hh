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
#ifndef OST_QA_CA_DISTANCE_HH
#define OST_QA_CA_DISTANCE_HH

/*
  Author: Marco Biasini
 */

#include <ost/mol/mol.hh>
#include <ost/message.hh>
#include <Eigen/Core>
#include <boost/scoped_ptr.hpp>
#include <ost/qa/module_config.hh>

namespace ost { namespace qa {

class CalphaDistance;

typedef boost::shared_ptr<CalphaDistance> CalphaDistancePtr;

/// \brief  calculate C-alpha distance matrix
///
/// \param a first entity view. The entity view is required to contain only
///      C-alpha atoms.
///
/// \param b second entity view. The entity view is required to contain only
///      C-alpha atoms.
///
/// \throws Error, if one or both of the two views do not
///     contain exactly one chain
CalphaDistancePtr DLLEXPORT_OST_QA CalculateCalphaDistance(const mol::EntityView& a,
                                                           const mol::EntityView& b);


class DLLEXPORT_OST_QA CalphaDistance : public mol::EntityVisitor {
public:
  CalphaDistance(const mol::EntityView& a, const mol::EntityView& b);

  virtual bool VisitAtom(const mol::AtomHandle& atom);

  /// \brief  Check if distance between two C-alpha is defined
  ///
  /// Check whether the distance between C-alpha of residue with \c number_a
  /// in  the first entity and residue with \c number_b in the second entity is
  /// defined, i.e. if both C-alphas are part of the view.
  ///
  /// \param number_a residue number in first entity view.
  ///
  /// \param number_b residue number in second entity view.
  bool IsDistanceDefined(const mol::ResNum& number_a,
                         const mol::ResNum& number_b) const;

  /// \brief  Get distance between two C-alpha atoms.
  ///
  /// \throws Error when distance between the two C-alpha atoms is
  ///     not defined.
  ///
  /// \sa     IsDistanceDefined
  Real GetDistance(const mol::ResNum& number_a,
                     const mol::ResNum& number_b) const;


private:
  CalphaDistance(const CalphaDistance&);
  mol::ResNum GetBiggestResNumber(const mol::EntityView& e) const;
  void CalculateDistances();

  mol::EntityView  a_;
  mol::EntityView  b_;
  Eigen::MatrixXd distance_mat_;
};

}}

#endif
