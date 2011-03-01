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
#ifndef OST_QA_INTERACTION_STATISTICS_HH
#define OST_QA_INTERACTION_STATISTICS_HH
/*
  Author: Marco Biasini
 */
#include <ost/mol/entity_view.hh>
#include <ost/mol/entity_visitor.hh>
#include <ost/qa/module_config.hh>
#include <ost/qa/amino_acids.hh>
#include <ost/qa/histogram.hh>
#include <ost/qa/atom_types.hh>



namespace ost { namespace qa {

/// \brief histogram for interaction histogram
///
/// the meaning of the dimensions is: type of first interaction partner, type
/// of second  interaction partner, distance between the two paterns.
typedef Histogram<int, int, Real>  InteractionHistogram;
class InteractionStatistics;
typedef boost::shared_ptr<InteractionStatistics> InteractionStatisticsPtr;

class DLLEXPORT_OST_QA InteractionStatistics : public mol::EntityVisitor {
public:
  /// \brief  construct new interaction statistics.
  ///
  /// \param   lower_cutoff only interaction parters that are further apart than 
  ///     this cutoff are taken into account.
  /// \param   upper_cutoff only interaction partners below the cutoff are taken 
  ///      into account
  /// \param   bucket_size size of distance bins in Angstroem.
  /// \param   sequence_sep residues that are closer than sequence_sep in 
  ///       sequence are not considered.
  InteractionStatistics(Real lower_cutoff,
                        Real upper_cutoff,
                        Real bucket_size,
                        int    sequence_sep);
                        
  ///  \brief load interaction statistics from file
  static InteractionStatisticsPtr LoadFromFile(const String& file_name);


  /// Extract the interaction potential from the given entity view.
  void Extract(mol::EntityView a, mol::EntityView b);

  void SaveToFile(const String& file_name) const;


  /// \brief Get distance bucket size
  Real GetDistanceBucketSize() const;

  int GetSequenceSeparation() const;

  Real GetUpperCutoff() const;

  Real GetLowerCutoff() const;

  /// \brief  Get number of atoms at the given distance regardless of their atom
  ///         type.
  uint64_t GetCount(Real distance) const;

  /// \brief get number of atoms at given distance by taking their type into
  ///        account.
  uint64_t GetCount(atom::ChemType a, atom::ChemType b, Real distance) const;

  /// \brief get number of atoms in given distance-bin by taking their type into
  ///        account.
  uint64_t GetCount(atom::ChemType a, atom::ChemType b, int distance_bin) const;

  /// \brief get number of atoms at given distance by taking their type into
  ///        account.
  uint64_t GetCount(atom::ChemType a, atom::ChemType b) const;

  /// \brief get number of atoms at given distance by taking their type into
  ///        account.
  void Set(atom::ChemType a, atom::ChemType b, int distance_bin, int counts);

  /// \internal
  template <typename DS>
  void Serialize(DS& ds);
public:
  virtual bool VisitResidue(const mol::ResidueHandle& r);
  virtual bool VisitAtom(const mol::AtomHandle& a);

private:
  InteractionStatistics();

  Real                 lower_cutoff_;
  Real                 lower_sqr_;
  Real                 upper_cutoff_;
  Real                 upper_sqr_;

  Real                 bucket_size_;
  int                    sequence_sep_;
  mol::EntityView        view_a_;
  mol::EntityView        view_b_;
  AminoAcid              amino_acid_;
  InteractionHistogram   histogram_;
};

}}
#endif
