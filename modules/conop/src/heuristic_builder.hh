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
#ifndef OST_CONOP_HEURISTIC_BUILDER_HH
#define OST_CONOP_HEURISTIC_BUILDER_HH

#include <set>
#include <map>

#include <ost/mol/chem_class.hh>

#include "builder.hh"

namespace ost { namespace conop {

namespace detail {

// internally used class
class DLLEXPORT_OST_CONOP ConnResEntry {
public:
  typedef std::pair<String,String> NamePair;
  typedef std::multiset<NamePair> PairSet;
  struct TorsionEntry {
    String a[4];
    String name;
  };
  typedef std::vector<TorsionEntry> TorsionEntryList;

public:
  ConnResEntry(const String& rname="", char single='\0',
               const mol::ChemClass& chem_class=mol::ChemClass(mol::ChemClass::UNKNOWN));
  int Check(const String& name1, const String& name2) const;
  bool HasAtom(const String& name);
  void AddAtom(const String& atom) { required_atoms_.push_back(atom); }
  void AddConn(const String& n1, const String& n2);
  void AddTors(const String& a1, const String& a2,
               const String& a3, const String& a4, const String& name);
  void SetPrev(const String& n);
  const String& GetPrev() const;
  void SetNext(const String& n);
  const String& GetNext() const;
  TorsionEntryList GetTorsionList() const;
  char GetOneLetterCode() const {return abbrev_;}
  mol::ChemClass GetChemClass() const { return chem_class_; }
  
  std::vector<String> GetReqAtoms() const { return required_atoms_;}
private:
  mol::ResidueKey rkey_;
  char       abbrev_;
  mol::ChemClass  chem_class_;
  PairSet pset_;
  String prev_;
  String next_;
  std::vector<String> required_atoms_;
  TorsionEntryList torsion_entry_list_;
};

typedef std::map<mol::ResidueKey,ConnResEntry> ConnResEntryMap;

} // ns

class DLLEXPORT_OST_CONOP HeuristicBuilder: public Builder {
public:
  HeuristicBuilder();
  virtual ~HeuristicBuilder();

  virtual void AssignTorsions(mol::ChainHandle ch);

  //! \brief connect by using information in the heuristic connectivity table.
  //
  // This does not make any assumption on the correctness of internal or external
  // coordinates. Only the atom names and residue need to be known.
  virtual void ConnectAtomsOfResidue(mol::ResidueHandle rh);
  
  virtual bool IsResidueComplete(const mol::ResidueHandle& rh);
//  virtual char GetOneLetterCode(const mol::ResidueKey& residue) const;

  //!\brief connect to previous residue in chain.
  virtual void ConnectResidueToPrev(mol::ResidueHandle rh, 
                                    mol::ResidueHandle prev);
  //!
  virtual void AssignTorsionsToResidue(mol::ResidueHandle residue);

  virtual void FillResidueProps(mol::ResidueHandle residue);

  //!\brief connect to next residue in chain.
  virtual void ConnectResidueToNext(mol::ResidueHandle rh, 
                                    mol::ResidueHandle next);

  //!\brief get connectivity table for residues
  // Retrieve connectiviy information based on residue key. The returned
  // \c pair->second is true, if an entry has been found and and false otherwise.
  std::pair<detail::ConnResEntry,bool> LookupResEntry(const mol::ResidueKey& key);
  
  virtual void FillAtomProps(mol::AtomHandle atom);

  const detail::ConnResEntry& DefaultPeptide() const {return default_peptide_;}
protected:
  void ConnectivityFromAtomNames(const mol::ResidueHandle& res,
                                 detail::ConnResEntry& centry,
                                 mol::AtomHandleList& unknown_atoms);
private:
  detail::ConnResEntryMap emap_;
  detail::ConnResEntry default_peptide_;
};


}} // ns

#endif
