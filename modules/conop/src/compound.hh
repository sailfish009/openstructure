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
#ifndef OST_CONOP_COMPOUND_HH
#define OST_CONOP_COMPOUND_HH

#include <vector>
#include <boost/shared_ptr.hpp>
#include <ost/string_ref.hh>
#include <ost/conop/module_config.hh>

#include <ost/mol/chem_class.hh>

namespace ost { namespace conop {

struct Date {
  Date(int y, int m, int d):
    year(y), month(m), day(d)
  { }
  Date():
    year(1900), month(1), day(1)
  { }
  bool operator<(const Date& date) const
  {
    return year<date.year && month<date.month && day<date.day;
  }
  bool operator==(const Date& date) const
  {
    return year==date.year && month==date.month && day==date.day;
  }  
  
  bool operator!=(const Date& date) const
  {
    return !this->operator==(date);
  }
  
  static Date FromString(const StringRef& str)
  {
    assert(str[4]=='-');
    assert(str[7]=='-');
    std::pair<bool, int> year=str.substr(0,4).to_int();
    std::pair<bool, int> month=str.substr(5,2).to_int();
    std::pair<bool, int> day=str.substr(8, 2).to_int();
    assert(year.first); assert(month.first); assert(day.first);
    return Date(year.second, month.second, day.second);
  }
  
  int year;
  int month;
  int day;  
};

struct AtomSpec {
  AtomSpec()
    : ordinal(0), is_leaving(false) {
  }
  int    ordinal;
  String name;
  String alt_name;
  String element;
  bool   is_leaving;
  bool   is_aromatic;
  bool operator==(const AtomSpec& rhs) const {
    return ordinal==rhs.ordinal && name==rhs.name && alt_name==rhs.alt_name &&
           element==rhs.element && is_leaving==rhs.is_leaving && 
           rhs.is_aromatic==rhs.is_aromatic;
  }
  bool operator!=(const AtomSpec& rhs) const {
    return !this->operator==(rhs);
  }
};

struct BondSpec {
  BondSpec()
    : atom_one(0), atom_two(0), order(1) {

  }

  bool operator==(const BondSpec& rhs) const {
    return atom_one==rhs.atom_one && atom_two==rhs.atom_two;
  }

  bool operator!=(const BondSpec& rhs) const {
    return !this->operator==(rhs);
  }
  int atom_one;
  int atom_two;
  int order;
};

typedef std::vector<AtomSpec> AtomSpecList;
typedef std::vector<BondSpec> BondSpecList;
class Compound;
typedef boost::shared_ptr<Compound> CompoundPtr;


/// \brief Knows about the atoms and bonds of a chemical compounds
class DLLEXPORT_OST_CONOP Compound {
public:
  Compound(const String& id)
    : olc_('?'), tlc_(id), chem_class_() {
  }

  /// \brief three-letter code that is unique for every compound 
  const String& GetID() const {
    return tlc_;
  }

  void SetOneLetterCode(char olc) {
    olc_=olc;
  }

  /// \brief one letter code, if available.
  ///
  /// For compounds without a defined one-letter code, the code is set to "?".
  /// One letter codes are ambigous. For instance, both glycine and guanosine
  /// have G as their one-letter code.
  char GetOneLetterCode() const {
    return olc_;
  }

  void SetChemClass(mol::ChemClass chem_class) {
    chem_class_=chem_class;
  }

  mol::ChemClass GetChemClass() const {
    return chem_class_;
  }

  bool IsPeptideLinking() const {
    return chem_class_.IsPeptideLinking();
  }
  void AddAtom(const AtomSpec& atom) {
    atom_specs_.push_back(atom);
  }

  void AddBond(const BondSpec& bond) {
    bond_specs_.push_back(bond);
  }

  const AtomSpecList& GetAtomSpecs() const {
    return atom_specs_;
  }
  
  int GetAtomSpecIndex(const String& name) const;
  
  const String& GetFormula() { return formula_; }
  
  void SetFormula(const String& formula) { formula_=formula; }
  
  const BondSpecList& GetBondSpecs() const {
    return bond_specs_;
  }
  const Date& GetModificationDate() const 
  { 
    return mod_date_; 
  }
  const Date& GetCreationDate() const 
  { 
    return creation_date_; 
  }  
  
  void SetModificationDate(const Date& mod_date) 
  { 
    mod_date_=mod_date; 
  }
  
  void SetCreationDate(const Date& creation_date) 
  { 
    creation_date_=creation_date; 
  }  
private:
  Compound();
  char                         olc_;
  String                       tlc_;
  String                       formula_;
  AtomSpecList                 atom_specs_;
  BondSpecList                 bond_specs_;
  mol::ChemClass               chem_class_;
  Date                         creation_date_;
  Date                         mod_date_;
};

}}

#endif
