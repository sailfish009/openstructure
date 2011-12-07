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
#ifndef OST_IO_IO_PROFILE_HH
#define OST_IO_IO_PROFILE_HH

#include <iostream>
#include <map>
#include <ost/mol/entity_handle.hh>
#include <ost/io/module_config.hh>
namespace ost { namespace io {

struct DLLEXPORT IOProfile {
public:
  IOProfile(String d, bool sh, bool qm, bool ft, bool js, bool nh, bool co, bool bf):
    dialect(d), strict_hydrogens(sh), quack_mode(qm), fault_tolerant(ft),
    join_spread_atom_records(js), no_hetatms(nh), calpha_only(co), bond_feasibilty(bf)
  { }
  IOProfile(): dialect("PDB"), strict_hydrogens(true), quack_mode(false),
    fault_tolerant(false), join_spread_atom_records(false), no_hetatms(false),
    calpha_only(false), bond_feasibilty(true)
  { }
  virtual ~IOProfile() { }

  String dialect;
  bool   strict_hydrogens;
  bool   quack_mode;
  bool   fault_tolerant;
  bool   join_spread_atom_records;
  bool   no_hetatms;
  bool   calpha_only;
  bool   bond_feasibilty;

  IOProfile Copy()
  {
    return IOProfile(dialect, strict_hydrogens, quack_mode, fault_tolerant,
                     join_spread_atom_records, no_hetatms, calpha_only, bond_feasibilty);
  }
  virtual void PostImport(mol::EntityHandle ent) { }
};

inline  std::ostream& operator<<(std::ostream& stream, const IOProfile& p)
{
  stream << "IOProfile(dialect='" << p.dialect << "', strict_hydrogens="
         << (p.strict_hydrogens ? "True" : "False") << ", quack_mode="
         << (p.quack_mode ? "True" : "False") << ", join_spread_atom_records="
         << (p.join_spread_atom_records ? "True" : "False") << ", no_hetatms="
         << (p.no_hetatms ? "True" : "False") << ", calpha_only="
         << (p.calpha_only ? "True" : "False") << ", fault_tolerant="
         << (p.fault_tolerant ? "True" : "False") << ", bond_feasibilty="
	 << (p.bond_feasibilty ? "True" : "False") << ")";
  return stream;
}

class DLLEXPORT_OST_IO IOProfileRegistry {
public:
  static IOProfileRegistry& Instance();
  
  IOProfile& Get(const String& key) 
  { 
    return profiles_[key];
  }
  
  void Set(const String& key, const IOProfile& profile)
  {
    profiles_[key]=profile;
  }
  
  IOProfile& GetDefault() { return profiles_["DEFAULT"]; }
private:
  IOProfileRegistry();
  std::map<String, IOProfile> profiles_;
};

}}

#endif
