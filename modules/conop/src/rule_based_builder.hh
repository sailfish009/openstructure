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
#ifndef OST_CONOP_RULE_BASED_BUILDER_HH
#define OST_CONOP_RULE_BASED_BUILDER_HH

/*
   Author: Marco Biasini
 */
#include <set>
#include <map>

#include "builder.hh"
#include "compound_lib.hh"

namespace ost { namespace conop {

/// \brief  Use library of chemical compounds to derive bonds and clean-up
///         structural problems
///
/// \todo   Sorting of the residues directly uses impl layer of the base module.
///         Would be much nicer if the order could be assigned in the public
///         interface and then tell the residues to sort the atoms in ascending
///         order.
class DLLEXPORT_OST_CONOP RuleBasedBuilder: public Builder {
public:
  RuleBasedBuilder(const CompoundLibPtr& compound_lib)
    : compound_lib_(compound_lib), last_compound_() {
  }
  
  virtual void SetDialect(Dialect dialect) {
    if (this->GetDialect()!=dialect) {
      Builder::SetDialect(dialect);
      last_compound_=CompoundPtr();
      compound_lib_->ClearCache();
    }
  }
  
  /// \brief fill atom properties such as element and radius
  virtual void FillAtomProps(mol::AtomHandle atom);
  
  virtual void CompleteAtoms(mol::ResidueHandle rh);

  /// \brief  Check residue completeness
  ///
  ///  By using the description of the chemical compound, the completeness of
  /// the residue is verified. The method distinguishes between required atoms
  /// and atoms that are optional, like OXT that is only present, if not peptide
  /// bond is formed. Whenever an unknown atom is encountered, OnUnknownAtom()
  /// is invoked. Subclasses of the RuleBasedBuilder may implement some
  /// additional logic to deal with unknown atom. Likewise, whenever a required
  /// atom is missing, OnMissingAtom() is invoked. Hydrogen atoms are not
  /// considered as required by default
  virtual void CheckResidueCompleteness(const mol::ResidueHandle& rh);

  /// \brief Identify residue by name
  ///
  /// Looks-up the residue in the database of chemical compounds and returns the
  /// name of the residue or "UNK" if the residue has not been found in the
  /// library.
  virtual mol::ResidueKey IdentifyResidue(const mol::ResidueHandle& rh);


  virtual void ConnectAtomsOfResidue(mol::ResidueHandle rh);

  /// \brief  Connects the two residues together.
  ///
  /// Connections are established if both of the residues are peptide-linking
  /// components and when PeptideBondExists() returns true.
  /// \param rh
  ///          is the N-terminal partner donating the C and O for the peptide
  ///         bond.
  /// \param next
  ///          is the C-terminal partner, donating the nitrogen to the bond.
  virtual void ConnectResidueToNext(mol::ResidueHandle rh,
                                    mol::ResidueHandle next);
  /// \brief requires chemical types
  virtual void AssignTorsions(mol::ChainHandle ch);
  virtual void AssignTorsionsToResidue(mol::ResidueHandle residue);
  /// \brief  Invoked whenever an unkknown atom has been encountered during a
  ///         residue completeness check.
  ///
  /// The default implementation guesses the atom properties based on the name 
  /// and returns false, meaning that it should be treated as an unknown atom.
  /// 
  /// Custom implementations of this method may delete the atom, or modify it.
  /// \todo   what should be done when the atom name is changed? it would be
  ///         neccessary to rerun CheckResidueCompleteness().
  virtual bool OnUnknownAtom(mol::AtomHandle atom);

  /// \brief Invoked whenever an atom is missing
  ///
  /// It is up to the overloaded method to deal with the missing atom, either
  /// by ignoring it or by inserting a dummy atom.
  virtual void OnMissingAtom(const mol::ResidueHandle& residue,
                              const String& atom_name) { }

  /// \brief Fill in missing information based on atom name.
  virtual void FillAtomProps(mol::AtomHandle atom, const AtomSpec& spec);

  /// \brief Set residue properties such as chemical class
  virtual void FillResidueProps(mol::ResidueHandle residue);

  /// \brief whether the residue has unknown atoms
  bool HasUnknownAtoms(mol::ResidueHandle res);                              
  /// \brief Check whether the residue has all required atoms. This does not
  ///        include hydrogens and leaving atoms such as the terminal OXT.
  virtual bool IsResidueComplete(const mol::ResidueHandle& residue);
private:
  CompoundLibPtr      compound_lib_;
  CompoundPtr         last_compound_;
  mol::ResidueHandle  last_residue_;
  bool                unknown_atoms_;
  void LookupCompound(const mol::ResidueHandle& rh);
  /// Change internal order of atoms in residue to the order given by compound
  void ReorderAtoms(mol::ResidueHandle residue, CompoundPtr compound);

  mol::AtomHandle LocateAtom(const mol::AtomHandleList& ahl, int ordinal);

  void AssignBackBoneTorsionsToResidue(mol::ResidueHandle residue);


};



}}

#endif

