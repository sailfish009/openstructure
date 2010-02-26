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
#ifndef OST_CONOP_SANITIZER_HH
#define OST_CONOP_SANITIZER_HH

#include <ost/mol/mol.hh>
#include <ost/conop/compound_lib.hh>

namespace ost { namespace conop {

/// \brief Structural cleanup
/// 
/// \todo Sanitizer acts both as the checker and the instance responsible for
///       cleaning up structural problems. Is this desired?
class DLLEXPORT_OST_CONOP Sanitizer : public mol::EntityVisitor {
public:
  Sanitizer(const CompoundLibPtr& compound_lib);
  
  virtual bool VisitAtom(const mol::AtomHandle& atom);
  
  virtual bool VisitResidue(const mol::ResidueHandle& residue);
  
  virtual bool VisitChain(const mol::ChainHandle& chain);
  
  virtual CompoundPtr OnUnknownResidue(const mol::ResidueHandle& residue) {
    return CompoundPtr();
  }
  
  virtual void OnUnknownAtom(const mol::AtomHandle& atom) { }
  virtual void OnMissingAtom(const mol::ResidueHandle& residue,
                             const String& atom_name) { }
private:
  void FillAtomProps(mol::AtomHandle atom, const AtomSpec& spec);
  /// Change internal order of atoms in residue to the order given by compound
  void ReorderAtoms(const mol::ResidueHandle& residue, CompoundPtr compound);
  /// Verify all atoms are present. 
  /// 
  /// The check exludes hydrogen atoms as well as "leaving atoms", i.e.
  /// carboxy-terminal atoms that leave when the peptide bond is formed.
  
  void VerifyCompleteness(const mol::ResidueHandle& residue, CompoundPtr compound);
  CompoundPtr IdentifyResidue(const mol::ResidueHandle& residue);
  

  CompoundLibPtr     compound_lib_;
};

}}
#endif
