//------------------------------------------------------------------------------
// This file is part of the OpenStructure project <www.openstructure.org>
//
// Copyright (C) 2008-2017 by the OpenStructure authors
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

#ifndef OST_ACCESSIBILITY_HH
#define OST_ACCESSIBILITY_HH

#include <ost/mol/entity_view.hh>
#include <ost/mol/entity_handle.hh>

namespace ost { namespace mol { namespace alg {


class AccessibilityParam {

public:
  // Singleton access to one constant instance
  static const AccessibilityParam& GetInstance() {
    static AccessibilityParam instance;
    return instance;
  }
  // Data access

  Real GuessRadius(const String& ele) const;

  Real GetVdWRadius(const String& rname, const String& aname, 
                    const String& ele) const;

  Real GetResidueAccessibility(const String& rname) const;

private:

  // construction only inside here
  AccessibilityParam();

  std::map<String, std::map<String, Real> > vdw_radii_;
  std::map<String, Real> accessibilities_;
};

Real Accessibility(ost::mol::EntityView& ent, 
                   Real probe_radius = 1.4,
                   bool include_hydrogens = false,  
                   bool include_hetatm = false,
                   bool include_water = false,
                   bool oligo_mode = false,
                   const String& selection = "",
                   const String& asa_abs = "asaAbs",
                   const String& asa_rel = "asaRel",
                   const String& asa_atom = "asaAtom");


Real Accessibility(ost::mol::EntityHandle& ent, 
                   Real probe_radius = 1.4,
                   bool include_hydrogens = false,
                   bool include_hetatm = false,
                   bool include_water = false,
                   bool oligo_mode = false,
                   const String& selection = "",
                   const String& asa_abs = "asaAbs",
                   const String& asa_rel = "asaRel",
                   const String& asa_atom = "asaAtom");

}}} //ns

#endif
