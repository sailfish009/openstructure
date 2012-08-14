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
#include <ost/conop/processor.hh>

namespace ost { namespace io {


struct DLLEXPORT IOProfile {
public:
  IOProfile(String d, bool sh, bool qm, bool ft, bool js, bool nh, 
            bool co, bool bf, conop::ProcessorPtr proc=conop::ProcessorPtr()):
    dialect(d), quack_mode(qm), fault_tolerant(ft), join_spread_atom_records(js), 
    no_hetatms(nh), calpha_only(co), processor(proc)
  {
    if (!processor) return;
    processor->SetCheckBondFeasibility(bf);
    processor->SetStrictHydrogens(sh);
    //processor->SetQuackMode(qm);
  }
  IOProfile(): dialect("PDB"), quack_mode(false), fault_tolerant(false), 
    join_spread_atom_records(false), no_hetatms(false),
    calpha_only(false), processor()
  { }

  String              dialect;
  bool                quack_mode;
  bool                fault_tolerant;
  bool                join_spread_atom_records;
  bool                no_hetatms;
  bool                calpha_only;
  conop::ProcessorPtr processor;

  IOProfile Copy()
  {
    return IOProfile(dialect, processor ? processor->GetStrictHydrogens() : false,
                     quack_mode, fault_tolerant, join_spread_atom_records, 
                     no_hetatms, calpha_only,  
                     processor ? processor->GetCheckBondFeasibility() : false,
                     processor ? processor->Copy() : conop::ProcessorPtr());
  }
};


inline  std::ostream& operator<<(std::ostream& stream, const IOProfile& p)
{
#if 0
  stream << "IOProfile(dialect='" << p.dialect << "', strict_hydrogens="
         << (p.strict_hydrogens ? "True" : "False") << ", quack_mode="
         << (p.quack_mode ? "True" : "False") << ", join_spread_atom_records="
         << (p.join_spread_atom_records ? "True" : "False") << ", no_hetatms="
         << (p.no_hetatms ? "True" : "False") << ", calpha_only="
         << (p.calpha_only ? "True" : "False") << ", fault_tolerant="
         << (p.fault_tolerant ? "True" : "False") << ", bond_feasibility_check="
	 << (p.bond_feasibility_check ? "True" : "False") << ")";
#endif
#warning implement me
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
