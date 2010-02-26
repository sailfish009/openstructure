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
#ifndef OST_QA_RAPDF_HH
#define OST_QA_RAPDF_HH

#include <ost/mol/entity_view.hh>
#include <boost/shared_ptr.hpp>
#include <boost/scoped_ptr.hpp>
#include <ost/mol/entity_visitor.hh>
#include <ost/qa/interaction_statistics.hh>

namespace ost { namespace qa {

typedef MultiClassifier<Real, int, int, Real> InteractionEnergies;
class RAPDF;

typedef boost::shared_ptr<RAPDF> RAPDFPtr;

/// \brief RAPDF after (Samudrala & Moult, 1998)
class DLLEXPORT_OST_QA RAPDF : public EntityVisitor {
public:
  /// load RAPDF potential from file.
  RAPDF(const String& file);
  RAPDF(const RAPDF& rhs);
  /// create new interaction potential from interaction statistics
  ///  \param statistics interaction statistics
  ///  \param sigma  mixing parameter (see Sippl et al., 1990)
  RAPDF(InteractionStatisticsPtr statistics, Real sigma=50.0);
  
  /// calculate all-atom interaction score for entity. all atoms of the
  /// entity are taken into account, but only energies from atoms in the
  /// view contribute to the energy.
  /// not thread-safe (just factor out visitor to make thread-safe)
  Real GetTotalEnergy(mol::EntityView entity);  
  
  virtual bool VisitAtom(const mol::AtomHandle& atom);
  virtual bool VisitResidue(const mol::ResidueHandle& residue);
  
private:
  boost::scoped_ptr<InteractionEnergies>    energies_;  
  AminoAcid                                 amino_acid_;
  int                                       rnumber_;
  Real                                    tmp_energy_;
};

}}

#endif
