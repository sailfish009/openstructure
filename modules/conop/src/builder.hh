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
#ifndef OST_CONOP_BUILDER_HH
#define OST_CONOP_BUILDER_HH

/*
   Author: Marco Biasini
 */
#include <ost/mol/mol.hh>

#include <ost/conop/module_config.hh>
#include <ost/conop/builder_fw.hh>

namespace ost { namespace conop {

/// \brief  abstract builder interface
/// 
/// A builder serves several purposes: \li knows how to connect atoms of
/// residues based on their name \li is able to identify as residue based on 
/// its name \li knows whether a residue has all required atoms \li assigns
/// named torsions to residues \li knows when to connect two consecutive
/// residues.
/// 
/// The exact behaviour for a builder is implementation-specific. While some
/// builders implement sophisticated checks to identity residues, other builders
/// may only perform a name lookup. For two specific implementations of
/// builders, HeuristicBuilder and RuleBasedBuilder.
/// 
/// Custom builders can be registered with Conopology::RegisterBuilder(). The
/// default builder is set with Conopology::SetDefaultBuilder(). Since all
/// entity loaders make use of the default builder to infer connectivity of the
/// atoms, changing the default builder can largely affect the loading
///  behaviour.
class DLLEXPORT_OST_CONOP Builder {
public:
public:
  virtual ~Builder();
  
  ///  \brief add any missing atoms to the residue based on its key,
  ///      with coordinates set to zero
  virtual void CompleteAtoms(mol::ResidueHandle rh);


  /// \brief verify that the given residue has all atoms it
  ///     is supposed to have based on its key
  virtual void CheckResidueCompleteness(const mol::ResidueHandle& rh);

  /// \brief Check whether the residue has all atoms it is supposed to have
  /// \todo Add hydrogen flag
  virtual bool IsResidueComplete(const mol::ResidueHandle& rh);
  
  /// \brief attempt to identify the residue based on its atoms, and return a 
  ///    suggestion for the proper residue key
  virtual mol::ResidueKey IdentifyResidue(const mol::ResidueHandle& rh);

  /// \brief Assign (correct) residue properties
  /// 
  /// Assign chemical class of the residue and one letter code.
  virtual void FillResidueProps(mol::ResidueHandle residue);

  virtual void FillAtomProps(mol::AtomHandle atom);
  /// \brief connect atoms of one residue
  /// 
  /// Connects atoms of residue based on residue and atom name. This method does
  /// not establish inter-residue bonds. To connect atoms that belong to 
  /// different residues, use ConnectResidueToPrev(), or ConnectResidueToNext().
  virtual void ConnectAtomsOfResidue(mol::ResidueHandle rh);
  /// \brief connect atoms of residue to previous
  /// 
  /// The order of the parameters is important. In case of a polypeptide chain,
  /// the residues are thought to be ordered from N- to C- terminus.
  /// 
  /// \sa ConnectResidueToNext
  virtual void ConnectResidueToPrev(mol::ResidueHandle rh,
                                    mol::ResidueHandle prev);
  /// \sa ConnectResidueToPrev                                    
  virtual void ConnectResidueToNext(mol::ResidueHandle rh, 
                                    mol::ResidueHandle next);

  /// \brief assign named torsions to a complete chain
  virtual void AssignTorsions(mol::ChainHandle ch);
  
  /// \brief assign named torsions to single residue
  virtual void AssignTorsionsToResidue(mol::ResidueHandle residue);
  
  /// \brief assign Backbone torsions to single residue  
  void AssignBackBoneTorsionsToResidue(mol::ResidueHandle res);
  
  /// \brief  Check if peptide bond is formed between the two atoms.
  /// 
  /// This method is called by ConnectResidueWithNext() after making sure that
  /// both residues participating in the peptide bond are peptide linking
  /// components.
  /// 
  /// By default, IsBondFeasible() is used to check whether the two atoms form
  /// a peptide bond.
  virtual bool DoesPeptideBondExist(const mol::AtomHandle& n, 
                                    const mol::AtomHandle& c);
  
  /// \brief Overloadable hook to check if bond between to atoms is feasible
  /// 
  /// The default implementation uses a distance-based check to check if the
  /// two atoms should be connected. The atoms are connected if they are in
  /// the range of 0.8 to 1.2 times their van-der-WAALS radius.
  virtual bool IsBondFeasible(const mol::AtomHandle& atom_a,
                              const mol::AtomHandle& atom_b);  

  /// \brief guess element of atom based on name and hetatm flag
  static String GuessAtomElement(const String& atom_name, bool hetatm);
   
  /// \brief whether the r1 and r2 have consecutive residue numbers
  static bool AreResiduesConsecutive(const mol::ResidueHandle& r1, 
                                     const mol::ResidueHandle& r2);

  /// |brief Connect \p atom with all atoms for whith IsBondFeasible() and 
  ///    AreResiduesConsecutive() returns true
  void DistanceBasedConnect(mol::AtomHandle atom);
};


}} // ns

#endif
